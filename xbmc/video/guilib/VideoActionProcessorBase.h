/*
 *  Copyright (C) 2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "video/guilib/VideoAction.h"

#include <memory>

class CFileItem;

namespace KODI::VIDEO::GUILIB
{
class CVideoActionProcessorBase
{
public:
  explicit CVideoActionProcessorBase(const std::shared_ptr<CFileItem>& item) : m_item(item) {}
  virtual ~CVideoActionProcessorBase() = default;

  bool ProcessDefaultAction();
  bool ProcessAction(Action action);

  bool GetUserCancelled() const { return m_userCancelled; }
  void SetUserCancelled(bool set) { m_userCancelled = set; }

  std::shared_ptr<CFileItem> GetItem() { return m_item; }
  std::shared_ptr<const CFileItem> GetItem() const { return m_item; }

protected:
  virtual Action GetDefaultAction() = 0;
  virtual bool Process(Action action) = 0;

private:
  CVideoActionProcessorBase() = delete;

  std::shared_ptr<CFileItem> m_item;
  bool m_userCancelled{false};
};
} // namespace KODI::VIDEO::GUILIB
