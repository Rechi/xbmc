/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DirectoryFactory.h"

#include "AddonsDirectory.h"
#ifdef HAVE_LIBBLURAY
#include "BlurayDirectory.h"
#endif
#ifdef HAS_OPTICAL_DRIVE
#include "CDDADirectory.h"
#endif // HAS_OPTICAL_DRIVE
#include "DAVDirectory.h"
#ifdef HAS_OPTICAL_DRIVE
#include "DVDDirectory.h"
#endif
#include "EventsDirectory.h"
#include "FTPDirectory.h"
#include "FavouritesDirectory.h"
#include "FileDirectoryFactory.h"
#include "FileItem.h"
#include "HTTPDirectory.h"
#if defined(HAS_ISO9660PP)
#include "ISO9660Directory.h"
#endif
#include "LibraryDirectory.h"
#include "MultiPathDirectory.h"
#include "MusicDatabaseDirectory.h"
#include "MusicSearchDirectory.h"
#ifdef HAS_FILESYSTEM_NFS
#include "NFSDirectory.h"
#endif
#include "PVRDirectory.h"
#include "PictureDatabaseDirectory.h"
#include "PlaylistDirectory.h"
#include "PluginDirectory.h"
#include "RSSDirectory.h"
#include "ResourceDirectory.h"
#include "ServiceBroker.h"
#include "SourcesDirectory.h"
#include "SpecialProtocolDirectory.h"
#include "StackDirectory.h"
#if defined(HAS_UDFREAD)
#include "UDFDirectory.h"
#endif
#ifdef HAS_UPNP
#include "UPnPDirectory.h"
#endif
#include "URL.h"
#include "VideoDatabaseDirectory.h"
#include "XbtDirectory.h"
#ifdef HAS_ZEROCONF
#include "ZeroconfDirectory.h"
#endif
#include "ZipDirectory.h"
#include "addons/VFSEntry.h"
#include "network/Network.h"
#include "network/WakeOnAccess.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#if defined(TARGET_ANDROID)
#include "platform/android/filesystem/APKDirectory.h"
#include "platform/android/filesystem/AndroidAppDirectory.h"
#elif defined(TARGET_DARWIN_TVOS)
#include "platform/darwin/tvos/filesystem/TVOSDirectory.h"
#endif
#ifdef TARGET_POSIX
#include "platform/posix/filesystem/PosixDirectory.h"
#ifdef HAS_FILESYSTEM_SMB
#include "platform/posix/filesystem/SMBDirectory.h"
#endif
#endif
#ifdef TARGET_WINDOWS_STORE
#include "platform/win10/filesystem/WinLibraryDirectory.h"
#elif defined(TARGET_WINDOWS)
#include "platform/win32/filesystem/Win32Directory.h"
#include "platform/win32/filesystem/Win32SMBDirectory.h"
#endif

#include <stdlib.h>

using namespace ADDON;

using namespace XFILE;

/*!
 \brief Create a IDirectory object of the share type specified in a given item path.
 \param item Specifies the item to which the factory will create the directory instance
 \return IDirectory object to access the directories on the share.
 \sa IDirectory
 */
IDirectory* CDirectoryFactory::Create(const CFileItem& item)
{
  CURL curl{item.GetDynPath()};

  // Store the mimetype, allowing the PlayListFactory to set it on the created FileItem
  const std::string& mimeType = item.GetMimeType();
  if (!mimeType.empty())
    curl.SetOption("mimetype", mimeType);

  return Create(curl);
}

/*!
 \brief Create a IDirectory object of the share type specified in \e strPath .
 \param strPath Specifies the share type to access, can be a share or share with path.
 \return IDirectory object to access the directories on the share.
 \sa IDirectory
 */
IDirectory* CDirectoryFactory::Create(const CURL& url)
{
  if (!CWakeOnAccess::GetInstance().WakeUpHost(url))
    return NULL;

  CFileItem item(url.Get(), true);
  IFileDirectory* pDir = CFileDirectoryFactory::Create(url, &item);
  if (pDir)
    return pDir;

  if (!url.GetProtocol().empty() && CServiceBroker::IsAddonInterfaceUp())
  {
    for (const auto& vfsAddon : CServiceBroker::GetVFSAddonCache().GetAddonInstances())
    {
      auto prots = StringUtils::Split(vfsAddon->GetProtocols(), "|");

      if (vfsAddon->HasDirectories() && std::find(prots.begin(), prots.end(), url.GetProtocol()) != prots.end())
        return new CVFSEntryIDirectoryWrapper(vfsAddon);
    }
  }

#ifdef TARGET_POSIX
  if (url.GetProtocol().empty() || url.IsProtocol("file"))
  {
#if defined(TARGET_DARWIN_TVOS)
    if (CTVOSDirectory::WantsDirectory(url))
      return new CTVOSDirectory();
#endif
    return new CPosixDirectory();
  }
#elif defined(TARGET_WINDOWS)
  if (url.GetProtocol().empty() || url.IsProtocol("file")) return new CWin32Directory();
#else
#error Local directory access is not implemented for this platform
#endif
  if (url.IsProtocol("special")) return new CSpecialProtocolDirectory();
  if (url.IsProtocol("sources")) return new CSourcesDirectory();
  if (url.IsProtocol("addons")) return new CAddonsDirectory();
#if defined(HAS_OPTICAL_DRIVE)
  if (url.IsProtocol("cdda")) return new CCDDADirectory();
#endif
#if defined(HAS_ISO9660PP)
  if (url.IsProtocol("iso9660")) return new CISO9660Directory();
#endif
#if defined(HAS_UDFREAD)
  if (url.IsProtocol("udf")) return new CUDFDirectory();
#endif
  if (url.IsProtocol("plugin")) return new CPluginDirectory();
#if defined(TARGET_ANDROID)
  if (url.IsProtocol("apk")) return new CAPKDirectory();
#endif
  if (url.IsProtocol("zip")) return new CZipDirectory();
  if (url.IsProtocol("xbt")) return new CXbtDirectory();
  if (url.IsProtocol("multipath")) return new CMultiPathDirectory();
  if (url.IsProtocol("stack")) return new CStackDirectory();
  if (url.IsProtocol("playlistmusic")) return new CPlaylistDirectory();
  if (url.IsProtocol("playlistvideo")) return new CPlaylistDirectory();
  if (url.IsProtocol("musicdb")) return new CMusicDatabaseDirectory();
  if (url.IsProtocol("musicsearch")) return new CMusicSearchDirectory();
  if (url.IsProtocol("videodb")) return new CVideoDatabaseDirectory();
  if (url.IsProtocol("library")) return new CLibraryDirectory();
  if (url.IsProtocol("favourites")) return new CFavouritesDirectory();
#if defined(TARGET_ANDROID)
  if (url.IsProtocol("androidapp")) return new CAndroidAppDirectory();
#endif
#ifdef HAVE_LIBBLURAY
  if (url.IsProtocol("bluray")) return new CBlurayDirectory();
#endif
  if (url.IsProtocol("resource")) return new CResourceDirectory();
  if (url.IsProtocol("events")) return new CEventsDirectory();
#ifdef TARGET_WINDOWS_STORE
  if (CWinLibraryDirectory::IsValid(url)) return new CWinLibraryDirectory();
#endif

  if (url.IsProtocol("ftp") || url.IsProtocol("ftps")) return new CFTPDirectory();
  if (url.IsProtocol("http") || url.IsProtocol("https")) return new CHTTPDirectory();
  if (url.IsProtocol("dav") || url.IsProtocol("davs")) return new CDAVDirectory();
#ifdef HAS_FILESYSTEM_SMB
#ifdef TARGET_WINDOWS
  if (url.IsProtocol("smb")) return new CWin32SMBDirectory();
#else
  if (url.IsProtocol("smb")) return new CSMBDirectory();
#endif
#endif
#ifdef HAS_UPNP
  if (url.IsProtocol("upnp")) return new CUPnPDirectory();
#endif
  if (url.IsProtocol("rss") || url.IsProtocol("rsss")) return new CRSSDirectory();
#ifdef HAS_ZEROCONF
  if (url.IsProtocol("zeroconf")) return new CZeroconfDirectory();
#endif
#ifdef HAS_FILESYSTEM_NFS
  if (url.IsProtocol("nfs")) return new CNFSDirectory();
#endif
#ifdef HAS_OPTICAL_DRIVE
  if (url.IsProtocol("dvd"))
    return new CDVDDirectory();
#endif

  if (url.IsProtocol("pvr"))
    return new CPVRDirectory();

  CLog::Log(LOGWARNING, "{} - unsupported protocol({}) in {}", __FUNCTION__, url.GetProtocol(),
            url.GetRedacted());
  return NULL;
}

