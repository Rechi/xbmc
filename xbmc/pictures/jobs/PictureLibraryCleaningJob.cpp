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

#include "PictureLibraryCleaningJob.h"

#include "dialogs/GUIDialogExtendedProgressBar.h"
#include "pictures/PictureDatabase.h"

CPictureLibraryCleaningJob::CPictureLibraryCleaningJob(
    const std::set<int>& paths /* = std::set<int>() */, bool showDialog /* = false */)
  : CPictureLibraryProgressJob(NULL), m_paths(paths), m_showDialog(showDialog)
{
}

CPictureLibraryCleaningJob::CPictureLibraryCleaningJob(const std::set<int>& paths,
                                                       CGUIDialogProgressBarHandle* progressBar)
  : CPictureLibraryProgressJob(progressBar), m_paths(paths), m_showDialog(false)
{
}

CPictureLibraryCleaningJob::~CPictureLibraryCleaningJob()
{
}

bool CPictureLibraryCleaningJob::operator==(const CJob* job) const
{
  if (strcmp(job->GetType(), GetType()) != 0)
    return false;

  const CPictureLibraryCleaningJob* cleaningJob =
      dynamic_cast<const CPictureLibraryCleaningJob*>(job);
  if (cleaningJob == NULL)
    return false;

  return m_paths == cleaningJob->m_paths && m_showDialog == cleaningJob->m_showDialog;
}

bool CPictureLibraryCleaningJob::Work(CPictureDatabase& db)
{
  return true;
}
