/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "ImusicInfoTagLoader.h"

namespace MUSIC_INFO
{

class CMusicInfoTagLoaderSHN: public IMusicInfoTagLoader
{
public:
  CMusicInfoTagLoaderSHN();
  ~CMusicInfoTagLoaderSHN() override;

  bool Load(const std::string& strFileName,
            CMusicInfoTag& tag,
            EmbeddedArt* art = nullptr) override;
};
}
