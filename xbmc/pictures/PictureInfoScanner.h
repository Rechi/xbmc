#pragma once
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

#include "PictureDatabase.h"

class CGUIDialogProgressBarHandle;

namespace PICTURE
{
class CPictureInfoScanner
{
public:
  CPictureInfoScanner();
  virtual ~CPictureInfoScanner();

  /*! \brief Scan a folder using the background scanner
     \param strDirectory path to scan
     \param scanAll whether to scan everything not already scanned (regardless of whether the user normally doesn't want a folder scanned.) Defaults to false.
     */
  void Start(const std::string& strDirectory, bool scanAll = false);
  bool IsScanning() const { return m_bRunning; }
  void Stop();

  //! \brief Set whether or not to show a progress dialog
  void ShowDialog(bool show) { m_showDialog = show; }

protected:
  virtual void Process();
  bool DoScan(const std::string& strDirectory);

  bool m_showDialog;
  CGUIDialogProgressBarHandle* m_handle;
  int m_currentItem;
  int m_itemCount;
  bool m_bStop;
  bool m_bRunning;
  bool m_bCanInterrupt;
  bool m_bClean;
  bool m_scanAll;
  std::string m_strStartDir;
  CPictureDatabase m_database;
  std::set<std::string> m_pathsToScan;
  std::set<int> m_pathsToClean;
};
} // namespace PICTURE
