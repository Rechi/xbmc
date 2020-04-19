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

#include "utils/JobManager.h"

#include <map>
#include <set>

class CGUIDialogProgressBarHandle;
class CPictureLibraryJob;

/*!
 \brief Queue for picture library jobs.

 The queue can only process a single job at any time and every job will be
 executed at the lowest priority.
 */
class CPictureLibraryQueue : protected CJobQueue
{
public:
  ~CPictureLibraryQueue();

  /*!
   \brief Gets the singleton instance of the picture library queue.
  */
  static CPictureLibraryQueue& GetInstance();

  /*!
   \brief Enqueue a library cleaning job.

   \param[in] paths Set with database IDs of paths to be cleaned
   \param[in] asynchronous Run the clean job asynchronously. Defaults to true
   \param[in] progressBar Progress bar to update in GUI. Defaults to NULL (no progress bar to update)
   */
  void CleanLibrary(const std::set<int>& paths = std::set<int>(),
                    bool asynchronous = true,
                    CGUIDialogProgressBarHandle* progressBar = NULL);

private:
  CPictureLibraryQueue();

  typedef std::set<CPictureLibraryJob*> PictureLibraryJobs;
  typedef std::map<std::string, PictureLibraryJobs> PictureLibraryJobMap;
  PictureLibraryJobMap m_jobs;
  CCriticalSection m_critical;

  bool m_cleaning;
};
