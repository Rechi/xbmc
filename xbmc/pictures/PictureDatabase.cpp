/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PictureDatabase.h"

#include "ServiceBroker.h"
#include "URL.h"
#include "dbwrappers/dataset.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

//********************************************************************************************************************************
CPictureDatabase::CPictureDatabase(void)
{
}

//********************************************************************************************************************************
CPictureDatabase::~CPictureDatabase(void)
{
}

//********************************************************************************************************************************
bool CPictureDatabase::Open()
{
  return CDatabase::Open(
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_databasePicture);
}

void CPictureDatabase::CreateTables()
{
  CLog::Log(LOGINFO, "create files table");
  m_pDS->exec(
      "CREATE TABLE files (idFile INTEGER PRIMARY KEY AUTOINCREMENT, idPath INTEGER, strFilename TEXT, dateAdded TEXT)");

  CLog::Log(LOGINFO, "create path table");
  m_pDS->exec(
      "CREATE TABLE path (idPath INTEGER PRIMARY KEY AUTOINCREMENT, strPath TEXT, noUpdate BOOL, idParentPath INTEGER)");

  CLog::Log(LOGINFO, "create picture table");
  m_pDS->exec(
      "CREATE TABLE picture (idPicture INTEGER PRIMARY KEY AUTOINCREMENT, idFile INTEGER, exposureDate DATETIME, GPSLatitude DOUBLE, GPSLongitude DOUBLE)");

  CLog::Log(LOGINFO, "create tag table");
  m_pDS->exec("CREATE TABLE tag (idTag INTEGER PRIMARY KEY AUTOINCREMENT, name  VARCHAR(255))");
  m_pDS->exec("CREATE TABLE tag_link (idTag INTEGER, idFile INTEGER)");
}

void CPictureDatabase::CreateAnalytics()
{
  /* indexes should be added on any columns that are used in in  */
  /* a where or a join. primary key on a column is the same as a */
  /* unique index on that column, so there is no need to add any */
  /* index if no other columns are refered                       */

  /* order of indexes are important, for an index to be considered all  */
  /* columns up to the column in question have to have been specified   */
  /* select * from foolink where foo_id = 1, can not take               */
  /* advantage of a index that has been created on ( bar_id, foo_id )   */
  /* however an index on ( foo_id, bar_id ) will be considered for use  */

  CLog::Log(LOGINFO, "%s - creating indicies", __FUNCTION__);
  m_pDS->exec("CREATE INDEX ix_path1 ON path (strPath(255))");
  m_pDS->exec("CREATE INDEX ix_path2 ON path (idParentPath)");
  m_pDS->exec("CREATE INDEX ix_file1 ON files (strFilename(255))");
  m_pDS->exec("CREATE INDEX ix_file2 ON files (idPath)");

  m_pDS->exec("CREATE UNIQUE INDEX ix_picture_file ON picture (idPicture, idFile)");
  m_pDS->exec("CREATE INDEX ix_picture1 on picture (idPicture)");
  m_pDS->exec("CREATE INDEX ix_picture2 on picture (idFile)");
  m_pDS->exec("CREATE INDEX ix_picture3 on picture (exposureDate)");

  m_pDS->exec("CREATE UNIQUE INDEX ix_tag_file ON tag_link (idTag, idFile)");
  m_pDS->exec("CREATE INDEX ix_tag1 on tag (name(255))");
  m_pDS->exec("CREATE INDEX ix_tag2 on tag_link (idTag)");
  m_pDS->exec("CREATE INDEX ix_tag3 on tag_link (idFile)");
}

bool CPictureDatabase::GetPaths(std::set<std::string>& paths)
{
  try
  {
    if (NULL == m_pDB.get())
      return false;
    if (NULL == m_pDS.get())
      return false;

    paths.clear();

    // grab all paths with movie content set
    if (!m_pDS->query("SELECT strPath, noUpdate FROM path"
                      " WHERE strPath NOT LIKE 'multipath://%%'"
                      " ORDER BY strPath"))
      return false;

    while (!m_pDS->eof())
    {
      if (!m_pDS->fv("noUpdate").get_asBool())
        paths.insert(m_pDS->fv("strPath").get_asString());
      m_pDS->next();
    }
    m_pDS->close();
    return true;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
  }
  return false;
}

bool CPictureDatabase::GetSubPaths(const std::string& basepath,
                                   std::vector<std::pair<int, std::string>>& subpaths)
{
  std::string sql;
  try
  {
    if (!m_pDB.get() || !m_pDS.get())
      return false;

    std::string path(basepath);
    URIUtils::AddSlashAtEnd(path);
    sql = PrepareSQL("SELECT idPath, strPath FROM path WHERE SUBSTR(strPath, 1, %i) = '%s'",
                     StringUtils::utf8_strlen(path.c_str()), path.c_str());

    m_pDS->query(sql.c_str());
    while (!m_pDS->eof())
    {
      subpaths.emplace_back(m_pDS->fv(0).get_asInt(), m_pDS->fv(1).get_asString());
      m_pDS->next();
    }
    m_pDS->close();
    return true;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s error during query: %s", __FUNCTION__, sql.c_str());
  }
  return false;
}

int CPictureDatabase::AddTag(const std::string& name)
{
  try
  {
    if (NULL == m_pDB.get())
      return -1;
    if (NULL == m_pDS.get())
      return -1;
    int idTag = -1;

    // ATTENTION: the trimming of actor names should really not be done here but after the scraping / NFO-parsing
    std::string trimmedName = name.c_str();
    StringUtils::Trim(trimmedName);

    std::string strSQL = PrepareSQL("SELECT idTag FROM tag WHERE name LIKE '%s'",
                                    trimmedName.substr(0, 255).c_str());
    m_pDS->query(strSQL.c_str());
    if (m_pDS->num_rows() == 0)
    {
      m_pDS->close();
      // doesnt exists, add it
      strSQL =
          PrepareSQL("INSERT INTO tag (name) VALUES ('%s')", trimmedName.substr(0, 255).c_str());
      m_pDS->exec(strSQL.c_str());
      idTag = (int)m_pDS->lastinsertid();
    }
    else
    {
      idTag = m_pDS->fv(0).get_asInt();
      m_pDS->close();
    }
    return idTag;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, name.c_str());
  }
  return -1;
}

void CPictureDatabase::DeleteTag(int idTag)
{
  try
  {
    if (m_pDB.get() == NULL || m_pDS.get() == NULL)
      return;

    std::string strSQL = PrepareSQL("DELETE FROM tag_link WHERE idTag = %i", idTag);
    m_pDS->exec(strSQL.c_str());

    strSQL = PrepareSQL("DELETE FROM tag WHERE idTag = %i", idTag);
    m_pDS->exec(strSQL.c_str());
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idTag);
  }
}

int CPictureDatabase::GetSchemaVersion() const
{
  return 0;
}
