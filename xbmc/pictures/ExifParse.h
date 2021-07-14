#pragma once

#include "libexif.h"

class CExifParse
{
  public:
   ~CExifParse(void) = default;
  bool Process(const unsigned char* Data, unsigned short length, ExifInfo_t* info);
  static int Get16(const void* Short, bool motorolaOrder = true);
  static int Get32(const void* Long, bool motorolaOrder = true);

  private:
    ExifInfo_t *m_ExifInfo = nullptr;
    double m_FocalPlaneXRes = 0.0;
    double m_FocalPlaneUnits = 0.0;
    unsigned m_LargestExifOffset = 0;          // Last exif data referenced (to check if thumbnail is at end)
    int m_ExifImageWidth = 0;
    bool m_MotorolaOrder = false;
    bool m_DateFound = false;

//    void    LocaliseDate        (void);
//    void    GetExposureTime     (const float exposureTime);
  double ConvertAnyFormat(const void* ValuePtr, int Format);
  void ProcessDir(const unsigned char* DirStart,
                  const unsigned char* OffsetBase,
                  unsigned ExifLength,
                  int NestingLevel);
  void ProcessGpsInfo(const unsigned char* DirStart,
                      int ByteCountUnused,
                      const unsigned char* OffsetBase,
                      unsigned ExifLength);
  void GetLatLong(unsigned int Format,
                  const unsigned char* ValuePtr,
                  int ComponentSize,
                  char* latlongString);
};

