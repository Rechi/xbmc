/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "PictureInfoTag.h"
#include "dbwrappers/Database.h"

class CPictureDatabase : public CDatabase
{
public:
  CPictureDatabase(void);
  virtual ~CPictureDatabase(void);

  virtual bool Open();

  // scanning hashes and paths scanned
  bool GetPaths(std::set<std::string>& paths);

  /*! \brief retrieve subpaths of a given path.  Assumes a heirarchical folder structure
  \param basepath the root path to retrieve subpaths for
  \param subpaths the returned subpaths
  \return true if we successfully retrieve subpaths (may be zero), false on error
  */
  bool GetSubPaths(const std::string& basepath, std::vector<std::pair<int, std::string>>& subpaths);

  int AddTag(const std::string& strTag);
  void DeleteTag(int idTag);

private:
  virtual void CreateTables();
  virtual void CreateAnalytics();

  virtual int GetSchemaVersion() const;
  const char* GetBaseDBName() const { return "MyPictures"; };
};
