/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace EDL
{
  struct Cut;
}

class CFileItem;

class CEdl
{
public:
  CEdl();

  bool ReadEditDecisionLists(const CFileItem& fileItem, float fFramesPerSecond);
  void Clear();

  bool HasCut() const;
  bool HasSceneMarker() const;
  std::string GetInfo() const;
  int GetTotalCutTime() const;
  int RemoveCutTime(int iSeek) const;
  double RestoreCutTime(double dClock) const;

  const std::vector<EDL::Cut>& GetCutList() const { return m_vecCuts; }

  bool InCut(int iSeek, EDL::Cut* pCut = nullptr);
  bool GetNearestCut(bool bPlus, int iSeek, EDL::Cut* pCut) const;

  int GetLastCutTime() const;
  void SetLastCutTime(int iCutTime);

  bool GetNextSceneMarker(bool bPlus, int iClock, int* iSceneMarker);

  static std::string MillisecondsToTimeString(int iMilliseconds);

private:
  int m_iTotalCutTime; // ms
  std::vector<EDL::Cut> m_vecCuts;
  std::vector<int> m_vecSceneMarkers;
  int m_lastCutTime;

  bool ReadEdl(const std::string& strMovie, float fFramesPerSecond);
  bool ReadComskip(const std::string& strMovie, float fFramesPerSecond);
  bool ReadVideoReDo(const std::string& strMovie);
  bool ReadBeyondTV(const std::string& strMovie);
  bool ReadPvr(const CFileItem& fileItem);

  bool AddCut(const EDL::Cut& newCut);
  bool AddSceneMarker(int sceneMarker);

  void MergeShortCommBreaks();
};
