#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "utils/UrlOptions.h"

class CFileItemList;

namespace XFILE
{
namespace PICTUREDATABASEDIRECTORY
{
class CQueryParams;

typedef enum _NODE_TYPE
{
  NODE_TYPE_NONE = 0,
  NODE_TYPE_ROOT,
  NODE_TYPE_TAGS
} NODE_TYPE;

class CDirectoryNode
{
public:
  static CDirectoryNode* ParseURL(const std::string& strPath);
  static void GetDatabaseInfo(const std::string& strPath, CQueryParams& params);
  virtual ~CDirectoryNode();

  NODE_TYPE GetType() const;

  virtual NODE_TYPE GetChildType() const;
  virtual std::string GetLocalizedName() const;

  CDirectoryNode* GetParent() const;

  bool CanCache() const;

protected:
  CDirectoryNode(NODE_TYPE Type, const std::string& strName, CDirectoryNode* pParent);
  static CDirectoryNode* CreateNode(NODE_TYPE Type,
                                    const std::string& strName,
                                    CDirectoryNode* pParent);

  void AddOptions(const std::string& options);
  void CollectQueryParams(CQueryParams& params) const;

  const std::string& GetName() const;
  int GetID() const;
  void RemoveParent();

  virtual bool GetContent(CFileItemList& items) const;

  std::string BuildPath() const;

private:
  NODE_TYPE m_Type;
  std::string m_strName;
  CDirectoryNode* m_pParent;
  CUrlOptions m_options;
};
} // namespace PICTUREDATABASEDIRECTORY
} // namespace XFILE
