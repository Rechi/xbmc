/*
 *  Copyright (C) 2005-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "pictures/PictureFileItemClassify.h"

#include "FileItem.h"
#include "utils/URIUtils.h"

namespace KODI::PICTURE
{
bool IsMusicDb(const CFileItem& item)
{
  return URIUtils::IsMusicDb(item.GetPath());
}
} // namespace KODI::PICTURE
