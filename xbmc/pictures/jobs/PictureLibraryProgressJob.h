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
#include "utils/ProgressJob.h"

/*!
 \brief Combined base implementation of a picture library job with a progress bar.
 */
class CPictureLibraryProgressJob : public CProgressJob, public CPictureLibraryJob
{
public:
  virtual ~CPictureLibraryProgressJob();

  // implementation of CJob
  virtual bool DoWork();
  virtual const char* GetType() const { return "CPictureLibraryProgressJob"; }
  virtual bool operator==(const CJob* job) const { return false; }

protected:
  CPictureLibraryProgressJob(CGUIDialogProgressBarHandle* progressBar);
};
