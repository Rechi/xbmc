#pragma once

#include "libexif.h"

class CIptcParse
{
  public:
    static bool Process(const unsigned char* Data, unsigned short length, IPTCInfo_t* info);
};

