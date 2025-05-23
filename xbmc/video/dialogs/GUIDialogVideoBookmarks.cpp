/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIDialogVideoBookmarks.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "ServiceBroker.h"
#include "Util.h"
#include "application/Application.h"
#include "application/ApplicationComponents.h"
#include "application/ApplicationPlayer.h"
#include "dialogs/GUIDialogContextMenu.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "imagefiles/ImageFileURL.h"
#include "input/actions/Action.h"
#include "input/actions/ActionIDs.h"
#include "messaging/ApplicationMessenger.h"
#include "pictures/Picture.h"
#include "profiles/ProfileManager.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/Crc32.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"
#include "utils/log.h"
#include "video/VideoDatabase.h"
#include "video/VideoFileItemClassify.h"
#include "view/ViewState.h"

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

#define CONTROL_ADD_BOOKMARK           2
#define CONTROL_CLEAR_BOOKMARKS        3
#define CONTROL_ADD_EPISODE_BOOKMARK   4

#define CONTROL_THUMBS                11

using namespace KODI::VIDEO;

CGUIDialogVideoBookmarks::CGUIDialogVideoBookmarks()
  : CGUIDialog(WINDOW_DIALOG_VIDEO_BOOKMARKS, "VideoOSDBookmarks.xml")
{
  m_vecItems = new CFileItemList;
  m_loadType = LOAD_EVERY_TIME;
}

CGUIDialogVideoBookmarks::~CGUIDialogVideoBookmarks()
{
  delete m_vecItems;
}

bool CGUIDialogVideoBookmarks::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_WINDOW_DEINIT:
    {
      CUtil::DeleteVideoDatabaseDirectoryCache();
      Clear();
    }
    break;

  case GUI_MSG_WINDOW_INIT:
    {
      // don't init this dialog if we don't playback a file
      const auto& components = CServiceBroker::GetAppComponents();
      const auto appPlayer = components.GetComponent<CApplicationPlayer>();
      if (!appPlayer->IsPlaying())
        return false;

      CGUIWindow::OnMessage(message);
      Update();
      return true;
    }
    break;

  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (iControl == CONTROL_ADD_BOOKMARK)
      {
        AddBookmark();
        Update();
      }
      else if (iControl == CONTROL_CLEAR_BOOKMARKS)
      {
        ClearBookmarks();
      }
      else if (iControl == CONTROL_ADD_EPISODE_BOOKMARK)
      {
        AddEpisodeBookmark();
        Update();
      }
      else if (m_viewControl.HasControl(iControl))  // list/thumb control
      {
        int iItem = m_viewControl.GetSelectedItem();
        int iAction = message.GetParam1();
        if (iAction == ACTION_DELETE_ITEM)
        {
          Delete(iItem);
        }
        else if (iAction == ACTION_SELECT_ITEM || iAction == ACTION_MOUSE_LEFT_CLICK)
        {
          GotoBookmark(iItem);
        }
      }
    }
    break;
  case GUI_MSG_SETFOCUS:
    {
      if (m_viewControl.HasControl(message.GetControlId()) && m_viewControl.GetCurrentControl() != message.GetControlId())
      {
        m_viewControl.SetFocused();
        return true;
      }
    }
    break;
  case GUI_MSG_REFRESH_LIST:
    {
      switch (message.GetParam1())
      {
      case 0:
        OnRefreshList();
        break;
      default:
        break;
      }
    }
    break;
  }

  return CGUIDialog::OnMessage(message);
}

bool CGUIDialogVideoBookmarks::OnAction(const CAction &action)
{
  switch(action.GetID())
  {
  case ACTION_CONTEXT_MENU:
  case ACTION_MOUSE_RIGHT_CLICK:
    {
      OnPopupMenu(m_viewControl.GetSelectedItem());
      return true;
    }
  }
  return CGUIDialog::OnAction(action);
}

int CGUIDialogVideoBookmarks::ItemToBookmarkIndex(int item) const
{
  if (item < 0 || item >= static_cast<int>(m_vecItems->Size()))
    return -1;

  const std::shared_ptr<CFileItem> fileItem{m_vecItems->Get(item)};

  if (!fileItem->GetProperty("isbookmark").asBoolean(false))
    return -1;

  const int bookmarkIdx{fileItem->GetProperty("bookmark").asInteger32(-1)};
  if (bookmarkIdx < 0 || bookmarkIdx >= static_cast<int>(m_bookmarks.size()))
  {
    CLog::LogF(LOGERROR, "invalid bookmark index {} for {} bookmark(s)", bookmarkIdx,
               m_bookmarks.size());
    return -1;
  }
  return bookmarkIdx;
}

void CGUIDialogVideoBookmarks::OnPopupMenu(int item)
{
  const int bookmarkIdx{ItemToBookmarkIndex(item)};
  if (bookmarkIdx < 0)
    return;

  const CBookmark& bm{m_bookmarks[bookmarkIdx]};

  // highlight the item
  (*m_vecItems)[item]->Select(true);

  CContextButtons choices;
  choices.Add(1, (bm.type == CBookmark::EPISODE
                      ? 20405
                      : 20404)); // "Remove episode bookmark" or "Remove bookmark"

  const int button{CGUIDialogContextMenu::ShowAndGetChoice(choices)};

  // unhighlight the item
  (*m_vecItems)[item]->Select(false);

  if (button == 1)
    Delete(bm);
}

void CGUIDialogVideoBookmarks::Delete(int item)
{
  const int bookmarkIdx{ItemToBookmarkIndex(item)};
  if (bookmarkIdx >= 0)
    Delete(m_bookmarks[bookmarkIdx]);
}

void CGUIDialogVideoBookmarks::Delete(const CBookmark& bm)
{
  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return;

  const std::string path{g_application.CurrentFileItem().GetDynPath()};
  videoDatabase.ClearBookMarkOfFile(path, bm, bm.type);
  videoDatabase.Close();
  CUtil::DeleteVideoDatabaseDirectoryCache();
  Update();
}

void CGUIDialogVideoBookmarks::OnRefreshList()
{
  m_bookmarks.clear();
  std::vector<CFileItemPtr> items;

  // open the d/b and retrieve the bookmarks for the current movie
  m_filePath = g_application.CurrentFileItem().GetDynPath();

  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return;

  videoDatabase.GetBookMarksForFile(m_filePath, m_bookmarks);
  videoDatabase.GetBookMarksForFile(m_filePath, m_bookmarks, CBookmark::EPISODE, true);
  videoDatabase.Close();

  std::unique_lock lock(m_refreshSection);
  m_vecItems->Clear();

  // cycle through each stored bookmark and add it to our list control
  for (unsigned int i = 0; i < m_bookmarks.size(); ++i)
  {
    std::string bookmarkTime;
    if (m_bookmarks[i].type == CBookmark::EPISODE)
      bookmarkTime = StringUtils::Format("{} {} {} {}", g_localizeStrings.Get(20373),
                                         m_bookmarks[i].seasonNumber, g_localizeStrings.Get(20359),
                                         m_bookmarks[i].episodeNumber);
    else
      bookmarkTime = StringUtils::SecondsToTimeString((long)m_bookmarks[i].timeInSeconds, TIME_FORMAT_HH_MM_SS);

    CFileItemPtr item(new CFileItem(StringUtils::Format(g_localizeStrings.Get(299), i + 1)));
    item->SetLabel2(bookmarkTime);
    item->SetArt("thumb", m_bookmarks[i].thumbNailImage);
    item->SetProperty("resumepoint", m_bookmarks[i].timeInSeconds);
    item->SetProperty("playerstate", m_bookmarks[i].playerState);
    item->SetProperty("isbookmark", "true");
    item->SetProperty("bookmark", i);
    items.push_back(item);
  }

  // add chapters if around
  const auto& components = CServiceBroker::GetAppComponents();
  const auto appPlayer = components.GetComponent<CApplicationPlayer>();
  for (int i = 1; i <= appPlayer->GetChapterCount(); ++i)
  {
    std::string chapterName;
    appPlayer->GetChapterName(chapterName, i);

    int64_t pos = appPlayer->GetChapterPos(i);
    std::string time = StringUtils::SecondsToTimeString((long) pos, TIME_FORMAT_HH_MM_SS);

    if (chapterName.empty() ||
        StringUtils::StartsWithNoCase(chapterName, time) ||
        StringUtils::IsNaturalNumber(chapterName))
      chapterName = StringUtils::Format(g_localizeStrings.Get(25010), i);

    CFileItemPtr item(new CFileItem(chapterName));
    item->SetLabel2(time);

    if (CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
            CSettings::SETTING_MYVIDEOS_EXTRACTCHAPTERTHUMBS))
    {
      auto chapterPath = IMAGE_FILES::CImageFileURL::FromFile(m_filePath, "video");
      chapterPath.AddOption("chapter", std::to_string(i));
      item->SetArt("thumb", chapterPath.ToCacheKey());
    }

    item->SetProperty("chapter", i);
    item->SetProperty("resumepoint", static_cast<double>(pos));
    item->SetProperty("ischapter", "true");
    items.push_back(item);
  }

  // sort items by resume point
  std::sort(items.begin(), items.end(), [](const CFileItemPtr &item1, const CFileItemPtr &item2) {
    return item1->GetProperty("resumepoint").asDouble() < item2->GetProperty("resumepoint").asDouble();
  });

  // add items to file list and mark the proper item as selected if the current playtime is above
  int selectedItemIndex = 0;
  double playTime = g_application.GetTime();
  for (auto& item : items)
  {
    m_vecItems->Add(item);
    if (playTime >= item->GetProperty("resumepoint").asDouble())
      selectedItemIndex = m_vecItems->Size() - 1;
  }

  m_viewControl.SetItems(*m_vecItems);
  m_viewControl.SetSelectedItem(selectedItemIndex);
}

void CGUIDialogVideoBookmarks::Update()
{
  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return;

  if (g_application.CurrentFileItem().HasVideoInfoTag() && g_application.CurrentFileItem().GetVideoInfoTag()->m_iEpisode > -1)
  {
    std::vector<CVideoInfoTag> episodes;
    videoDatabase.GetEpisodesByFile(g_application.CurrentFile(),episodes);
    if (episodes.size() > 1)
    {
      CONTROL_ENABLE(CONTROL_ADD_EPISODE_BOOKMARK);
    }
    else
    {
      CONTROL_DISABLE(CONTROL_ADD_EPISODE_BOOKMARK);
    }
  }
  else
  {
    CONTROL_DISABLE(CONTROL_ADD_EPISODE_BOOKMARK);
  }


  m_viewControl.SetCurrentView(DEFAULT_VIEW_ICONS);

  // empty the list ready for population
  Clear();

  OnRefreshList();

  videoDatabase.Close();
}

void CGUIDialogVideoBookmarks::Clear()
{
  m_viewControl.Clear();
  m_vecItems->Clear();
}

void CGUIDialogVideoBookmarks::GotoBookmark(int item)
{
  auto& components = CServiceBroker::GetAppComponents();
  const auto appPlayer = components.GetComponent<CApplicationPlayer>();
  if (item < 0 || item >= m_vecItems->Size() || !appPlayer->HasPlayer())
    return;

  CFileItemPtr fileItem = m_vecItems->Get(item);
  int chapter = static_cast<int>(fileItem->GetProperty("chapter").asInteger());
  if (chapter <= 0)
  {
    appPlayer->SetPlayerState(fileItem->GetProperty("playerstate").asString());
    g_application.SeekTime(fileItem->GetProperty("resumepoint").asDouble());
  }
  else
    appPlayer->SeekChapter(chapter);

  Close();
}

void CGUIDialogVideoBookmarks::ClearBookmarks()
{
  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return;

  const std::string path{g_application.CurrentFileItem().GetDynPath()};
  videoDatabase.ClearBookMarksOfFile(path, CBookmark::STANDARD);
  videoDatabase.ClearBookMarksOfFile(path, CBookmark::RESUME);
  videoDatabase.ClearBookMarksOfFile(path, CBookmark::EPISODE);
  videoDatabase.Close();
  Update();
}

bool CGUIDialogVideoBookmarks::AddBookmark(CVideoInfoTag* tag)
{
  CBookmark bookmark;
  bookmark.timeInSeconds = (int)g_application.GetTime();
  bookmark.totalTimeInSeconds = (int)g_application.GetTotalTime();

  auto& components = CServiceBroker::GetAppComponents();
  const auto appPlayer = components.GetComponent<CApplicationPlayer>();

  if (appPlayer->HasPlayer())
    bookmark.playerState = appPlayer->GetPlayerState();
  else
    bookmark.playerState.clear();

  bookmark.player = g_application.GetCurrentPlayer();

  // create the thumbnail image
  const float aspectRatio{appPlayer->GetRenderAspectRatio()};
  CRect srcRect{}, renderRect{}, viewRect{};
  appPlayer->GetRects(srcRect, renderRect, viewRect);
  const unsigned int srcWidth{static_cast<unsigned int>(srcRect.Width())};
  const unsigned int srcHeight{static_cast<unsigned int>(srcRect.Height())};
  const unsigned int renderWidth{static_cast<unsigned int>(renderRect.Width())};
  const unsigned int renderHeight{static_cast<unsigned int>(renderRect.Height())};
  const unsigned int viewWidth{static_cast<unsigned int>(viewRect.Width())};
  const unsigned int viewHeight{static_cast<unsigned int>(viewRect.Height())};

  if (!srcWidth || !srcHeight || !renderWidth || !renderHeight || !viewWidth || !viewHeight)
    return false;

  const unsigned int orientation{appPlayer->GetOrientation()};
  const bool rotated{orientation == 90 || orientation == 270};

  // FIXME: the renderer sets the scissors to the size of the screen (provided by graphiccontext),
  // limiting the max size of thumbs (for example 4k video played on 1024x768 screen)

  // The advanced setting defines the max size of the largest dimension (depends on orientation)
  const unsigned int maxThumbDim{
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_imageRes};
  unsigned int width{}, height{};

  if (!rotated)
  {
    if (aspectRatio >= 1.0f)
    {
      width = std::min({maxThumbDim, viewWidth, renderWidth, srcWidth});
      height = static_cast<unsigned int>(width / aspectRatio);
    }
    else
    {
      height = std::min({maxThumbDim, viewHeight, renderHeight, srcHeight});
      width = static_cast<unsigned int>(height * aspectRatio);
    }
  }
  else
  {
    // rotation is applied during rendering, switching source width and height
    if (aspectRatio >= 1.0f)
    {
      height = std::min({maxThumbDim, viewHeight, renderHeight, srcWidth});
      width = static_cast<unsigned int>(height / aspectRatio);
    }
    else
    {
      width = std::min({maxThumbDim, viewWidth, renderWidth, srcHeight});
      height = static_cast<unsigned int>(width * aspectRatio);
    }
  }

  uint8_t *pixels = (uint8_t*)malloc(height * width * 4);
  unsigned int captureId = appPlayer->RenderCaptureAlloc();

  appPlayer->RenderCapture(captureId, width, height, CAPTUREFLAG_IMMEDIATELY);
  bool hasImage = appPlayer->RenderCaptureGetPixels(captureId, 1000, pixels, height * width * 4);

  if (hasImage)
  {
    const std::shared_ptr<CProfileManager> profileManager = CServiceBroker::GetSettingsComponent()->GetProfileManager();

    auto crc = Crc32::ComputeFromLowerCase(g_application.CurrentFile());
    bookmark.thumbNailImage =
        StringUtils::Format("{:08x}_{}.jpg", crc, (int)bookmark.timeInSeconds);
    bookmark.thumbNailImage = URIUtils::AddFileToFolder(profileManager->GetBookmarksThumbFolder(), bookmark.thumbNailImage);

    if (!CPicture::CreateThumbnailFromSurface(pixels, width, height, width * 4,
                                                         bookmark.thumbNailImage))
    {
      bookmark.thumbNailImage.clear();
    }
    else
      CLog::Log(LOGERROR,"CGUIDialogVideoBookmarks: failed to create thumbnail");

    appPlayer->RenderCaptureRelease(captureId);
  }
  else
    CLog::Log(LOGERROR,"CGUIDialogVideoBookmarks: failed to create thumbnail 2");

  free(pixels);

  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return false;

  if (tag)
    videoDatabase.AddBookMarkForEpisode(*tag, bookmark);
  else
  {
    const std::string path{g_application.CurrentFileItem().GetDynPath()};
    videoDatabase.AddBookMarkToFile(path, bookmark, CBookmark::STANDARD);
  }
  videoDatabase.Close();
  return true;
}

void CGUIDialogVideoBookmarks::OnWindowLoaded()
{
  CGUIDialog::OnWindowLoaded();
  m_viewControl.Reset();
  m_viewControl.SetParentWindow(GetID());
  m_viewControl.AddView(GetControl(CONTROL_THUMBS));
  m_vecItems->Clear();
}

void CGUIDialogVideoBookmarks::OnWindowUnload()
{
  m_vecItems->Clear();
  CGUIDialog::OnWindowUnload();
  m_viewControl.Reset();
}

CGUIControl *CGUIDialogVideoBookmarks::GetFirstFocusableControl(int id)
{
  if (m_viewControl.HasControl(id))
    id = m_viewControl.GetCurrentControl();
  return CGUIWindow::GetFirstFocusableControl(id);
}

bool CGUIDialogVideoBookmarks::AddEpisodeBookmark()
{
  std::vector<CVideoInfoTag> episodes;
  CVideoDatabase videoDatabase;
  if (!videoDatabase.Open())
    return false;

  videoDatabase.GetEpisodesByFile(g_application.CurrentFile(), episodes);
  videoDatabase.Close();
  if (!episodes.empty())
  {
    CContextButtons choices;
    for (unsigned int i=0; i < episodes.size(); ++i)
    {
      std::string strButton =
          StringUtils::Format("{} {}, {} {}", g_localizeStrings.Get(20373), episodes[i].m_iSeason,
                              g_localizeStrings.Get(20359), episodes[i].m_iEpisode);
      choices.Add(i, strButton);
    }

    int pressed = CGUIDialogContextMenu::ShowAndGetChoice(choices);
    if (pressed >= 0)
    {
      AddBookmark(&episodes[pressed]);
      return true;
    }
  }
  return false;
}



bool CGUIDialogVideoBookmarks::OnAddBookmark()
{
  if (!IsVideo(g_application.CurrentFileItem()))
    return false;

  if (CGUIDialogVideoBookmarks::AddBookmark())
  {
    CServiceBroker::GetGUI()->GetWindowManager().SendMessage(GUI_MSG_REFRESH_LIST, 0, WINDOW_DIALOG_VIDEO_BOOKMARKS);
    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info,
                                          g_localizeStrings.Get(298),   // "Bookmarks"
                                          g_localizeStrings.Get(21362));// "Bookmark created"
    return true;
  }
  return false;
}

bool CGUIDialogVideoBookmarks::OnAddEpisodeBookmark()
{
  bool bReturn = false;
  if (g_application.CurrentFileItem().HasVideoInfoTag() && g_application.CurrentFileItem().GetVideoInfoTag()->m_iEpisode > -1)
  {
    CVideoDatabase videoDatabase;
    if (!videoDatabase.Open())
      return bReturn;
    std::vector<CVideoInfoTag> episodes;
    videoDatabase.GetEpisodesByFile(g_application.CurrentFile(),episodes);
    if (episodes.size() > 1)
    {
      bReturn = CGUIDialogVideoBookmarks::AddEpisodeBookmark();
      if(bReturn)
      {
        CServiceBroker::GetGUI()->GetWindowManager().SendMessage(GUI_MSG_REFRESH_LIST, 0, WINDOW_DIALOG_VIDEO_BOOKMARKS);
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info,
                                              g_localizeStrings.Get(298),   // "Bookmarks"
                                              g_localizeStrings.Get(21363));// "Episode Bookmark created"

      }
    }
    videoDatabase.Close();
  }
  return bReturn;
}
