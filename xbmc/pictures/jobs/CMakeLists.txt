set(SOURCES PictureLibraryCleaningJob.cpp
            PictureLibraryJob.cpp
            PictureLibraryProgressJob.cpp
            PictureLibraryScanningJob.cpp)

set(HEADERS PictureLibraryCleaningJob.h
            PictureLibraryJob.h
            PictureLibraryProgressJob.h
            PictureLibraryScanningJob.h)

core_add_library(picture_jobs)
