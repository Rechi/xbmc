set(SOURCES ExifParse.cpp
            GUIDialogPictureInfo.cpp
            GUIViewStatePictures.cpp
            GUIWindowPictures.cpp
            GUIWindowSlideShow.cpp
            IptcParse.cpp
            JpegParse.cpp
            libexif.cpp
            Picture.cpp
            PictureDatabase.cpp
            PictureInfoLoader.cpp
            PictureInfoScanner.cpp
            PictureInfoTag.cpp
            PictureLibraryQueue.cpp
            PictureScalingAlgorithm.cpp
            PictureThumbLoader.cpp
            SlideShowPicture.cpp)

set(HEADERS GUIDialogPictureInfo.h
            GUIViewStatePictures.h
            GUIWindowPictures.h
            GUIWindowSlideShow.h
            Picture.h
            PictureDatabase.h
            PictureInfoLoader.h
            PictureInfoScanner.h
            PictureInfoTag.h
            PictureLibraryQueue.h
            PictureScalingAlgorithm.h
            PictureThumbLoader.h
            SlideShowPicture.h)

core_add_library(pictures)
