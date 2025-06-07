/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "PictureInfoScanner.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "GUIInfoManager.h"
#include "PictureLibraryQueue.h"
#include "ServiceBroker.h"
#include "URL.h"
#include "dialogs/GUIDialogExtendedProgressBar.h"
#include "filesystem/Directory.h"
#include "filesystem/DirectoryCache.h"
#include "filesystem/MultiPathDirectory.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "interfaces/AnnouncementManager.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "threads/SystemClock.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <chrono>

using namespace XFILE;

namespace PICTURE
{
CPictureInfoScanner::CPictureInfoScanner()
{
  m_bStop = false;
  m_bRunning = false;
  m_handle = NULL;
  m_showDialog = false;
  m_bCanInterrupt = false;
  m_currentItem = 0;
  m_itemCount = 0;
  m_bClean = false;
  m_scanAll = false;
}

CPictureInfoScanner::~CPictureInfoScanner()
{
}

void CPictureInfoScanner::Process()
{
  m_bStop = false;

  try
  {
    if (m_showDialog && !CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
                            CSettings::SETTING_PICTURELIBRARY_BACKGROUNDUPDATE))
    {
      CGUIDialogExtendedProgressBar* dialog =
          (CGUIDialogExtendedProgressBar*)CServiceBroker::GetGUI()->GetWindowManager().GetWindow(
              WINDOW_DIALOG_EXT_PROGRESS);
      if (dialog)
        m_handle = dialog->GetHandle(g_localizeStrings.Get(314));
    }

    // check if we only need to perform a cleaning
    if (m_bClean && m_pathsToScan.empty())
    {
      std::set<int> paths;
      CPictureLibraryQueue::GetInstance().CleanLibrary(paths, false, m_handle);

      if (m_handle)
        m_handle->MarkFinished();
      m_handle = NULL;

      m_bRunning = false;

      return;
    }

    auto tick = std::chrono::steady_clock::now();

    m_database.Open();

    m_bCanInterrupt = true;

    CLog::Log(LOGINFO, "PictureInfoScanner: Starting scan ..");
    CServiceBroker::GetAnnouncementManager()->Announce(ANNOUNCEMENT::PictureLibrary,
                                                       "OnScanStarted");

    // Reset progress vars
    m_currentItem = 0;
    m_itemCount = -1;

    // Database operations should not be canceled
    // using Interupt() while scanning as it could
    // result in unexpected behaviour.
    m_bCanInterrupt = false;

    bool bCancelled = false;
    while (!bCancelled && !m_pathsToScan.empty())
    {
      /*
        * A copy of the directory path is used because the path supplied is
        * immediately removed from the m_pathsToScan set in DoScan(). If the
        * reference points to the entry in the set a null reference error
        * occurs.
        */
      std::string directory = *m_pathsToScan.begin();
      if (!CDirectory::Exists(directory))
      {
        /*
          * Note that this will skip clean (if m_bClean is enabled) if the directory really
          * doesn't exist rather than a NAS being switched off.  A manual clean from settings
          * will still pick up and remove it though.
          */
        CLog::Log(LOGWARNING, "%s directory '%s' does not exist - skipping scan%s.", __FUNCTION__,
                  CURL::GetRedacted(directory).c_str(), m_bClean ? " and clean" : "");
        m_pathsToScan.erase(m_pathsToScan.begin());
      }
      else if (!DoScan(directory))
        bCancelled = true;
    }

    if (!bCancelled)
    {
      if (m_bClean)
        CPictureLibraryQueue::GetInstance().CleanLibrary(m_pathsToClean, false, m_handle);
      else
      {
        if (m_handle)
          m_handle->SetTitle(g_localizeStrings.Get(331));
        m_database.Compress(false);
      }
    }

    CServiceBroker::GetGUI()
        ->GetInfoManager()
        .GetInfoProviders()
        .GetLibraryInfoProvider()
        .ResetLibraryBools();
    m_database.Close();

    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - tick);
    CLog::Log(LOGINFO, "PicktureInfoScanner: Finished scan. Scanning for pickture info took {}s",
              elapsed.count());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "PicktureInfoScanner: Exception while scanning.");
  }

  m_bRunning = false;
  CServiceBroker::GetAnnouncementManager()->Announce(ANNOUNCEMENT::PictureLibrary,
                                                     "OnScanFinished");

  if (m_handle)
    m_handle->MarkFinished();
  m_handle = NULL;
}

void CPictureInfoScanner::Start(const std::string& strDirectory, bool scanAll)
{
  m_strStartDir = strDirectory;
  m_scanAll = scanAll;
  m_pathsToScan.clear();
  m_pathsToClean.clear();

  m_database.Open();
  if (strDirectory.empty())
  { // scan all paths in the database.  We do this by scanning all paths in the db, and crossing them off the list as
    // we go.
    m_database.GetPaths(m_pathsToScan);
  }
  else
  { // scan all the paths of this subtree that is in the database
    std::vector<std::string> rootDirs;
    if (URIUtils::IsMultiPath(strDirectory))
      CMultiPathDirectory::GetPaths(strDirectory, rootDirs);
    else
      rootDirs.push_back(strDirectory);

    for (std::vector<std::string>::const_iterator it = rootDirs.begin(); it < rootDirs.end(); ++it)
    {
      m_pathsToScan.insert(*it);
      std::vector<std::pair<int, std::string>> subpaths;
      m_database.GetSubPaths(*it, subpaths);
      for (std::vector<std::pair<int, std::string>>::iterator it = subpaths.begin();
           it < subpaths.end(); ++it)
        m_pathsToScan.insert(it->second);
    }
  }
  m_database.Close();
  m_bClean =
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_bPictureLibraryCleanOnUpdate;

  m_bRunning = true;
  Process();
}

void CPictureInfoScanner::Stop()
{
  if (m_bCanInterrupt)
    m_database.Interrupt();

  m_bStop = true;
}

bool CPictureInfoScanner::DoScan(const std::string& strDirectory)
{
  if (m_handle)
  {
    m_handle->SetText(g_localizeStrings.Get(20415));
  }

  /*
    * Remove this path from the list we're processing. This must be done prior to
    * the check for file or folder exclusion to prevent an infinite while loop
    * in Process().
    */
  std::set<std::string>::iterator it = m_pathsToScan.find(strDirectory);
  if (it != m_pathsToScan.end())
    m_pathsToScan.erase(it);

  // load subfolder
  CFileItemList items;
  bool foundDirectly = false;
  bool bSkip = false;

  return !m_bStop;
}
} // namespace PICTURE
