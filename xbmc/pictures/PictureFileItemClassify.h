/*
 *  Copyright (C) 2005-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

class CFileItem;

namespace KODI::PICTURE
{
//! \brief Check whether an item is a picture database item.
bool IsMusicDb(const CFileItem& item);
} // namespace KODI::PICTURE
