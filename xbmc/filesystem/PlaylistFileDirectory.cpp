/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PlaylistFileDirectory.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "URL.h"
#include "filesystem/File.h"
#include "playlists/PlayList.h"
#include "playlists/PlayListFactory.h"

using namespace KODI;

namespace XFILE
{
  CPlaylistFileDirectory::CPlaylistFileDirectory() = default;

  CPlaylistFileDirectory::~CPlaylistFileDirectory() = default;

  bool CPlaylistFileDirectory::GetDirectory(const CURL& url, CFileItemList& items)
  {
    std::unique_ptr<PLAYLIST::CPlayList> pPlayList(PLAYLIST::CPlayListFactory::Create(url));
    if (nullptr != pPlayList)
    {
      // load it
      if (!pPlayList->Load(url.Get()))
        return false; //hmmm unable to load playlist?

      PLAYLIST::CPlayList playlist = *pPlayList;
      // convert playlist items to songs
      for (int i = 0; i < playlist.size(); ++i)
      {
        CFileItemPtr item = playlist[i];
        item->SetProgramCount(i); //! @todo remove this hack for playlist order
        items.Add(item);
      }
    }
    return true;
  }

  bool CPlaylistFileDirectory::ContainsFiles(const CURL& url)
  {
    std::unique_ptr<PLAYLIST::CPlayList> pPlayList(PLAYLIST::CPlayListFactory::Create(url));
    if (nullptr != pPlayList)
    {
      // load it
      if (!pPlayList->Load(url.Get()))
        return false; //hmmm unable to load playlist?

      return (pPlayList->size() > 1);
    }
    return false;
  }

  bool CPlaylistFileDirectory::Remove(const CURL& url)
  {
    return XFILE::CFile::Delete(url);
  }
  } // namespace XFILE
