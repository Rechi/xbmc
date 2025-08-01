/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "GUIControl.h"

#include <vector>

class CGUIControlProfiler;
class TiXmlElement;

class CGUIControlProfilerItem
{
public:
  CGUIControlProfiler *m_pProfiler;
  CGUIControlProfilerItem * m_pParent;
  CGUIControl *m_pControl;
  std::vector<CGUIControlProfilerItem *> m_vecChildren;
  std::string m_strDescription;
  int m_controlID;
  CGUIControl::GUICONTROLTYPES m_ControlType;
  unsigned int m_visTime = 0;
  unsigned int m_renderTime = 0;
  int64_t m_i64VisStart = 0;
  int64_t m_i64RenderStart = 0;

  CGUIControlProfilerItem(CGUIControlProfiler *pProfiler, CGUIControlProfilerItem *pParent, CGUIControl *pControl);
  ~CGUIControlProfilerItem();

  void Reset(CGUIControlProfiler *pProfiler);
  void BeginVisibility();
  void EndVisibility();
  void BeginRender();
  void EndRender();
  void SaveToXML(TiXmlElement *parent);
  unsigned int GetTotalTime() const { return m_visTime + m_renderTime; }

  CGUIControlProfilerItem *AddControl(CGUIControl *pControl);
  CGUIControlProfilerItem *FindOrAddControl(CGUIControl *pControl, bool recurse);
};

class CGUIControlProfiler
{
public:
  CGUIControlProfiler(const CGUIControlProfiler& that) = delete;
  CGUIControlProfiler& operator=(const CGUIControlProfiler& that) = delete;

  static CGUIControlProfiler& Instance();
  static bool IsRunning();

  void Start();
  void EndFrame();
  void BeginVisibility(CGUIControl *pControl);
  void EndVisibility(CGUIControl *pControl);
  void BeginRender(CGUIControl *pControl);
  void EndRender(CGUIControl *pControl);
  int GetMaxFrameCount() const { return m_iMaxFrameCount; }
  void SetMaxFrameCount(int iMaxFrameCount) { m_iMaxFrameCount = iMaxFrameCount; }
  void SetOutputFile(const std::string& strOutputFile) { m_strOutputFile = strOutputFile; }
  const std::string& GetOutputFile() const { return m_strOutputFile; }
  bool SaveResults();
  unsigned int GetTotalTime() const { return m_ItemHead.GetTotalTime(); }

  float m_fPerfScale;
private:
  CGUIControlProfiler();
  ~CGUIControlProfiler() = default;

  CGUIControlProfilerItem m_ItemHead;
  CGUIControlProfilerItem* m_pLastItem = nullptr;
  CGUIControlProfilerItem *FindOrAddControl(CGUIControl *pControl);

  static bool m_bIsRunning;
  std::string m_strOutputFile;
  int m_iMaxFrameCount = 200;
  int m_iFrameCount = 0;
};

#define GUIPROFILER_VISIBILITY_BEGIN(x) { if (CGUIControlProfiler::IsRunning()) CGUIControlProfiler::Instance().BeginVisibility(x); }
#define GUIPROFILER_VISIBILITY_END(x) { if (CGUIControlProfiler::IsRunning()) CGUIControlProfiler::Instance().EndVisibility(x); }
#define GUIPROFILER_RENDER_BEGIN(x) { if (CGUIControlProfiler::IsRunning()) CGUIControlProfiler::Instance().BeginRender(x); }
#define GUIPROFILER_RENDER_END(x) { if (CGUIControlProfiler::IsRunning()) CGUIControlProfiler::Instance().EndRender(x); }

