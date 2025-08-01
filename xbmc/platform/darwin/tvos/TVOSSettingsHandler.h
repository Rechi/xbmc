/*
 *  Copyright (C) 2010-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "settings/lib/ISettingCallback.h"

class CTVOSInputSettings : public ISettingCallback
{
public:
  CTVOSInputSettings(CTVOSInputSettings const&) = delete;
  CTVOSInputSettings& operator=(CTVOSInputSettings const&) = delete;

  static CTVOSInputSettings& GetInstance();

  void Initialize();

  void OnSettingChanged(const std::shared_ptr<const CSetting>& setting) override;

private:
  CTVOSInputSettings() = default;

  static CTVOSInputSettings* m_instance;
};
