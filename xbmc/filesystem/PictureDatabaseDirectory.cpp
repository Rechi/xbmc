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

#include "PictureDatabaseDirectory.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "PictureDatabaseDirectory/QueryParams.h"
#include "utils/LegacyPathTranslation.h"

using namespace std;
using namespace XFILE;
using namespace PICTUREDATABASEDIRECTORY;

CPictureDatabaseDirectory::CPictureDatabaseDirectory(void)
{
}

CPictureDatabaseDirectory::~CPictureDatabaseDirectory(void)
{
}

bool CPictureDatabaseDirectory::GetDirectory(const CURL& url, CFileItemList& items)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(url);
  items.SetPath(path);
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return false;

  items.SetLabel(pNode->GetLocalizedName());

  return true;
}

NODE_TYPE CPictureDatabaseDirectory::GetDirectoryChildType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strPath);
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  return pNode->GetChildType();
}

NODE_TYPE CPictureDatabaseDirectory::GetDirectoryType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strPath);
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  return pNode->GetType();
}

NODE_TYPE CPictureDatabaseDirectory::GetDirectoryParentType(const std::string& strPath)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strPath);
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return NODE_TYPE_NONE;

  CDirectoryNode* pParentNode = pNode->GetParent();

  if (!pParentNode)
    return NODE_TYPE_NONE;

  return pParentNode->GetChildType();
}

bool CPictureDatabaseDirectory::GetQueryParams(const std::string& strPath, CQueryParams& params)
{
  std::string path = CLegacyPathTranslation::TranslateVideoDbPath(strPath);
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(path));

  if (!pNode.get())
    return false;

  CDirectoryNode::GetDatabaseInfo(strPath, params);
  return true;
}
