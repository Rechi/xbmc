#pragma once
/*
 *      Copyright (C) 2014 Team XBMC
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

#include "PictureLibraryJob.h"
#include "pictures/PictureInfoScanner.h"

#include <string>

/*!
 \brief Picture library job implementation for scanning items.

 Uses CPictureInfoScanner for the whole filesystem scanning and can be run with
 or without a visible progress bar.
 */
class CPictureLibraryScanningJob : public CPictureLibraryJob
{
public:
  /*!
   \brief Creates a new picture library scanning job.

   \param[in] directory Directory to be scanned for new items
   \param[in] scanAll Whether to scan all items or not
   \param[in] showProgress Whether to show a progress bar or not
   */
  CPictureLibraryScanningJob(const std::string& directory,
                             bool scanAll = false,
                             bool showProgress = true);
  virtual ~CPictureLibraryScanningJob();

  // specialization of CPictureLibraryJob
  virtual bool CanBeCancelled() const { return true; }
  virtual bool Cancel();

  // specialization of CJob
  virtual const char* GetType() const { return "PictureLibraryScanningJob"; }
  virtual bool operator==(const CJob* job) const;

protected:
  // implementation of CPictureLibraryJob
  virtual bool Work(CPictureDatabase& db);

private:
  PICTURE::CPictureInfoScanner m_scanner;
  std::string m_directory;
  bool m_showProgress;
  bool m_scanAll;
};
