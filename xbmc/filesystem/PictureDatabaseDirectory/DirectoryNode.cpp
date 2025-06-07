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

#include "DirectoryNode.h"

#include "QueryParams.h"
#include "URL.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"

using namespace std;
using namespace XFILE::PICTUREDATABASEDIRECTORY;

//  Parses a given path and returns the current node of the path
CDirectoryNode* CDirectoryNode::ParseURL(const std::string& strPath)
{
  CURL url(strPath);

  std::string strDirectory = url.GetFileName();
  URIUtils::RemoveSlashAtEnd(strDirectory);

  vector<string> Path = StringUtils::Split(strDirectory, '/');
  Path.insert(Path.begin(), "");

  CDirectoryNode* pNode = NULL;
  CDirectoryNode* pParent = NULL;
  NODE_TYPE NodeType = NODE_TYPE_ROOT;

  for (int i = 0; i < (int)Path.size(); ++i)
  {
    pNode = CDirectoryNode::CreateNode(NodeType, Path[i], pParent);
    NodeType = pNode ? pNode->GetChildType() : NODE_TYPE_NONE;
    pParent = pNode;
  }

  // Add all the additional URL options to the last node
  if (pNode)
    pNode->AddOptions(url.GetOptions());

  return pNode;
}

//  returns the database ids of the path,
void CDirectoryNode::GetDatabaseInfo(const std::string& strPath, CQueryParams& params)
{
  unique_ptr<CDirectoryNode> pNode(CDirectoryNode::ParseURL(strPath));

  if (!pNode.get())
    return;

  pNode->CollectQueryParams(params);
}

//  Create a node object
CDirectoryNode* CDirectoryNode::CreateNode(NODE_TYPE Type,
                                           const std::string& strName,
                                           CDirectoryNode* pParent)
{
  return NULL;
}

//  Current node name
const std::string& CDirectoryNode::GetName() const
{
  return m_strName;
}

//  Current node type
NODE_TYPE CDirectoryNode::GetType() const
{
  return m_Type;
}

//  Return the parent directory node or NULL, if there is no
CDirectoryNode* CDirectoryNode::GetParent() const
{
  return m_pParent;
}

void CDirectoryNode::AddOptions(const std::string& options)
{
  if (options.empty())
    return;

  m_options.AddOptions(options);
}

//  Collects Query params from this and all parent nodes. If a NODE_TYPE can
//  be used as a database parameter, it will be added to the
//  params object.
void CDirectoryNode::CollectQueryParams(CQueryParams& params) const
{
  params.SetQueryParam(m_Type, m_strName);

  CDirectoryNode* pParent = m_pParent;
  while (pParent != NULL)
  {
    params.SetQueryParam(pParent->GetType(), pParent->GetName());
    pParent = pParent->GetParent();
  }
}
