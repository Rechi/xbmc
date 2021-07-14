/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PluginFile.h"

#include "URL.h"

using namespace XFILE;

CPluginFile::CPluginFile() : COverrideFile(false)
{
}

CPluginFile::~CPluginFile() = default;

bool CPluginFile::Exists(const CURL& url)
{
  return true;
}

std::string CPluginFile::TranslatePath(const CURL& url)
{
  return url.Get();
}
