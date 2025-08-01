/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "pvr/IPVRComponent.h"

namespace PVR
{
class CPVRGUIActionsDatabase : public IPVRComponent
{
public:
  CPVRGUIActionsDatabase() = default;
  CPVRGUIActionsDatabase(const CPVRGUIActionsDatabase&) = delete;
  CPVRGUIActionsDatabase const& operator=(CPVRGUIActionsDatabase const&) = delete;
  ~CPVRGUIActionsDatabase() override = default;

  /*!
   * @brief Reset the TV database to it's initial state and delete all the data.
   * @param bResetEPGOnly True to only reset the EPG database, false to reset both PVR and EPG
   * database.
   * @return true on success, false otherwise.
   */
  bool ResetDatabase(bool bResetEPGOnly) const;
};

namespace GUI
{
// pretty scope and name
using Database = CPVRGUIActionsDatabase;
} // namespace GUI

} // namespace PVR
