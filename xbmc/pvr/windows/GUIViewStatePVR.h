/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "view/GUIViewState.h"

class CFileItemList;

namespace PVR
{
class CGUIViewStatePVR : public CGUIViewState
{
public:
  CGUIViewStatePVR(const int windowId, const CFileItemList& items)
    : CGUIViewState(items), m_windowId(windowId)
  {
  }

  int GetWindowId() const { return m_windowId; }

protected:
  bool HideParentDirItems() override { return true; }

private:
  const int m_windowId{-1};
};

class CGUIViewStateWindowPVRChannels : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRChannels(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
};

class CGUIViewStateWindowPVRRecordings : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRRecordings(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
  bool HideParentDirItems() override;
};

class CGUIViewStateWindowPVRGuide : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRGuide(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
};

class CGUIViewStateWindowPVRTimers : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRTimers(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
  bool HideParentDirItems() override;
};

class CGUIViewStateWindowPVRSearch : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRSearch(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
  bool HideParentDirItems() override;
};

class CGUIViewStateWindowPVRProviders : public CGUIViewStatePVR
{
public:
  CGUIViewStateWindowPVRProviders(int windowId, const CFileItemList& items);

protected:
  void SaveViewState() override;
  bool HideParentDirItems() override;
};
} // namespace PVR
