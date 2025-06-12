/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#ifdef __cplusplus

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// # of bytes for initial allocation for printf
#define FORMAT_BLOCK_SIZE 512

// macros for gcc, clang & others
#ifndef PARAM1_PRINTF_FORMAT
#ifdef __GNUC__
// for use in functions that take printf format string as first parameter and additional printf parameters as second parameter
// for example: int myprintf(const char* format, ...) PARAM1_PRINTF_FORMAT;
#define PARAM1_PRINTF_FORMAT __attribute__((format(printf, 1, 2)))

// for use in functions that take printf format string as second parameter and additional printf parameters as third parameter
// for example: bool log_string(int logLevel, const char* format, ...) PARAM2_PRINTF_FORMAT;
// note: all non-static class member functions take pointer to class object as hidden first parameter
#define PARAM2_PRINTF_FORMAT __attribute__((format(printf, 2, 3)))

// for use in functions that take printf format string as third parameter and additional printf parameters as fourth parameter
// note: all non-static class member functions take pointer to class object as hidden first parameter
// for example: class A { bool log_string(int logLevel, const char* functionName, const char* format, ...) PARAM3_PRINTF_FORMAT; };
#define PARAM3_PRINTF_FORMAT __attribute__((format(printf, 3, 4)))

// for use in functions that take printf format string as fourth parameter and additional printf parameters as fifth parameter
// note: all non-static class member functions take pointer to class object as hidden first parameter
// for example: class A { bool log_string(int logLevel, const char* functionName, int component, const char* format, ...) PARAM4_PRINTF_FORMAT; };
#define PARAM4_PRINTF_FORMAT __attribute__((format(printf, 4, 5)))
#else // ! __GNUC__
#define PARAM1_PRINTF_FORMAT
#define PARAM2_PRINTF_FORMAT
#define PARAM3_PRINTF_FORMAT
#define PARAM4_PRINTF_FORMAT
#endif // ! __GNUC__
#endif // PARAM1_PRINTF_FORMAT

// macros for VC
// VC check parameters only when "Code Analysis" is called
#ifndef PRINTF_FORMAT_STRING
#ifdef _MSC_VER
#include <sal.h>

// for use in any function that take printf format string and parameters
// for example: bool log_string(int logLevel, PRINTF_FORMAT_STRING const char* format, ...);
#define PRINTF_FORMAT_STRING _In_z_ _Printf_format_string_

// specify that parameter must be zero-terminated string
// for example: void SetName(IN_STRING const char* newName);
#define IN_STRING _In_z_

// specify that parameter must be zero-terminated string or NULL
// for example: bool SetAdditionalName(IN_OPT_STRING const char* addName);
#define IN_OPT_STRING _In_opt_z_
#else // ! _MSC_VER
#define PRINTF_FORMAT_STRING
#define IN_STRING
#define IN_OPT_STRING
#endif // ! _MSC_VER
#endif // PRINTF_FORMAT_STRING

static constexpr wchar_t unicode_lowers[] = {
    static_cast<wchar_t>(0x0061), static_cast<wchar_t>(0x0062), static_cast<wchar_t>(0x0063),
    static_cast<wchar_t>(0x0064), static_cast<wchar_t>(0x0065), static_cast<wchar_t>(0x0066),
    static_cast<wchar_t>(0x0067), static_cast<wchar_t>(0x0068), static_cast<wchar_t>(0x0069),
    static_cast<wchar_t>(0x006A), static_cast<wchar_t>(0x006B), static_cast<wchar_t>(0x006C),
    static_cast<wchar_t>(0x006D), static_cast<wchar_t>(0x006E), static_cast<wchar_t>(0x006F),
    static_cast<wchar_t>(0x0070), static_cast<wchar_t>(0x0071), static_cast<wchar_t>(0x0072),
    static_cast<wchar_t>(0x0073), static_cast<wchar_t>(0x0074), static_cast<wchar_t>(0x0075),
    static_cast<wchar_t>(0x0076), static_cast<wchar_t>(0x0077), static_cast<wchar_t>(0x0078),
    static_cast<wchar_t>(0x0079), static_cast<wchar_t>(0x007A), static_cast<wchar_t>(0x00E0),
    static_cast<wchar_t>(0x00E1), static_cast<wchar_t>(0x00E2), static_cast<wchar_t>(0x00E3),
    static_cast<wchar_t>(0x00E4), static_cast<wchar_t>(0x00E5), static_cast<wchar_t>(0x00E6),
    static_cast<wchar_t>(0x00E7), static_cast<wchar_t>(0x00E8), static_cast<wchar_t>(0x00E9),
    static_cast<wchar_t>(0x00EA), static_cast<wchar_t>(0x00EB), static_cast<wchar_t>(0x00EC),
    static_cast<wchar_t>(0x00ED), static_cast<wchar_t>(0x00EE), static_cast<wchar_t>(0x00EF),
    static_cast<wchar_t>(0x00F0), static_cast<wchar_t>(0x00F1), static_cast<wchar_t>(0x00F2),
    static_cast<wchar_t>(0x00F3), static_cast<wchar_t>(0x00F4), static_cast<wchar_t>(0x00F5),
    static_cast<wchar_t>(0x00F6), static_cast<wchar_t>(0x00F8), static_cast<wchar_t>(0x00F9),
    static_cast<wchar_t>(0x00FA), static_cast<wchar_t>(0x00FB), static_cast<wchar_t>(0x00FC),
    static_cast<wchar_t>(0x00FD), static_cast<wchar_t>(0x00FE), static_cast<wchar_t>(0x00FF),
    static_cast<wchar_t>(0x0101), static_cast<wchar_t>(0x0103), static_cast<wchar_t>(0x0105),
    static_cast<wchar_t>(0x0107), static_cast<wchar_t>(0x0109), static_cast<wchar_t>(0x010B),
    static_cast<wchar_t>(0x010D), static_cast<wchar_t>(0x010F), static_cast<wchar_t>(0x0111),
    static_cast<wchar_t>(0x0113), static_cast<wchar_t>(0x0115), static_cast<wchar_t>(0x0117),
    static_cast<wchar_t>(0x0119), static_cast<wchar_t>(0x011B), static_cast<wchar_t>(0x011D),
    static_cast<wchar_t>(0x011F), static_cast<wchar_t>(0x0121), static_cast<wchar_t>(0x0123),
    static_cast<wchar_t>(0x0125), static_cast<wchar_t>(0x0127), static_cast<wchar_t>(0x0129),
    static_cast<wchar_t>(0x012B), static_cast<wchar_t>(0x012D), static_cast<wchar_t>(0x012F),
    static_cast<wchar_t>(0x0131), static_cast<wchar_t>(0x0133), static_cast<wchar_t>(0x0135),
    static_cast<wchar_t>(0x0137), static_cast<wchar_t>(0x013A), static_cast<wchar_t>(0x013C),
    static_cast<wchar_t>(0x013E), static_cast<wchar_t>(0x0140), static_cast<wchar_t>(0x0142),
    static_cast<wchar_t>(0x0144), static_cast<wchar_t>(0x0146), static_cast<wchar_t>(0x0148),
    static_cast<wchar_t>(0x014B), static_cast<wchar_t>(0x014D), static_cast<wchar_t>(0x014F),
    static_cast<wchar_t>(0x0151), static_cast<wchar_t>(0x0153), static_cast<wchar_t>(0x0155),
    static_cast<wchar_t>(0x0157), static_cast<wchar_t>(0x0159), static_cast<wchar_t>(0x015B),
    static_cast<wchar_t>(0x015D), static_cast<wchar_t>(0x015F), static_cast<wchar_t>(0x0161),
    static_cast<wchar_t>(0x0163), static_cast<wchar_t>(0x0165), static_cast<wchar_t>(0x0167),
    static_cast<wchar_t>(0x0169), static_cast<wchar_t>(0x016B), static_cast<wchar_t>(0x016D),
    static_cast<wchar_t>(0x016F), static_cast<wchar_t>(0x0171), static_cast<wchar_t>(0x0173),
    static_cast<wchar_t>(0x0175), static_cast<wchar_t>(0x0177), static_cast<wchar_t>(0x017A),
    static_cast<wchar_t>(0x017C), static_cast<wchar_t>(0x017E), static_cast<wchar_t>(0x0183),
    static_cast<wchar_t>(0x0185), static_cast<wchar_t>(0x0188), static_cast<wchar_t>(0x018C),
    static_cast<wchar_t>(0x0192), static_cast<wchar_t>(0x0199), static_cast<wchar_t>(0x01A1),
    static_cast<wchar_t>(0x01A3), static_cast<wchar_t>(0x01A5), static_cast<wchar_t>(0x01A8),
    static_cast<wchar_t>(0x01AD), static_cast<wchar_t>(0x01B0), static_cast<wchar_t>(0x01B4),
    static_cast<wchar_t>(0x01B6), static_cast<wchar_t>(0x01B9), static_cast<wchar_t>(0x01BD),
    static_cast<wchar_t>(0x01C6), static_cast<wchar_t>(0x01C9), static_cast<wchar_t>(0x01CC),
    static_cast<wchar_t>(0x01CE), static_cast<wchar_t>(0x01D0), static_cast<wchar_t>(0x01D2),
    static_cast<wchar_t>(0x01D4), static_cast<wchar_t>(0x01D6), static_cast<wchar_t>(0x01D8),
    static_cast<wchar_t>(0x01DA), static_cast<wchar_t>(0x01DC), static_cast<wchar_t>(0x01DF),
    static_cast<wchar_t>(0x01E1), static_cast<wchar_t>(0x01E3), static_cast<wchar_t>(0x01E5),
    static_cast<wchar_t>(0x01E7), static_cast<wchar_t>(0x01E9), static_cast<wchar_t>(0x01EB),
    static_cast<wchar_t>(0x01ED), static_cast<wchar_t>(0x01EF), static_cast<wchar_t>(0x01F3),
    static_cast<wchar_t>(0x01F5), static_cast<wchar_t>(0x01FB), static_cast<wchar_t>(0x01FD),
    static_cast<wchar_t>(0x01FF), static_cast<wchar_t>(0x0201), static_cast<wchar_t>(0x0203),
    static_cast<wchar_t>(0x0205), static_cast<wchar_t>(0x0207), static_cast<wchar_t>(0x0209),
    static_cast<wchar_t>(0x020B), static_cast<wchar_t>(0x020D), static_cast<wchar_t>(0x020F),
    static_cast<wchar_t>(0x0211), static_cast<wchar_t>(0x0213), static_cast<wchar_t>(0x0215),
    static_cast<wchar_t>(0x0217), static_cast<wchar_t>(0x0253), static_cast<wchar_t>(0x0254),
    static_cast<wchar_t>(0x0257), static_cast<wchar_t>(0x0258), static_cast<wchar_t>(0x0259),
    static_cast<wchar_t>(0x025B), static_cast<wchar_t>(0x0260), static_cast<wchar_t>(0x0263),
    static_cast<wchar_t>(0x0268), static_cast<wchar_t>(0x0269), static_cast<wchar_t>(0x026F),
    static_cast<wchar_t>(0x0272), static_cast<wchar_t>(0x0275), static_cast<wchar_t>(0x0283),
    static_cast<wchar_t>(0x0288), static_cast<wchar_t>(0x028A), static_cast<wchar_t>(0x028B),
    static_cast<wchar_t>(0x0292), static_cast<wchar_t>(0x03AC), static_cast<wchar_t>(0x03AD),
    static_cast<wchar_t>(0x03AE), static_cast<wchar_t>(0x03AF), static_cast<wchar_t>(0x03B1),
    static_cast<wchar_t>(0x03B2), static_cast<wchar_t>(0x03B3), static_cast<wchar_t>(0x03B4),
    static_cast<wchar_t>(0x03B5), static_cast<wchar_t>(0x03B6), static_cast<wchar_t>(0x03B7),
    static_cast<wchar_t>(0x03B8), static_cast<wchar_t>(0x03B9), static_cast<wchar_t>(0x03BA),
    static_cast<wchar_t>(0x03BB), static_cast<wchar_t>(0x03BC), static_cast<wchar_t>(0x03BD),
    static_cast<wchar_t>(0x03BE), static_cast<wchar_t>(0x03BF), static_cast<wchar_t>(0x03C0),
    static_cast<wchar_t>(0x03C1), static_cast<wchar_t>(0x03C3), static_cast<wchar_t>(0x03C4),
    static_cast<wchar_t>(0x03C5), static_cast<wchar_t>(0x03C6), static_cast<wchar_t>(0x03C7),
    static_cast<wchar_t>(0x03C8), static_cast<wchar_t>(0x03C9), static_cast<wchar_t>(0x03CA),
    static_cast<wchar_t>(0x03CB), static_cast<wchar_t>(0x03CC), static_cast<wchar_t>(0x03CD),
    static_cast<wchar_t>(0x03CE), static_cast<wchar_t>(0x03E3), static_cast<wchar_t>(0x03E5),
    static_cast<wchar_t>(0x03E7), static_cast<wchar_t>(0x03E9), static_cast<wchar_t>(0x03EB),
    static_cast<wchar_t>(0x03ED), static_cast<wchar_t>(0x03EF), static_cast<wchar_t>(0x0430),
    static_cast<wchar_t>(0x0431), static_cast<wchar_t>(0x0432), static_cast<wchar_t>(0x0433),
    static_cast<wchar_t>(0x0434), static_cast<wchar_t>(0x0435), static_cast<wchar_t>(0x0436),
    static_cast<wchar_t>(0x0437), static_cast<wchar_t>(0x0438), static_cast<wchar_t>(0x0439),
    static_cast<wchar_t>(0x043A), static_cast<wchar_t>(0x043B), static_cast<wchar_t>(0x043C),
    static_cast<wchar_t>(0x043D), static_cast<wchar_t>(0x043E), static_cast<wchar_t>(0x043F),
    static_cast<wchar_t>(0x0440), static_cast<wchar_t>(0x0441), static_cast<wchar_t>(0x0442),
    static_cast<wchar_t>(0x0443), static_cast<wchar_t>(0x0444), static_cast<wchar_t>(0x0445),
    static_cast<wchar_t>(0x0446), static_cast<wchar_t>(0x0447), static_cast<wchar_t>(0x0448),
    static_cast<wchar_t>(0x0449), static_cast<wchar_t>(0x044A), static_cast<wchar_t>(0x044B),
    static_cast<wchar_t>(0x044C), static_cast<wchar_t>(0x044D), static_cast<wchar_t>(0x044E),
    static_cast<wchar_t>(0x044F), static_cast<wchar_t>(0x0451), static_cast<wchar_t>(0x0452),
    static_cast<wchar_t>(0x0453), static_cast<wchar_t>(0x0454), static_cast<wchar_t>(0x0455),
    static_cast<wchar_t>(0x0456), static_cast<wchar_t>(0x0457), static_cast<wchar_t>(0x0458),
    static_cast<wchar_t>(0x0459), static_cast<wchar_t>(0x045A), static_cast<wchar_t>(0x045B),
    static_cast<wchar_t>(0x045C), static_cast<wchar_t>(0x045E), static_cast<wchar_t>(0x045F),
    static_cast<wchar_t>(0x0461), static_cast<wchar_t>(0x0463), static_cast<wchar_t>(0x0465),
    static_cast<wchar_t>(0x0467), static_cast<wchar_t>(0x0469), static_cast<wchar_t>(0x046B),
    static_cast<wchar_t>(0x046D), static_cast<wchar_t>(0x046F), static_cast<wchar_t>(0x0471),
    static_cast<wchar_t>(0x0473), static_cast<wchar_t>(0x0475), static_cast<wchar_t>(0x0477),
    static_cast<wchar_t>(0x0479), static_cast<wchar_t>(0x047B), static_cast<wchar_t>(0x047D),
    static_cast<wchar_t>(0x047F), static_cast<wchar_t>(0x0481), static_cast<wchar_t>(0x0491),
    static_cast<wchar_t>(0x0493), static_cast<wchar_t>(0x0495), static_cast<wchar_t>(0x0497),
    static_cast<wchar_t>(0x0499), static_cast<wchar_t>(0x049B), static_cast<wchar_t>(0x049D),
    static_cast<wchar_t>(0x049F), static_cast<wchar_t>(0x04A1), static_cast<wchar_t>(0x04A3),
    static_cast<wchar_t>(0x04A5), static_cast<wchar_t>(0x04A7), static_cast<wchar_t>(0x04A9),
    static_cast<wchar_t>(0x04AB), static_cast<wchar_t>(0x04AD), static_cast<wchar_t>(0x04AF),
    static_cast<wchar_t>(0x04B1), static_cast<wchar_t>(0x04B3), static_cast<wchar_t>(0x04B5),
    static_cast<wchar_t>(0x04B7), static_cast<wchar_t>(0x04B9), static_cast<wchar_t>(0x04BB),
    static_cast<wchar_t>(0x04BD), static_cast<wchar_t>(0x04BF), static_cast<wchar_t>(0x04C2),
    static_cast<wchar_t>(0x04C4), static_cast<wchar_t>(0x04C8), static_cast<wchar_t>(0x04CC),
    static_cast<wchar_t>(0x04D1), static_cast<wchar_t>(0x04D3), static_cast<wchar_t>(0x04D5),
    static_cast<wchar_t>(0x04D7), static_cast<wchar_t>(0x04D9), static_cast<wchar_t>(0x04DB),
    static_cast<wchar_t>(0x04DD), static_cast<wchar_t>(0x04DF), static_cast<wchar_t>(0x04E1),
    static_cast<wchar_t>(0x04E3), static_cast<wchar_t>(0x04E5), static_cast<wchar_t>(0x04E7),
    static_cast<wchar_t>(0x04E9), static_cast<wchar_t>(0x04EB), static_cast<wchar_t>(0x04EF),
    static_cast<wchar_t>(0x04F1), static_cast<wchar_t>(0x04F3), static_cast<wchar_t>(0x04F5),
    static_cast<wchar_t>(0x04F9), static_cast<wchar_t>(0x0561), static_cast<wchar_t>(0x0562),
    static_cast<wchar_t>(0x0563), static_cast<wchar_t>(0x0564), static_cast<wchar_t>(0x0565),
    static_cast<wchar_t>(0x0566), static_cast<wchar_t>(0x0567), static_cast<wchar_t>(0x0568),
    static_cast<wchar_t>(0x0569), static_cast<wchar_t>(0x056A), static_cast<wchar_t>(0x056B),
    static_cast<wchar_t>(0x056C), static_cast<wchar_t>(0x056D), static_cast<wchar_t>(0x056E),
    static_cast<wchar_t>(0x056F), static_cast<wchar_t>(0x0570), static_cast<wchar_t>(0x0571),
    static_cast<wchar_t>(0x0572), static_cast<wchar_t>(0x0573), static_cast<wchar_t>(0x0574),
    static_cast<wchar_t>(0x0575), static_cast<wchar_t>(0x0576), static_cast<wchar_t>(0x0577),
    static_cast<wchar_t>(0x0578), static_cast<wchar_t>(0x0579), static_cast<wchar_t>(0x057A),
    static_cast<wchar_t>(0x057B), static_cast<wchar_t>(0x057C), static_cast<wchar_t>(0x057D),
    static_cast<wchar_t>(0x057E), static_cast<wchar_t>(0x057F), static_cast<wchar_t>(0x0580),
    static_cast<wchar_t>(0x0581), static_cast<wchar_t>(0x0582), static_cast<wchar_t>(0x0583),
    static_cast<wchar_t>(0x0584), static_cast<wchar_t>(0x0585), static_cast<wchar_t>(0x0586),
    static_cast<wchar_t>(0x10D0), static_cast<wchar_t>(0x10D1), static_cast<wchar_t>(0x10D2),
    static_cast<wchar_t>(0x10D3), static_cast<wchar_t>(0x10D4), static_cast<wchar_t>(0x10D5),
    static_cast<wchar_t>(0x10D6), static_cast<wchar_t>(0x10D7), static_cast<wchar_t>(0x10D8),
    static_cast<wchar_t>(0x10D9), static_cast<wchar_t>(0x10DA), static_cast<wchar_t>(0x10DB),
    static_cast<wchar_t>(0x10DC), static_cast<wchar_t>(0x10DD), static_cast<wchar_t>(0x10DE),
    static_cast<wchar_t>(0x10DF), static_cast<wchar_t>(0x10E0), static_cast<wchar_t>(0x10E1),
    static_cast<wchar_t>(0x10E2), static_cast<wchar_t>(0x10E3), static_cast<wchar_t>(0x10E4),
    static_cast<wchar_t>(0x10E5), static_cast<wchar_t>(0x10E6), static_cast<wchar_t>(0x10E7),
    static_cast<wchar_t>(0x10E8), static_cast<wchar_t>(0x10E9), static_cast<wchar_t>(0x10EA),
    static_cast<wchar_t>(0x10EB), static_cast<wchar_t>(0x10EC), static_cast<wchar_t>(0x10ED),
    static_cast<wchar_t>(0x10EE), static_cast<wchar_t>(0x10EF), static_cast<wchar_t>(0x10F0),
    static_cast<wchar_t>(0x10F1), static_cast<wchar_t>(0x10F2), static_cast<wchar_t>(0x10F3),
    static_cast<wchar_t>(0x10F4), static_cast<wchar_t>(0x10F5), static_cast<wchar_t>(0x1E01),
    static_cast<wchar_t>(0x1E03), static_cast<wchar_t>(0x1E05), static_cast<wchar_t>(0x1E07),
    static_cast<wchar_t>(0x1E09), static_cast<wchar_t>(0x1E0B), static_cast<wchar_t>(0x1E0D),
    static_cast<wchar_t>(0x1E0F), static_cast<wchar_t>(0x1E11), static_cast<wchar_t>(0x1E13),
    static_cast<wchar_t>(0x1E15), static_cast<wchar_t>(0x1E17), static_cast<wchar_t>(0x1E19),
    static_cast<wchar_t>(0x1E1B), static_cast<wchar_t>(0x1E1D), static_cast<wchar_t>(0x1E1F),
    static_cast<wchar_t>(0x1E21), static_cast<wchar_t>(0x1E23), static_cast<wchar_t>(0x1E25),
    static_cast<wchar_t>(0x1E27), static_cast<wchar_t>(0x1E29), static_cast<wchar_t>(0x1E2B),
    static_cast<wchar_t>(0x1E2D), static_cast<wchar_t>(0x1E2F), static_cast<wchar_t>(0x1E31),
    static_cast<wchar_t>(0x1E33), static_cast<wchar_t>(0x1E35), static_cast<wchar_t>(0x1E37),
    static_cast<wchar_t>(0x1E39), static_cast<wchar_t>(0x1E3B), static_cast<wchar_t>(0x1E3D),
    static_cast<wchar_t>(0x1E3F), static_cast<wchar_t>(0x1E41), static_cast<wchar_t>(0x1E43),
    static_cast<wchar_t>(0x1E45), static_cast<wchar_t>(0x1E47), static_cast<wchar_t>(0x1E49),
    static_cast<wchar_t>(0x1E4B), static_cast<wchar_t>(0x1E4D), static_cast<wchar_t>(0x1E4F),
    static_cast<wchar_t>(0x1E51), static_cast<wchar_t>(0x1E53), static_cast<wchar_t>(0x1E55),
    static_cast<wchar_t>(0x1E57), static_cast<wchar_t>(0x1E59), static_cast<wchar_t>(0x1E5B),
    static_cast<wchar_t>(0x1E5D), static_cast<wchar_t>(0x1E5F), static_cast<wchar_t>(0x1E61),
    static_cast<wchar_t>(0x1E63), static_cast<wchar_t>(0x1E65), static_cast<wchar_t>(0x1E67),
    static_cast<wchar_t>(0x1E69), static_cast<wchar_t>(0x1E6B), static_cast<wchar_t>(0x1E6D),
    static_cast<wchar_t>(0x1E6F), static_cast<wchar_t>(0x1E71), static_cast<wchar_t>(0x1E73),
    static_cast<wchar_t>(0x1E75), static_cast<wchar_t>(0x1E77), static_cast<wchar_t>(0x1E79),
    static_cast<wchar_t>(0x1E7B), static_cast<wchar_t>(0x1E7D), static_cast<wchar_t>(0x1E7F),
    static_cast<wchar_t>(0x1E81), static_cast<wchar_t>(0x1E83), static_cast<wchar_t>(0x1E85),
    static_cast<wchar_t>(0x1E87), static_cast<wchar_t>(0x1E89), static_cast<wchar_t>(0x1E8B),
    static_cast<wchar_t>(0x1E8D), static_cast<wchar_t>(0x1E8F), static_cast<wchar_t>(0x1E91),
    static_cast<wchar_t>(0x1E93), static_cast<wchar_t>(0x1E95), static_cast<wchar_t>(0x1EA1),
    static_cast<wchar_t>(0x1EA3), static_cast<wchar_t>(0x1EA5), static_cast<wchar_t>(0x1EA7),
    static_cast<wchar_t>(0x1EA9), static_cast<wchar_t>(0x1EAB), static_cast<wchar_t>(0x1EAD),
    static_cast<wchar_t>(0x1EAF), static_cast<wchar_t>(0x1EB1), static_cast<wchar_t>(0x1EB3),
    static_cast<wchar_t>(0x1EB5), static_cast<wchar_t>(0x1EB7), static_cast<wchar_t>(0x1EB9),
    static_cast<wchar_t>(0x1EBB), static_cast<wchar_t>(0x1EBD), static_cast<wchar_t>(0x1EBF),
    static_cast<wchar_t>(0x1EC1), static_cast<wchar_t>(0x1EC3), static_cast<wchar_t>(0x1EC5),
    static_cast<wchar_t>(0x1EC7), static_cast<wchar_t>(0x1EC9), static_cast<wchar_t>(0x1ECB),
    static_cast<wchar_t>(0x1ECD), static_cast<wchar_t>(0x1ECF), static_cast<wchar_t>(0x1ED1),
    static_cast<wchar_t>(0x1ED3), static_cast<wchar_t>(0x1ED5), static_cast<wchar_t>(0x1ED7),
    static_cast<wchar_t>(0x1ED9), static_cast<wchar_t>(0x1EDB), static_cast<wchar_t>(0x1EDD),
    static_cast<wchar_t>(0x1EDF), static_cast<wchar_t>(0x1EE1), static_cast<wchar_t>(0x1EE3),
    static_cast<wchar_t>(0x1EE5), static_cast<wchar_t>(0x1EE7), static_cast<wchar_t>(0x1EE9),
    static_cast<wchar_t>(0x1EEB), static_cast<wchar_t>(0x1EED), static_cast<wchar_t>(0x1EEF),
    static_cast<wchar_t>(0x1EF1), static_cast<wchar_t>(0x1EF3), static_cast<wchar_t>(0x1EF5),
    static_cast<wchar_t>(0x1EF7), static_cast<wchar_t>(0x1EF9), static_cast<wchar_t>(0x1F00),
    static_cast<wchar_t>(0x1F01), static_cast<wchar_t>(0x1F02), static_cast<wchar_t>(0x1F03),
    static_cast<wchar_t>(0x1F04), static_cast<wchar_t>(0x1F05), static_cast<wchar_t>(0x1F06),
    static_cast<wchar_t>(0x1F07), static_cast<wchar_t>(0x1F10), static_cast<wchar_t>(0x1F11),
    static_cast<wchar_t>(0x1F12), static_cast<wchar_t>(0x1F13), static_cast<wchar_t>(0x1F14),
    static_cast<wchar_t>(0x1F15), static_cast<wchar_t>(0x1F20), static_cast<wchar_t>(0x1F21),
    static_cast<wchar_t>(0x1F22), static_cast<wchar_t>(0x1F23), static_cast<wchar_t>(0x1F24),
    static_cast<wchar_t>(0x1F25), static_cast<wchar_t>(0x1F26), static_cast<wchar_t>(0x1F27),
    static_cast<wchar_t>(0x1F30), static_cast<wchar_t>(0x1F31), static_cast<wchar_t>(0x1F32),
    static_cast<wchar_t>(0x1F33), static_cast<wchar_t>(0x1F34), static_cast<wchar_t>(0x1F35),
    static_cast<wchar_t>(0x1F36), static_cast<wchar_t>(0x1F37), static_cast<wchar_t>(0x1F40),
    static_cast<wchar_t>(0x1F41), static_cast<wchar_t>(0x1F42), static_cast<wchar_t>(0x1F43),
    static_cast<wchar_t>(0x1F44), static_cast<wchar_t>(0x1F45), static_cast<wchar_t>(0x1F51),
    static_cast<wchar_t>(0x1F53), static_cast<wchar_t>(0x1F55), static_cast<wchar_t>(0x1F57),
    static_cast<wchar_t>(0x1F60), static_cast<wchar_t>(0x1F61), static_cast<wchar_t>(0x1F62),
    static_cast<wchar_t>(0x1F63), static_cast<wchar_t>(0x1F64), static_cast<wchar_t>(0x1F65),
    static_cast<wchar_t>(0x1F66), static_cast<wchar_t>(0x1F67), static_cast<wchar_t>(0x1F80),
    static_cast<wchar_t>(0x1F81), static_cast<wchar_t>(0x1F82), static_cast<wchar_t>(0x1F83),
    static_cast<wchar_t>(0x1F84), static_cast<wchar_t>(0x1F85), static_cast<wchar_t>(0x1F86),
    static_cast<wchar_t>(0x1F87), static_cast<wchar_t>(0x1F90), static_cast<wchar_t>(0x1F91),
    static_cast<wchar_t>(0x1F92), static_cast<wchar_t>(0x1F93), static_cast<wchar_t>(0x1F94),
    static_cast<wchar_t>(0x1F95), static_cast<wchar_t>(0x1F96), static_cast<wchar_t>(0x1F97),
    static_cast<wchar_t>(0x1FA0), static_cast<wchar_t>(0x1FA1), static_cast<wchar_t>(0x1FA2),
    static_cast<wchar_t>(0x1FA3), static_cast<wchar_t>(0x1FA4), static_cast<wchar_t>(0x1FA5),
    static_cast<wchar_t>(0x1FA6), static_cast<wchar_t>(0x1FA7), static_cast<wchar_t>(0x1FB0),
    static_cast<wchar_t>(0x1FB1), static_cast<wchar_t>(0x1FD0), static_cast<wchar_t>(0x1FD1),
    static_cast<wchar_t>(0x1FE0), static_cast<wchar_t>(0x1FE1), static_cast<wchar_t>(0x24D0),
    static_cast<wchar_t>(0x24D1), static_cast<wchar_t>(0x24D2), static_cast<wchar_t>(0x24D3),
    static_cast<wchar_t>(0x24D4), static_cast<wchar_t>(0x24D5), static_cast<wchar_t>(0x24D6),
    static_cast<wchar_t>(0x24D7), static_cast<wchar_t>(0x24D8), static_cast<wchar_t>(0x24D9),
    static_cast<wchar_t>(0x24DA), static_cast<wchar_t>(0x24DB), static_cast<wchar_t>(0x24DC),
    static_cast<wchar_t>(0x24DD), static_cast<wchar_t>(0x24DE), static_cast<wchar_t>(0x24DF),
    static_cast<wchar_t>(0x24E0), static_cast<wchar_t>(0x24E1), static_cast<wchar_t>(0x24E2),
    static_cast<wchar_t>(0x24E3), static_cast<wchar_t>(0x24E4), static_cast<wchar_t>(0x24E5),
    static_cast<wchar_t>(0x24E6), static_cast<wchar_t>(0x24E7), static_cast<wchar_t>(0x24E8),
    static_cast<wchar_t>(0x24E9), static_cast<wchar_t>(0xFF41), static_cast<wchar_t>(0xFF42),
    static_cast<wchar_t>(0xFF43), static_cast<wchar_t>(0xFF44), static_cast<wchar_t>(0xFF45),
    static_cast<wchar_t>(0xFF46), static_cast<wchar_t>(0xFF47), static_cast<wchar_t>(0xFF48),
    static_cast<wchar_t>(0xFF49), static_cast<wchar_t>(0xFF4A), static_cast<wchar_t>(0xFF4B),
    static_cast<wchar_t>(0xFF4C), static_cast<wchar_t>(0xFF4D), static_cast<wchar_t>(0xFF4E),
    static_cast<wchar_t>(0xFF4F), static_cast<wchar_t>(0xFF50), static_cast<wchar_t>(0xFF51),
    static_cast<wchar_t>(0xFF52), static_cast<wchar_t>(0xFF53), static_cast<wchar_t>(0xFF54),
    static_cast<wchar_t>(0xFF55), static_cast<wchar_t>(0xFF56), static_cast<wchar_t>(0xFF57),
    static_cast<wchar_t>(0xFF58), static_cast<wchar_t>(0xFF59), static_cast<wchar_t>(0xFF5A)};

static const wchar_t unicode_uppers[] = {
    static_cast<wchar_t>(0x0041), static_cast<wchar_t>(0x0042), static_cast<wchar_t>(0x0043),
    static_cast<wchar_t>(0x0044), static_cast<wchar_t>(0x0045), static_cast<wchar_t>(0x0046),
    static_cast<wchar_t>(0x0047), static_cast<wchar_t>(0x0048), static_cast<wchar_t>(0x0049),
    static_cast<wchar_t>(0x004A), static_cast<wchar_t>(0x004B), static_cast<wchar_t>(0x004C),
    static_cast<wchar_t>(0x004D), static_cast<wchar_t>(0x004E), static_cast<wchar_t>(0x004F),
    static_cast<wchar_t>(0x0050), static_cast<wchar_t>(0x0051), static_cast<wchar_t>(0x0052),
    static_cast<wchar_t>(0x0053), static_cast<wchar_t>(0x0054), static_cast<wchar_t>(0x0055),
    static_cast<wchar_t>(0x0056), static_cast<wchar_t>(0x0057), static_cast<wchar_t>(0x0058),
    static_cast<wchar_t>(0x0059), static_cast<wchar_t>(0x005A), static_cast<wchar_t>(0x00C0),
    static_cast<wchar_t>(0x00C1), static_cast<wchar_t>(0x00C2), static_cast<wchar_t>(0x00C3),
    static_cast<wchar_t>(0x00C4), static_cast<wchar_t>(0x00C5), static_cast<wchar_t>(0x00C6),
    static_cast<wchar_t>(0x00C7), static_cast<wchar_t>(0x00C8), static_cast<wchar_t>(0x00C9),
    static_cast<wchar_t>(0x00CA), static_cast<wchar_t>(0x00CB), static_cast<wchar_t>(0x00CC),
    static_cast<wchar_t>(0x00CD), static_cast<wchar_t>(0x00CE), static_cast<wchar_t>(0x00CF),
    static_cast<wchar_t>(0x00D0), static_cast<wchar_t>(0x00D1), static_cast<wchar_t>(0x00D2),
    static_cast<wchar_t>(0x00D3), static_cast<wchar_t>(0x00D4), static_cast<wchar_t>(0x00D5),
    static_cast<wchar_t>(0x00D6), static_cast<wchar_t>(0x00D8), static_cast<wchar_t>(0x00D9),
    static_cast<wchar_t>(0x00DA), static_cast<wchar_t>(0x00DB), static_cast<wchar_t>(0x00DC),
    static_cast<wchar_t>(0x00DD), static_cast<wchar_t>(0x00DE), static_cast<wchar_t>(0x0178),
    static_cast<wchar_t>(0x0100), static_cast<wchar_t>(0x0102), static_cast<wchar_t>(0x0104),
    static_cast<wchar_t>(0x0106), static_cast<wchar_t>(0x0108), static_cast<wchar_t>(0x010A),
    static_cast<wchar_t>(0x010C), static_cast<wchar_t>(0x010E), static_cast<wchar_t>(0x0110),
    static_cast<wchar_t>(0x0112), static_cast<wchar_t>(0x0114), static_cast<wchar_t>(0x0116),
    static_cast<wchar_t>(0x0118), static_cast<wchar_t>(0x011A), static_cast<wchar_t>(0x011C),
    static_cast<wchar_t>(0x011E), static_cast<wchar_t>(0x0120), static_cast<wchar_t>(0x0122),
    static_cast<wchar_t>(0x0124), static_cast<wchar_t>(0x0126), static_cast<wchar_t>(0x0128),
    static_cast<wchar_t>(0x012A), static_cast<wchar_t>(0x012C), static_cast<wchar_t>(0x012E),
    static_cast<wchar_t>(0x0049), static_cast<wchar_t>(0x0132), static_cast<wchar_t>(0x0134),
    static_cast<wchar_t>(0x0136), static_cast<wchar_t>(0x0139), static_cast<wchar_t>(0x013B),
    static_cast<wchar_t>(0x013D), static_cast<wchar_t>(0x013F), static_cast<wchar_t>(0x0141),
    static_cast<wchar_t>(0x0143), static_cast<wchar_t>(0x0145), static_cast<wchar_t>(0x0147),
    static_cast<wchar_t>(0x014A), static_cast<wchar_t>(0x014C), static_cast<wchar_t>(0x014E),
    static_cast<wchar_t>(0x0150), static_cast<wchar_t>(0x0152), static_cast<wchar_t>(0x0154),
    static_cast<wchar_t>(0x0156), static_cast<wchar_t>(0x0158), static_cast<wchar_t>(0x015A),
    static_cast<wchar_t>(0x015C), static_cast<wchar_t>(0x015E), static_cast<wchar_t>(0x0160),
    static_cast<wchar_t>(0x0162), static_cast<wchar_t>(0x0164), static_cast<wchar_t>(0x0166),
    static_cast<wchar_t>(0x0168), static_cast<wchar_t>(0x016A), static_cast<wchar_t>(0x016C),
    static_cast<wchar_t>(0x016E), static_cast<wchar_t>(0x0170), static_cast<wchar_t>(0x0172),
    static_cast<wchar_t>(0x0174), static_cast<wchar_t>(0x0176), static_cast<wchar_t>(0x0179),
    static_cast<wchar_t>(0x017B), static_cast<wchar_t>(0x017D), static_cast<wchar_t>(0x0182),
    static_cast<wchar_t>(0x0184), static_cast<wchar_t>(0x0187), static_cast<wchar_t>(0x018B),
    static_cast<wchar_t>(0x0191), static_cast<wchar_t>(0x0198), static_cast<wchar_t>(0x01A0),
    static_cast<wchar_t>(0x01A2), static_cast<wchar_t>(0x01A4), static_cast<wchar_t>(0x01A7),
    static_cast<wchar_t>(0x01AC), static_cast<wchar_t>(0x01AF), static_cast<wchar_t>(0x01B3),
    static_cast<wchar_t>(0x01B5), static_cast<wchar_t>(0x01B8), static_cast<wchar_t>(0x01BC),
    static_cast<wchar_t>(0x01C4), static_cast<wchar_t>(0x01C7), static_cast<wchar_t>(0x01CA),
    static_cast<wchar_t>(0x01CD), static_cast<wchar_t>(0x01CF), static_cast<wchar_t>(0x01D1),
    static_cast<wchar_t>(0x01D3), static_cast<wchar_t>(0x01D5), static_cast<wchar_t>(0x01D7),
    static_cast<wchar_t>(0x01D9), static_cast<wchar_t>(0x01DB), static_cast<wchar_t>(0x01DE),
    static_cast<wchar_t>(0x01E0), static_cast<wchar_t>(0x01E2), static_cast<wchar_t>(0x01E4),
    static_cast<wchar_t>(0x01E6), static_cast<wchar_t>(0x01E8), static_cast<wchar_t>(0x01EA),
    static_cast<wchar_t>(0x01EC), static_cast<wchar_t>(0x01EE), static_cast<wchar_t>(0x01F1),
    static_cast<wchar_t>(0x01F4), static_cast<wchar_t>(0x01FA), static_cast<wchar_t>(0x01FC),
    static_cast<wchar_t>(0x01FE), static_cast<wchar_t>(0x0200), static_cast<wchar_t>(0x0202),
    static_cast<wchar_t>(0x0204), static_cast<wchar_t>(0x0206), static_cast<wchar_t>(0x0208),
    static_cast<wchar_t>(0x020A), static_cast<wchar_t>(0x020C), static_cast<wchar_t>(0x020E),
    static_cast<wchar_t>(0x0210), static_cast<wchar_t>(0x0212), static_cast<wchar_t>(0x0214),
    static_cast<wchar_t>(0x0216), static_cast<wchar_t>(0x0181), static_cast<wchar_t>(0x0186),
    static_cast<wchar_t>(0x018A), static_cast<wchar_t>(0x018E), static_cast<wchar_t>(0x018F),
    static_cast<wchar_t>(0x0190), static_cast<wchar_t>(0x0193), static_cast<wchar_t>(0x0194),
    static_cast<wchar_t>(0x0197), static_cast<wchar_t>(0x0196), static_cast<wchar_t>(0x019C),
    static_cast<wchar_t>(0x019D), static_cast<wchar_t>(0x019F), static_cast<wchar_t>(0x01A9),
    static_cast<wchar_t>(0x01AE), static_cast<wchar_t>(0x01B1), static_cast<wchar_t>(0x01B2),
    static_cast<wchar_t>(0x01B7), static_cast<wchar_t>(0x0386), static_cast<wchar_t>(0x0388),
    static_cast<wchar_t>(0x0389), static_cast<wchar_t>(0x038A), static_cast<wchar_t>(0x0391),
    static_cast<wchar_t>(0x0392), static_cast<wchar_t>(0x0393), static_cast<wchar_t>(0x0394),
    static_cast<wchar_t>(0x0395), static_cast<wchar_t>(0x0396), static_cast<wchar_t>(0x0397),
    static_cast<wchar_t>(0x0398), static_cast<wchar_t>(0x0399), static_cast<wchar_t>(0x039A),
    static_cast<wchar_t>(0x039B), static_cast<wchar_t>(0x039C), static_cast<wchar_t>(0x039D),
    static_cast<wchar_t>(0x039E), static_cast<wchar_t>(0x039F), static_cast<wchar_t>(0x03A0),
    static_cast<wchar_t>(0x03A1), static_cast<wchar_t>(0x03A3), static_cast<wchar_t>(0x03A4),
    static_cast<wchar_t>(0x03A5), static_cast<wchar_t>(0x03A6), static_cast<wchar_t>(0x03A7),
    static_cast<wchar_t>(0x03A8), static_cast<wchar_t>(0x03A9), static_cast<wchar_t>(0x03AA),
    static_cast<wchar_t>(0x03AB), static_cast<wchar_t>(0x038C), static_cast<wchar_t>(0x038E),
    static_cast<wchar_t>(0x038F), static_cast<wchar_t>(0x03E2), static_cast<wchar_t>(0x03E4),
    static_cast<wchar_t>(0x03E6), static_cast<wchar_t>(0x03E8), static_cast<wchar_t>(0x03EA),
    static_cast<wchar_t>(0x03EC), static_cast<wchar_t>(0x03EE), static_cast<wchar_t>(0x0410),
    static_cast<wchar_t>(0x0411), static_cast<wchar_t>(0x0412), static_cast<wchar_t>(0x0413),
    static_cast<wchar_t>(0x0414), static_cast<wchar_t>(0x0415), static_cast<wchar_t>(0x0416),
    static_cast<wchar_t>(0x0417), static_cast<wchar_t>(0x0418), static_cast<wchar_t>(0x0419),
    static_cast<wchar_t>(0x041A), static_cast<wchar_t>(0x041B), static_cast<wchar_t>(0x041C),
    static_cast<wchar_t>(0x041D), static_cast<wchar_t>(0x041E), static_cast<wchar_t>(0x041F),
    static_cast<wchar_t>(0x0420), static_cast<wchar_t>(0x0421), static_cast<wchar_t>(0x0422),
    static_cast<wchar_t>(0x0423), static_cast<wchar_t>(0x0424), static_cast<wchar_t>(0x0425),
    static_cast<wchar_t>(0x0426), static_cast<wchar_t>(0x0427), static_cast<wchar_t>(0x0428),
    static_cast<wchar_t>(0x0429), static_cast<wchar_t>(0x042A), static_cast<wchar_t>(0x042B),
    static_cast<wchar_t>(0x042C), static_cast<wchar_t>(0x042D), static_cast<wchar_t>(0x042E),
    static_cast<wchar_t>(0x042F), static_cast<wchar_t>(0x0401), static_cast<wchar_t>(0x0402),
    static_cast<wchar_t>(0x0403), static_cast<wchar_t>(0x0404), static_cast<wchar_t>(0x0405),
    static_cast<wchar_t>(0x0406), static_cast<wchar_t>(0x0407), static_cast<wchar_t>(0x0408),
    static_cast<wchar_t>(0x0409), static_cast<wchar_t>(0x040A), static_cast<wchar_t>(0x040B),
    static_cast<wchar_t>(0x040C), static_cast<wchar_t>(0x040E), static_cast<wchar_t>(0x040F),
    static_cast<wchar_t>(0x0460), static_cast<wchar_t>(0x0462), static_cast<wchar_t>(0x0464),
    static_cast<wchar_t>(0x0466), static_cast<wchar_t>(0x0468), static_cast<wchar_t>(0x046A),
    static_cast<wchar_t>(0x046C), static_cast<wchar_t>(0x046E), static_cast<wchar_t>(0x0470),
    static_cast<wchar_t>(0x0472), static_cast<wchar_t>(0x0474), static_cast<wchar_t>(0x0476),
    static_cast<wchar_t>(0x0478), static_cast<wchar_t>(0x047A), static_cast<wchar_t>(0x047C),
    static_cast<wchar_t>(0x047E), static_cast<wchar_t>(0x0480), static_cast<wchar_t>(0x0490),
    static_cast<wchar_t>(0x0492), static_cast<wchar_t>(0x0494), static_cast<wchar_t>(0x0496),
    static_cast<wchar_t>(0x0498), static_cast<wchar_t>(0x049A), static_cast<wchar_t>(0x049C),
    static_cast<wchar_t>(0x049E), static_cast<wchar_t>(0x04A0), static_cast<wchar_t>(0x04A2),
    static_cast<wchar_t>(0x04A4), static_cast<wchar_t>(0x04A6), static_cast<wchar_t>(0x04A8),
    static_cast<wchar_t>(0x04AA), static_cast<wchar_t>(0x04AC), static_cast<wchar_t>(0x04AE),
    static_cast<wchar_t>(0x04B0), static_cast<wchar_t>(0x04B2), static_cast<wchar_t>(0x04B4),
    static_cast<wchar_t>(0x04B6), static_cast<wchar_t>(0x04B8), static_cast<wchar_t>(0x04BA),
    static_cast<wchar_t>(0x04BC), static_cast<wchar_t>(0x04BE), static_cast<wchar_t>(0x04C1),
    static_cast<wchar_t>(0x04C3), static_cast<wchar_t>(0x04C7), static_cast<wchar_t>(0x04CB),
    static_cast<wchar_t>(0x04D0), static_cast<wchar_t>(0x04D2), static_cast<wchar_t>(0x04D4),
    static_cast<wchar_t>(0x04D6), static_cast<wchar_t>(0x04D8), static_cast<wchar_t>(0x04DA),
    static_cast<wchar_t>(0x04DC), static_cast<wchar_t>(0x04DE), static_cast<wchar_t>(0x04E0),
    static_cast<wchar_t>(0x04E2), static_cast<wchar_t>(0x04E4), static_cast<wchar_t>(0x04E6),
    static_cast<wchar_t>(0x04E8), static_cast<wchar_t>(0x04EA), static_cast<wchar_t>(0x04EE),
    static_cast<wchar_t>(0x04F0), static_cast<wchar_t>(0x04F2), static_cast<wchar_t>(0x04F4),
    static_cast<wchar_t>(0x04F8), static_cast<wchar_t>(0x0531), static_cast<wchar_t>(0x0532),
    static_cast<wchar_t>(0x0533), static_cast<wchar_t>(0x0534), static_cast<wchar_t>(0x0535),
    static_cast<wchar_t>(0x0536), static_cast<wchar_t>(0x0537), static_cast<wchar_t>(0x0538),
    static_cast<wchar_t>(0x0539), static_cast<wchar_t>(0x053A), static_cast<wchar_t>(0x053B),
    static_cast<wchar_t>(0x053C), static_cast<wchar_t>(0x053D), static_cast<wchar_t>(0x053E),
    static_cast<wchar_t>(0x053F), static_cast<wchar_t>(0x0540), static_cast<wchar_t>(0x0541),
    static_cast<wchar_t>(0x0542), static_cast<wchar_t>(0x0543), static_cast<wchar_t>(0x0544),
    static_cast<wchar_t>(0x0545), static_cast<wchar_t>(0x0546), static_cast<wchar_t>(0x0547),
    static_cast<wchar_t>(0x0548), static_cast<wchar_t>(0x0549), static_cast<wchar_t>(0x054A),
    static_cast<wchar_t>(0x054B), static_cast<wchar_t>(0x054C), static_cast<wchar_t>(0x054D),
    static_cast<wchar_t>(0x054E), static_cast<wchar_t>(0x054F), static_cast<wchar_t>(0x0550),
    static_cast<wchar_t>(0x0551), static_cast<wchar_t>(0x0552), static_cast<wchar_t>(0x0553),
    static_cast<wchar_t>(0x0554), static_cast<wchar_t>(0x0555), static_cast<wchar_t>(0x0556),
    static_cast<wchar_t>(0x10A0), static_cast<wchar_t>(0x10A1), static_cast<wchar_t>(0x10A2),
    static_cast<wchar_t>(0x10A3), static_cast<wchar_t>(0x10A4), static_cast<wchar_t>(0x10A5),
    static_cast<wchar_t>(0x10A6), static_cast<wchar_t>(0x10A7), static_cast<wchar_t>(0x10A8),
    static_cast<wchar_t>(0x10A9), static_cast<wchar_t>(0x10AA), static_cast<wchar_t>(0x10AB),
    static_cast<wchar_t>(0x10AC), static_cast<wchar_t>(0x10AD), static_cast<wchar_t>(0x10AE),
    static_cast<wchar_t>(0x10AF), static_cast<wchar_t>(0x10B0), static_cast<wchar_t>(0x10B1),
    static_cast<wchar_t>(0x10B2), static_cast<wchar_t>(0x10B3), static_cast<wchar_t>(0x10B4),
    static_cast<wchar_t>(0x10B5), static_cast<wchar_t>(0x10B6), static_cast<wchar_t>(0x10B7),
    static_cast<wchar_t>(0x10B8), static_cast<wchar_t>(0x10B9), static_cast<wchar_t>(0x10BA),
    static_cast<wchar_t>(0x10BB), static_cast<wchar_t>(0x10BC), static_cast<wchar_t>(0x10BD),
    static_cast<wchar_t>(0x10BE), static_cast<wchar_t>(0x10BF), static_cast<wchar_t>(0x10C0),
    static_cast<wchar_t>(0x10C1), static_cast<wchar_t>(0x10C2), static_cast<wchar_t>(0x10C3),
    static_cast<wchar_t>(0x10C4), static_cast<wchar_t>(0x10C5), static_cast<wchar_t>(0x1E00),
    static_cast<wchar_t>(0x1E02), static_cast<wchar_t>(0x1E04), static_cast<wchar_t>(0x1E06),
    static_cast<wchar_t>(0x1E08), static_cast<wchar_t>(0x1E0A), static_cast<wchar_t>(0x1E0C),
    static_cast<wchar_t>(0x1E0E), static_cast<wchar_t>(0x1E10), static_cast<wchar_t>(0x1E12),
    static_cast<wchar_t>(0x1E14), static_cast<wchar_t>(0x1E16), static_cast<wchar_t>(0x1E18),
    static_cast<wchar_t>(0x1E1A), static_cast<wchar_t>(0x1E1C), static_cast<wchar_t>(0x1E1E),
    static_cast<wchar_t>(0x1E20), static_cast<wchar_t>(0x1E22), static_cast<wchar_t>(0x1E24),
    static_cast<wchar_t>(0x1E26), static_cast<wchar_t>(0x1E28), static_cast<wchar_t>(0x1E2A),
    static_cast<wchar_t>(0x1E2C), static_cast<wchar_t>(0x1E2E), static_cast<wchar_t>(0x1E30),
    static_cast<wchar_t>(0x1E32), static_cast<wchar_t>(0x1E34), static_cast<wchar_t>(0x1E36),
    static_cast<wchar_t>(0x1E38), static_cast<wchar_t>(0x1E3A), static_cast<wchar_t>(0x1E3C),
    static_cast<wchar_t>(0x1E3E), static_cast<wchar_t>(0x1E40), static_cast<wchar_t>(0x1E42),
    static_cast<wchar_t>(0x1E44), static_cast<wchar_t>(0x1E46), static_cast<wchar_t>(0x1E48),
    static_cast<wchar_t>(0x1E4A), static_cast<wchar_t>(0x1E4C), static_cast<wchar_t>(0x1E4E),
    static_cast<wchar_t>(0x1E50), static_cast<wchar_t>(0x1E52), static_cast<wchar_t>(0x1E54),
    static_cast<wchar_t>(0x1E56), static_cast<wchar_t>(0x1E58), static_cast<wchar_t>(0x1E5A),
    static_cast<wchar_t>(0x1E5C), static_cast<wchar_t>(0x1E5E), static_cast<wchar_t>(0x1E60),
    static_cast<wchar_t>(0x1E62), static_cast<wchar_t>(0x1E64), static_cast<wchar_t>(0x1E66),
    static_cast<wchar_t>(0x1E68), static_cast<wchar_t>(0x1E6A), static_cast<wchar_t>(0x1E6C),
    static_cast<wchar_t>(0x1E6E), static_cast<wchar_t>(0x1E70), static_cast<wchar_t>(0x1E72),
    static_cast<wchar_t>(0x1E74), static_cast<wchar_t>(0x1E76), static_cast<wchar_t>(0x1E78),
    static_cast<wchar_t>(0x1E7A), static_cast<wchar_t>(0x1E7C), static_cast<wchar_t>(0x1E7E),
    static_cast<wchar_t>(0x1E80), static_cast<wchar_t>(0x1E82), static_cast<wchar_t>(0x1E84),
    static_cast<wchar_t>(0x1E86), static_cast<wchar_t>(0x1E88), static_cast<wchar_t>(0x1E8A),
    static_cast<wchar_t>(0x1E8C), static_cast<wchar_t>(0x1E8E), static_cast<wchar_t>(0x1E90),
    static_cast<wchar_t>(0x1E92), static_cast<wchar_t>(0x1E94), static_cast<wchar_t>(0x1EA0),
    static_cast<wchar_t>(0x1EA2), static_cast<wchar_t>(0x1EA4), static_cast<wchar_t>(0x1EA6),
    static_cast<wchar_t>(0x1EA8), static_cast<wchar_t>(0x1EAA), static_cast<wchar_t>(0x1EAC),
    static_cast<wchar_t>(0x1EAE), static_cast<wchar_t>(0x1EB0), static_cast<wchar_t>(0x1EB2),
    static_cast<wchar_t>(0x1EB4), static_cast<wchar_t>(0x1EB6), static_cast<wchar_t>(0x1EB8),
    static_cast<wchar_t>(0x1EBA), static_cast<wchar_t>(0x1EBC), static_cast<wchar_t>(0x1EBE),
    static_cast<wchar_t>(0x1EC0), static_cast<wchar_t>(0x1EC2), static_cast<wchar_t>(0x1EC4),
    static_cast<wchar_t>(0x1EC6), static_cast<wchar_t>(0x1EC8), static_cast<wchar_t>(0x1ECA),
    static_cast<wchar_t>(0x1ECC), static_cast<wchar_t>(0x1ECE), static_cast<wchar_t>(0x1ED0),
    static_cast<wchar_t>(0x1ED2), static_cast<wchar_t>(0x1ED4), static_cast<wchar_t>(0x1ED6),
    static_cast<wchar_t>(0x1ED8), static_cast<wchar_t>(0x1EDA), static_cast<wchar_t>(0x1EDC),
    static_cast<wchar_t>(0x1EDE), static_cast<wchar_t>(0x1EE0), static_cast<wchar_t>(0x1EE2),
    static_cast<wchar_t>(0x1EE4), static_cast<wchar_t>(0x1EE6), static_cast<wchar_t>(0x1EE8),
    static_cast<wchar_t>(0x1EEA), static_cast<wchar_t>(0x1EEC), static_cast<wchar_t>(0x1EEE),
    static_cast<wchar_t>(0x1EF0), static_cast<wchar_t>(0x1EF2), static_cast<wchar_t>(0x1EF4),
    static_cast<wchar_t>(0x1EF6), static_cast<wchar_t>(0x1EF8), static_cast<wchar_t>(0x1F08),
    static_cast<wchar_t>(0x1F09), static_cast<wchar_t>(0x1F0A), static_cast<wchar_t>(0x1F0B),
    static_cast<wchar_t>(0x1F0C), static_cast<wchar_t>(0x1F0D), static_cast<wchar_t>(0x1F0E),
    static_cast<wchar_t>(0x1F0F), static_cast<wchar_t>(0x1F18), static_cast<wchar_t>(0x1F19),
    static_cast<wchar_t>(0x1F1A), static_cast<wchar_t>(0x1F1B), static_cast<wchar_t>(0x1F1C),
    static_cast<wchar_t>(0x1F1D), static_cast<wchar_t>(0x1F28), static_cast<wchar_t>(0x1F29),
    static_cast<wchar_t>(0x1F2A), static_cast<wchar_t>(0x1F2B), static_cast<wchar_t>(0x1F2C),
    static_cast<wchar_t>(0x1F2D), static_cast<wchar_t>(0x1F2E), static_cast<wchar_t>(0x1F2F),
    static_cast<wchar_t>(0x1F38), static_cast<wchar_t>(0x1F39), static_cast<wchar_t>(0x1F3A),
    static_cast<wchar_t>(0x1F3B), static_cast<wchar_t>(0x1F3C), static_cast<wchar_t>(0x1F3D),
    static_cast<wchar_t>(0x1F3E), static_cast<wchar_t>(0x1F3F), static_cast<wchar_t>(0x1F48),
    static_cast<wchar_t>(0x1F49), static_cast<wchar_t>(0x1F4A), static_cast<wchar_t>(0x1F4B),
    static_cast<wchar_t>(0x1F4C), static_cast<wchar_t>(0x1F4D), static_cast<wchar_t>(0x1F59),
    static_cast<wchar_t>(0x1F5B), static_cast<wchar_t>(0x1F5D), static_cast<wchar_t>(0x1F5F),
    static_cast<wchar_t>(0x1F68), static_cast<wchar_t>(0x1F69), static_cast<wchar_t>(0x1F6A),
    static_cast<wchar_t>(0x1F6B), static_cast<wchar_t>(0x1F6C), static_cast<wchar_t>(0x1F6D),
    static_cast<wchar_t>(0x1F6E), static_cast<wchar_t>(0x1F6F), static_cast<wchar_t>(0x1F88),
    static_cast<wchar_t>(0x1F89), static_cast<wchar_t>(0x1F8A), static_cast<wchar_t>(0x1F8B),
    static_cast<wchar_t>(0x1F8C), static_cast<wchar_t>(0x1F8D), static_cast<wchar_t>(0x1F8E),
    static_cast<wchar_t>(0x1F8F), static_cast<wchar_t>(0x1F98), static_cast<wchar_t>(0x1F99),
    static_cast<wchar_t>(0x1F9A), static_cast<wchar_t>(0x1F9B), static_cast<wchar_t>(0x1F9C),
    static_cast<wchar_t>(0x1F9D), static_cast<wchar_t>(0x1F9E), static_cast<wchar_t>(0x1F9F),
    static_cast<wchar_t>(0x1FA8), static_cast<wchar_t>(0x1FA9), static_cast<wchar_t>(0x1FAA),
    static_cast<wchar_t>(0x1FAB), static_cast<wchar_t>(0x1FAC), static_cast<wchar_t>(0x1FAD),
    static_cast<wchar_t>(0x1FAE), static_cast<wchar_t>(0x1FAF), static_cast<wchar_t>(0x1FB8),
    static_cast<wchar_t>(0x1FB9), static_cast<wchar_t>(0x1FD8), static_cast<wchar_t>(0x1FD9),
    static_cast<wchar_t>(0x1FE8), static_cast<wchar_t>(0x1FE9), static_cast<wchar_t>(0x24B6),
    static_cast<wchar_t>(0x24B7), static_cast<wchar_t>(0x24B8), static_cast<wchar_t>(0x24B9),
    static_cast<wchar_t>(0x24BA), static_cast<wchar_t>(0x24BB), static_cast<wchar_t>(0x24BC),
    static_cast<wchar_t>(0x24BD), static_cast<wchar_t>(0x24BE), static_cast<wchar_t>(0x24BF),
    static_cast<wchar_t>(0x24C0), static_cast<wchar_t>(0x24C1), static_cast<wchar_t>(0x24C2),
    static_cast<wchar_t>(0x24C3), static_cast<wchar_t>(0x24C4), static_cast<wchar_t>(0x24C5),
    static_cast<wchar_t>(0x24C6), static_cast<wchar_t>(0x24C7), static_cast<wchar_t>(0x24C8),
    static_cast<wchar_t>(0x24C9), static_cast<wchar_t>(0x24CA), static_cast<wchar_t>(0x24CB),
    static_cast<wchar_t>(0x24CC), static_cast<wchar_t>(0x24CD), static_cast<wchar_t>(0x24CE),
    static_cast<wchar_t>(0x24CF), static_cast<wchar_t>(0xFF21), static_cast<wchar_t>(0xFF22),
    static_cast<wchar_t>(0xFF23), static_cast<wchar_t>(0xFF24), static_cast<wchar_t>(0xFF25),
    static_cast<wchar_t>(0xFF26), static_cast<wchar_t>(0xFF27), static_cast<wchar_t>(0xFF28),
    static_cast<wchar_t>(0xFF29), static_cast<wchar_t>(0xFF2A), static_cast<wchar_t>(0xFF2B),
    static_cast<wchar_t>(0xFF2C), static_cast<wchar_t>(0xFF2D), static_cast<wchar_t>(0xFF2E),
    static_cast<wchar_t>(0xFF2F), static_cast<wchar_t>(0xFF30), static_cast<wchar_t>(0xFF31),
    static_cast<wchar_t>(0xFF32), static_cast<wchar_t>(0xFF33), static_cast<wchar_t>(0xFF34),
    static_cast<wchar_t>(0xFF35), static_cast<wchar_t>(0xFF36), static_cast<wchar_t>(0xFF37),
    static_cast<wchar_t>(0xFF38), static_cast<wchar_t>(0xFF39), static_cast<wchar_t>(0xFF3A)};

namespace kodi
{
namespace tools
{

template<typename T, std::enable_if_t<!std::is_enum<T>::value, int> = 0>
constexpr auto&& EnumToInt(T&& arg) noexcept
{
  return arg;
}
template<typename T, std::enable_if_t<std::is_enum<T>::value, int> = 0>
constexpr auto EnumToInt(T&& arg) noexcept
{
  return static_cast<int>(arg);
}

//==============================================================================
/// @defgroup cpp_kodi_tools_StringUtils_Defs Definitions, structures and enumerators
/// @ingroup cpp_kodi_tools_StringUtils
/// @brief **Parts used within string util functions**\n
/// All to string functions associated data structures.
///
/// It is divided into individual modules that correspond to the respective
/// types.
///
///
///

//==============================================================================
/// @defgroup cpp_kodi_tools_StringUtils_Defs_TIME_FORMAT enum TIME_FORMAT
/// @ingroup cpp_kodi_tools_StringUtils_Defs
/// @brief TIME_FORMAT enum/bitmask used for formatting time strings.
///
/// Note the use of bitmasking, e.g. TIME_FORMAT_HH_MM_SS = TIME_FORMAT_HH | TIME_FORMAT_MM | TIME_FORMAT_SS
/// @sa kodi::tools::StringUtils::SecondsToTimeString
///
/// @note For InfoLabels use the equivalent value listed (bold) on the
/// description of each enum value.
///
/// <b>Example:</b> 3661 seconds => h=1, hh=01, m=1, mm=01, ss=01, hours=1, mins=61, secs=3661
///
///@{
enum TIME_FORMAT
{
  /// Usually used as the fallback value if the format value is empty
  TIME_FORMAT_GUESS = 0,

  /// <b>ss</b> - seconds only
  TIME_FORMAT_SS = 1,

  /// <b>mm</b> - minutes only (2-digit)
  TIME_FORMAT_MM = 2,

  /// <b>mm:ss</b> - minutes and seconds
  TIME_FORMAT_MM_SS = 3,

  /// <b>hh</b> - hours only (2-digit)
  TIME_FORMAT_HH = 4,

  /// <b>hh:ss</b> - hours and seconds (this is not particularly useful)
  TIME_FORMAT_HH_SS = 5,

  /// <b>hh:mm</b> - hours and minutes
  TIME_FORMAT_HH_MM = 6,

  /// <b>hh:mm:ss</b> - hours, minutes and seconds
  TIME_FORMAT_HH_MM_SS = 7,

  /// <b>xx</b> - returns AM/PM for a 12-hour clock
  TIME_FORMAT_XX = 8,

  /// <b>hh:mm xx</b> - returns hours and minutes in a 12-hour clock format (AM/PM)
  TIME_FORMAT_HH_MM_XX = 14,

  /// <b>hh:mm:ss xx</b> - returns hours (2-digit), minutes and seconds in a 12-hour clock format (AM/PM)
  TIME_FORMAT_HH_MM_SS_XX = 15,

  /// <b>h</b> - hours only (1-digit)
  TIME_FORMAT_H = 16,

  /// <b>hh:mm:ss</b> - hours, minutes and seconds
  TIME_FORMAT_H_MM_SS = 19,

  /// <b>hh:mm:ss xx</b> - returns hours (1-digit), minutes and seconds in a 12-hour clock format (AM/PM)
  TIME_FORMAT_H_MM_SS_XX = 27,

  /// <b>secs</b> - total time in seconds
  TIME_FORMAT_SECS = 32,

  /// <b>mins</b> - total time in minutes
  TIME_FORMAT_MINS = 64,

  /// <b>hours</b> - total time in hours
  TIME_FORMAT_HOURS = 128,

  /// <b>m</b> - minutes only (1-digit)
  TIME_FORMAT_M = 256
};
///@}
//------------------------------------------------------------------------------

//==============================================================================
/// @defgroup cpp_kodi_tools_StringUtils class StringUtils
/// @ingroup cpp_kodi_tools
/// @brief **C++ class for processing strings**\n
/// This class brings many different functions to edit, check or search texts.
///
/// Is intended to reduce any code work of C++ on addons and to have them faster
/// to use.
///
/// All functions are static within the <b>`kodi::tools::StringUtils`</b> class.
///
///@{
class StringUtils
{
public:
  //============================================================================
  /// @ingroup cpp_kodi_tools_StringUtils_Defs
  /// @brief Defines a static empty <b>`std::string`</b>.
  ///
  static const std::string Empty;
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_FormatControl String format
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **Formatting functions**\n
  /// Used to output the given values in newly formatted text using functions.
  ///
  /*!@{*/

  //============================================================================
  /// @brief Returns the C++ string pointed by given format. If format includes
  /// format specifiers (subsequences beginning with %), the additional arguments
  /// following format are formatted and inserted in the resulting string replacing
  /// their respective specifiers.
  ///
  /// After the format parameter, the function expects at least as many additional
  /// arguments as specified by format.
  ///
  /// @param[in] fmt The format of the text to process for output.
  ///                C string that contains the text to be written to the stream.
  ///                It can optionally contain embedded format specifiers that are
  ///                replaced by the values specified in subsequent additional
  ///                arguments and formatted as requested.
  ///  |  specifier | Output                                             | Example
  ///  |------------|----------------------------------------------------|------------
  ///  |  d or i    | Signed decimal integer                             | 392
  ///  |  u         | Unsigned decimal integer                           | 7235
  ///  |  o         | Unsigned octal                                     | 610
  ///  |  x         | Unsigned hexadecimal integer                       | 7fa
  ///  |  X         | Unsigned hexadecimal integer (uppercase)           | 7FA
  ///  |  f         | Decimal floating point, lowercase                  | 392.65
  ///  |  F         | Decimal floating point, uppercase                  | 392.65
  ///  |  e         | Scientific notation (mantissa/exponent), lowercase | 3.9265e+2
  ///  |  E         | Scientific notation (mantissa/exponent), uppercase | 3.9265E+2
  ///  |  g         | Use the shortest representation: %e or %f          | 392.65
  ///  |  G         | Use the shortest representation: %E or %F          | 392.65
  ///  |  a         | Hexadecimal floating point, lowercase              | -0xc.90fep-2
  ///  |  A         | Hexadecimal floating point, uppercase              | -0XC.90FEP-2
  ///  |  c         | Character                                          | a
  ///  |  s         | String of characters                               | sample
  ///  |  p         | Pointer address                                    | b8000000
  ///  |  %         | A % followed by another % character will write a single % to the stream. | %
  /// The length sub-specifier modifies the length of the data type. This is a chart
  /// showing the types used to interpret the corresponding arguments with and without
  /// length specifier (if a different type is used, the proper type promotion or
  /// conversion is performed, if allowed):
  ///  | length| d i           | u o x X               | f F e E g G a A | c     | s       | p       | n               |
  ///  |-------|---------------|-----------------------|-----------------|-------|---------|---------|-----------------|
  ///  | (none)| int           | unsigned int          | double          | int   | char*   | void*   | int*            |
  ///  | hh    | signed char   | unsigned char         |                 |       |         |         | signed char*    |
  ///  | h     | short int     | unsigned short int    |                 |       |         |         | short int*      |
  ///  | l     | long int      | unsigned long int     |                 | wint_t| wchar_t*|         | long int*       |
  ///  | ll    | long long int | unsigned long long int|                 |       |         |         | long long int*  |
  ///  | j     | intmax_t      | uintmax_t             |                 |       |         |         | intmax_t*       |
  ///  | z     | size_t        | size_t                |                 |       |         |         | size_t*         |
  ///  | t     | ptrdiff_t     | ptrdiff_t             |                 |       |         |         | ptrdiff_t*      |
  ///  | L     |               |                       | long double     |       |         |         |                 |
  ///  <b>Note:</b> that the c specifier takes an int (or wint_t) as argument, but performs the proper conversion to a char value
  ///  (or a wchar_t) before formatting it for output.
  /// @param[in] ... <i>(additional arguments)</i>\n
  ///                Depending on the format string, the function may expect a
  ///                sequence of additional arguments, each containing a value
  ///                to be used to replace a format specifier in the format
  ///                string (or a pointer to a storage location, for n).\n
  ///                There should be at least as many of these arguments as the
  ///                number of values specified in the format specifiers.
  ///                Additional arguments are ignored by the function.
  /// @return Formatted string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string str = kodi::tools::StringUtils::Format("Hello {} {}", "World", 2020);
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string Format(const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    std::string str = FormatV(fmt, args);
    va_end(args);

    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns the C++ wide string pointed by given format.
  ///
  /// @param[in] fmt The format of the text to process for output
  ///                (see @ref Format(const char* fmt, ...) for details).
  /// @param[in] ... <i>(additional arguments)</i>\n
  ///                Depending on the format string, the function may expect a
  ///                sequence of additional arguments, each containing a value
  ///                to be used to replace a format specifier in the format
  ///                string (or a pointer to a storage location, for n).\n
  ///                There should be at least as many of these arguments as the
  ///                number of values specified in the format specifiers.
  ///                Additional arguments are ignored by the function.
  /// @return Formatted string
  ///
  inline static std::wstring Format(const wchar_t* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    std::wstring str = FormatV(fmt, args);
    va_end(args);

    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns the C++ string pointed by given format list.
  ///
  /// @param[in] fmt The format of the text to process for output
  ///                (see @ref Format(const char* fmt, ...) for details).
  /// @param[in] args A value identifying a variable arguments list initialized
  ///                 with `va_start`.
  /// @return Formatted string
  ///
  inline static std::string FormatV(PRINTF_FORMAT_STRING const char* fmt, va_list args)
  {
    if (!fmt || !fmt[0])
      return "";

    int size = FORMAT_BLOCK_SIZE;
    va_list argCopy;

    while (true)
    {
      char* cstr = reinterpret_cast<char*>(malloc(sizeof(char) * size));
      if (!cstr)
        return "";

      va_copy(argCopy, args);
      int nActual = vsnprintf(cstr, size, fmt, argCopy);
      va_end(argCopy);

      if (nActual > -1 && nActual < size) // We got a valid result
      {
        std::string str(cstr, nActual);
        free(cstr);
        return str;
      }
      free(cstr);
#ifndef TARGET_WINDOWS
      if (nActual > -1) // Exactly what we will need (glibc 2.1)
        size = nActual + 1;
      else // Let's try to double the size (glibc 2.0)
        size *= 2;
#else // TARGET_WINDOWS
      va_copy(argCopy, args);
      size = _vscprintf(fmt, argCopy);
      va_end(argCopy);
      if (size < 0)
        return "";
      else
        size++; // increment for null-termination
#endif // TARGET_WINDOWS
    }

    return ""; // unreachable
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns the C++ wide string pointed by given format list.
  ///
  /// @param[in] fmt The format of the text to process for output
  ///                (see @ref Format(const char* fmt, ...) for details).
  /// @param[in] args A value identifying a variable arguments list initialized
  ///                 with `va_start`.
  /// @return Formatted string
  ///
  inline static std::wstring FormatV(PRINTF_FORMAT_STRING const wchar_t* fmt, va_list args)
  {
    if (!fmt || !fmt[0])
      return L"";

    int size = FORMAT_BLOCK_SIZE;
    va_list argCopy;

    while (true)
    {
      wchar_t* cstr = reinterpret_cast<wchar_t*>(malloc(sizeof(wchar_t) * size));
      if (!cstr)
        return L"";

      va_copy(argCopy, args);
      int nActual = vswprintf(cstr, size, fmt, argCopy);
      va_end(argCopy);

      if (nActual > -1 && nActual < size) // We got a valid result
      {
        std::wstring str(cstr, nActual);
        free(cstr);
        return str;
      }
      free(cstr);

#ifndef TARGET_WINDOWS
      if (nActual > -1) // Exactly what we will need (glibc 2.1)
        size = nActual + 1;
      else // Let's try to double the size (glibc 2.0)
        size *= 2;
#else // TARGET_WINDOWS
      va_copy(argCopy, args);
      size = _vscwprintf(fmt, argCopy);
      va_end(argCopy);
      if (size < 0)
        return L"";
      else
        size++; // increment for null-termination
#endif // TARGET_WINDOWS
    }

    return L"";
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns bytes in a human readable format using the smallest unit
  /// that will fit `bytes` in at most three digits. The number of decimals are
  /// adjusted with significance such that 'small' numbers will have more
  /// decimals than larger ones.
  ///
  /// For example: 1024 bytes will be formatted as "1.00kB", 10240 bytes as
  /// "10.0kB" and 102400 bytes as "100kB". See TestStringUtils for more
  /// examples.
  ///
  /// Supported file sizes:
  /// | Value      | Short | Metric
  /// |------------|-------|-----------
  /// | 1          | B     | byte
  /// | 1024¹      | kB    | kilobyte
  /// | 1024²      | MB    | megabyte
  /// | 1024³      | GB    | gigabyte
  /// | 1024 exp 4 | TB    | terabyte
  /// | 1024 exp 5 | PB    | petabyte
  /// | 1024 exp 6 | EB    | exabyte
  /// | 1024 exp 7 | ZB    | zettabyte
  /// | 1024 exp 8 | YB    | yottabyte
  ///
  /// @param[in] bytes Bytes amount to return as human readable string
  /// @return Size as string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_STREQ("0B", kodi::tools::StringUtils::FormatFileSize(0).c_str());
  ///
  /// EXPECT_STREQ("999B", kodi::tools::StringUtils::FormatFileSize(999).c_str());
  /// EXPECT_STREQ("0.98kB", kodi::tools::StringUtils::FormatFileSize(1000).c_str());
  ///
  /// EXPECT_STREQ("1.00kB", kodi::tools::StringUtils::FormatFileSize(1024).c_str());
  /// EXPECT_STREQ("9.99kB", kodi::tools::StringUtils::FormatFileSize(10229).c_str());
  ///
  /// EXPECT_STREQ("10.1kB", kodi::tools::StringUtils::FormatFileSize(10387).c_str());
  /// EXPECT_STREQ("99.9kB", kodi::tools::StringUtils::FormatFileSize(102297).c_str());
  ///
  /// EXPECT_STREQ("100kB", kodi::tools::StringUtils::FormatFileSize(102400).c_str());
  /// EXPECT_STREQ("999kB", kodi::tools::StringUtils::FormatFileSize(1023431).c_str());
  ///
  /// EXPECT_STREQ("0.98MB", kodi::tools::StringUtils::FormatFileSize(1023897).c_str());
  /// EXPECT_STREQ("0.98MB", kodi::tools::StringUtils::FormatFileSize(1024000).c_str());
  ///
  /// EXPECT_STREQ("5.30EB", kodi::tools::StringUtils::FormatFileSize(6115888293969133568).c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string FormatFileSize(uint64_t bytes)
  {
    const std::array<std::string, 9> units{{"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"}};
    if (bytes < 1000)
      return Format("%" PRIu64 "B", bytes);

    size_t i = 0;
    double value = static_cast<double>(bytes);
    while (i + 1 < units.size() && value >= 999.5)
    {
      ++i;
      value /= 1024.0;
    }
    unsigned int decimals = value < 9.995 ? 2 : (value < 99.95 ? 1 : 0);
    auto frmt = "%." + Format("%u", decimals) + "f%s";
    return Format(frmt.c_str(), value, units[i].c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert the string of binary chars to the actual string.
  ///
  /// Convert the string representation of binary chars to the actual string.
  /// For example <b>`\1\2\3`</b> is converted to a string with binary char
  /// <b>`\1`</b>, <b>`\2`</b> and <b>`\3`</b>
  ///
  /// @param[in] in String to convert
  /// @return Converted string
  ///
  inline static std::string BinaryStringToString(const std::string& in)
  {
    std::string out;
    out.reserve(in.size() / 2);
    for (const char *cur = in.c_str(), *end = cur + in.size(); cur != end; ++cur)
    {
      if (*cur == '\\')
      {
        ++cur;
        if (cur == end)
        {
          break;
        }
        if (isdigit(*cur))
        {
          char* end;
          unsigned long num = strtol(cur, &end, 10);
          cur = end - 1;
          out.push_back(static_cast<char>(num));
          continue;
        }
      }
      out.push_back(*cur);
    }
    return out;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert each character in the string to its hexadecimal
  /// representation and return the concatenated result
  ///
  /// Example: "abc\n" -> "6162630a"
  ///
  /// @param[in] in String to convert
  /// @return Converted string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_STREQ("", kodi::tools::StringUtils::ToHexadecimal("").c_str());
  /// EXPECT_STREQ("616263", kodi::tools::StringUtils::ToHexadecimal("abc").c_str());
  /// std::string a{"a\0b\n", 4};
  /// EXPECT_STREQ("6100620a", kodi::tools::StringUtils::ToHexadecimal(a).c_str());
  /// std::string nul{"\0", 1};
  /// EXPECT_STREQ("00", kodi::tools::StringUtils::ToHexadecimal(nul).c_str());
  /// std::string ff{"\xFF", 1};
  /// EXPECT_STREQ("ff", kodi::tools::StringUtils::ToHexadecimal(ff).c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string ToHexadecimal(const std::string& in)
  {
    std::ostringstream ss;
    ss << std::hex;
    for (unsigned char ch : in)
    {
      ss << std::setw(2) << std::setfill('0') << static_cast<unsigned long>(ch);
    }
    return ss.str();
  }
  //----------------------------------------------------------------------------

  /*!@}*/

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_EditControl String edit
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **Edits given texts**\n
  /// This is used to revise the respective strings and to get them in the desired format.
  ///
  /*!@{*/

  //============================================================================
  /// @brief Convert a string to uppercase.
  ///
  /// @param[in,out] str String to convert
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "TEST";
  ///
  /// std::string varstr = "TeSt";
  /// kodi::tools::StringUtils::ToUpper(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static void ToUpper(std::string& str)
  {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert a 16bit wide string to uppercase.
  ///
  /// @param[in,out] str String to convert
  ///
  inline static void ToUpper(std::wstring& str)
  {
    transform(str.begin(), str.end(), str.begin(), toupperUnicode);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert a string to lowercase.
  ///
  /// @param[in,out] str String to convert
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "test";
  ///
  /// std::string varstr = "TeSt";
  /// kodi::tools::StringUtils::ToLower(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static void ToLower(std::string& str)
  {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert a 16bit wide string to lowercase.
  ///
  /// @param[in,out] str String to convert
  ///
  inline static void ToLower(std::wstring& str)
  {
    transform(str.begin(), str.end(), str.begin(), tolowerUnicode);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Combine all numerical digits and give it as integer value.
  ///
  /// @param[in,out] str String to check for digits
  /// @return All numerical digits fit together as integer value
  ///
  inline static int ReturnDigits(const std::string& str)
  {
    std::stringstream ss;
    for (const auto& character : str)
    {
      if (isdigit(character))
        ss << character;
    }
    return atoi(ss.str().c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns a string from start with givent count.
  ///
  /// @param[in] str String to use
  /// @param[in] count Amount of characters to go from left
  /// @return The left part string in amount of given count or complete if it
  ///         was higher.
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr, varstr;
  /// std::string origstr = "test";
  ///
  /// refstr = "";
  /// varstr = kodi::tools::StringUtils::Left(origstr, 0);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "te";
  /// varstr = kodi::tools::StringUtils::Left(origstr, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "test";
  /// varstr = kodi::tools::StringUtils::Left(origstr, 10);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string Left(const std::string& str, size_t count)
  {
    count = std::max(static_cast<size_t>(0), std::min(count, str.size()));
    return str.substr(0, count);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Get substring from mid of given string.
  ///
  /// @param[in] str String to get substring from
  /// @param[in] first Position from where to start
  /// @param[in] count [opt] length of position to get after start, default is
  ///                  complete to end
  /// @return The substring taken from middle of input string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr, varstr;
  /// std::string origstr = "test";
  ///
  /// refstr = "";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 0, 0);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "te";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 0, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "test";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 0, 10);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "st";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "st";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 2, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "es";
  /// varstr = kodi::tools::StringUtils::Mid(origstr, 1, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string Mid(const std::string& str,
                                size_t first,
                                size_t count = std::string::npos)
  {
    if (first + count > str.size())
      count = str.size() - first;

    if (first > str.size())
      return std::string();

    assert(first + count <= str.size());

    return str.substr(first, count);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Returns a string from end with givent count.
  ///
  /// @param[in] str String to use
  /// @param[in] count Amount of characters to go from right
  /// @return The right part string in amount of given count or complete if it
  ///         was higher.
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr, varstr;
  /// std::string origstr = "test";
  ///
  /// refstr = "";
  /// varstr = kodi::tools::StringUtils::Right(origstr, 0);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "st";
  /// varstr = kodi::tools::StringUtils::Right(origstr, 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// refstr = "test";
  /// varstr = kodi::tools::StringUtils::Right(origstr, 10);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string Right(const std::string& str, size_t count)
  {
    count = std::max(static_cast<size_t>(0), std::min(count, str.size()));
    return str.substr(str.size() - count);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted spaces at begin and end
  /// of string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @return The changed string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "test test";
  ///
  /// std::string varstr = " test test   ";
  /// kodi::tools::StringUtils::Trim(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string& Trim(std::string& str)
  {
    TrimLeft(str);
    return TrimRight(str);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted characters at begin and end
  /// of string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @param[in] chars Characters to use for trim
  /// @return The changed string
  ///
  inline static std::string& Trim(std::string& str, const char* const chars)
  {
    TrimLeft(str, chars);
    return TrimRight(str, chars);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted spaces at begin of string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @return The changed string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "test test   ";
  ///
  /// std::string varstr = " test test   ";
  /// kodi::tools::StringUtils::TrimLeft(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string& TrimLeft(std::string& str)
  {
    str.erase(str.begin(),
              std::find_if(str.begin(), str.end(), [](char s) { return IsSpace(s) == 0; }));
    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted characters at begin of
  /// string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @param[in] chars Characters to use for trim
  /// @return The changed string
  ///
  inline static std::string& TrimLeft(std::string& str, const char* const chars)
  {
    size_t nidx = str.find_first_not_of(chars);
    str.erase(0, nidx);
    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted spaces at end of string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @return The changed string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = " test test";
  ///
  /// std::string varstr = " test test   ";
  /// kodi::tools::StringUtils::TrimRight(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string& TrimRight(std::string& str)
  {
    str.erase(std::find_if(str.rbegin(), str.rend(), [](char s) { return IsSpace(s) == 0; }).base(),
              str.end());
    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Trim a string with remove of not wanted characters at end of
  /// string.
  ///
  /// @param[in,out] str String to trim, becomes also changed and given on
  ///                    return
  /// @param[in] chars Characters to use for trim
  /// @return The changed string
  ///
  inline static std::string& TrimRight(std::string& str, const char* const chars)
  {
    size_t nidx = str.find_last_not_of(chars);
    str.erase(str.npos == nidx ? 0 : ++nidx);
    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Cleanup string by remove of duplicates of spaces and tabs.
  ///
  /// @param[in,out] str String to remove duplicates, becomes also changed and
  ///                    given further on return
  /// @return The changed string
  ///
  inline static std::string& RemoveDuplicatedSpacesAndTabs(std::string& str)
  {
    std::string::iterator it = str.begin();
    bool onSpace = false;
    while (it != str.end())
    {
      if (*it == '\t')
        *it = ' ';

      if (*it == ' ')
      {
        if (onSpace)
        {
          it = str.erase(it);
          continue;
        }
        else
          onSpace = true;
      }
      else
        onSpace = false;

      ++it;
    }
    return str;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Replace a character with another inside text string.
  ///
  /// @param[in] str String to replace within
  /// @param[in] oldChar Character to search for replacement
  /// @param[in] newChar New character to use for replacement
  /// @return Amount of replaced characters
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "text text";
  ///
  /// std::string varstr = "test test";
  /// EXPECT_EQ(kodi::tools::StringUtils::Replace(varstr, 's', 'x'), 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// EXPECT_EQ(kodi::tools::StringUtils::Replace(varstr, 's', 'x'), 0);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static int Replace(std::string& str, char oldChar, char newChar)
  {
    int replacedChars = 0;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it)
    {
      if (*it == oldChar)
      {
        *it = newChar;
        replacedChars++;
      }
    }

    return replacedChars;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Replace a complete text with another inside text string.
  ///
  /// @param[in] str String to replace within
  /// @param[in] oldStr String to search for replacement
  /// @param[in] newStr New string to use for replacement
  /// @return Amount of replaced text fields
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "text text";
  ///
  /// std::string varstr = "test test";
  /// EXPECT_EQ(kodi::tools::StringUtils::Replace(varstr, "s", "x"), 2);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  ///
  /// EXPECT_EQ(kodi::tools::StringUtils::Replace(varstr, "s", "x"), 0);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static int Replace(std::string& str, const std::string& oldStr, const std::string& newStr)
  {
    if (oldStr.empty())
      return 0;

    int replacedChars = 0;
    size_t index = 0;

    while (index < str.size() && (index = str.find(oldStr, index)) != std::string::npos)
    {
      str.replace(index, oldStr.size(), newStr);
      index += newStr.size();
      replacedChars++;
    }

    return replacedChars;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Replace a complete text with another inside 16bit wide text string.
  ///
  /// @param[in] str String to replace within
  /// @param[in] oldStr String to search for replacement
  /// @param[in] newStr New string to use for replacement
  /// @return Amount of replaced text fields
  ///
  inline static int Replace(std::wstring& str,
                            const std::wstring& oldStr,
                            const std::wstring& newStr)
  {
    if (oldStr.empty())
      return 0;

    int replacedChars = 0;
    size_t index = 0;

    while (index < str.size() && (index = str.find(oldStr, index)) != std::string::npos)
    {
      str.replace(index, oldStr.size(), newStr);
      index += newStr.size();
      replacedChars++;
    }

    return replacedChars;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Transform characters to create a safe URL.
  ///
  /// @param[in] str The string to transform
  /// @return The transformed string, with unsafe characters replaced by "_"
  ///
  /// Safe URLs are composed of the unreserved characters defined in
  /// RFC 3986 section 2.3:
  ///
  ///   ALPHA / DIGIT / "-" / "." / "_" / "~"
  ///
  /// Characters outside of this set will be replaced by "_".
  ///
  inline static std::string MakeSafeUrl(const std::string& str)
  {
    std::string safeUrl;

    safeUrl.reserve(str.size());

    std::transform(str.begin(), str.end(), std::back_inserter(safeUrl),
                   [](char c)
                   {
                     if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
                         ('0' <= c && c <= '9') || c == '-' || c == '.' || c == '_' || c == '~')
                     {
                       return c;
                     }
                     return '_';
                   });

    return safeUrl;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Transform characters to create a safe, printable string.
  ///
  /// @param[in] str The string to transform
  /// @return The transformed string, with unsafe characters replaced by " "
  ///
  /// Unsafe characters are defined as the non-printable ASCII characters
  /// (character code 0-31).
  ///
  inline static std::string MakeSafeString(const std::string& str)
  {
    std::string safeString;

    safeString.reserve(str.size());

    std::transform(str.begin(), str.end(), std::back_inserter(safeString),
                   [](char c)
                   {
                     if (c < 0x20)
                       return ' ';

                     return c;
                   });

    return safeString;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Removes a MAC address from a given string.
  ///
  /// @param[in] str The string containing a MAC address
  /// @return The string without the MAC address (for chaining)
  ///
  inline static std::string RemoveMACAddress(const std::string& str)
  {
    std::regex re(R"mac([\(\[]?([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})[\)\]]?)mac");
    return std::regex_replace(str, re, "", std::regex_constants::format_default);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Remove carriage return and line feeds on string ends.
  ///
  /// @param[in,out] str String where CR and LF becomes removed on end
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr, varstr;
  ///
  /// refstr = "test\r\nstring\nblah blah";
  /// varstr = "test\r\nstring\nblah blah\n";
  /// kodi::tools::StringUtils::RemoveCRLF(varstr);
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static void RemoveCRLF(std::string& strLine) { StringUtils::TrimRight(strLine, "\n\r"); }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert a word to a digit numerical string
  ///
  /// @param[in] str String to convert
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// std::string ref, var;
  ///
  /// ref = "8378 787464";
  /// var = "test string";
  /// kodi::tools::StringUtils::WordToDigits(var);
  /// EXPECT_STREQ(ref.c_str(), var.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static void WordToDigits(std::string& word)
  {
    static const char word_to_letter[] = "22233344455566677778889999";
    StringUtils::ToLower(word);
    for (unsigned int i = 0; i < word.size(); ++i)
    { // NB: This assumes ascii, which probably needs extending at some point.
      char letter = word[i];
      if ((letter >= 'a' && letter <= 'z')) // assume contiguous letter range
      {
        word[i] = word_to_letter[letter - 'a'];
      }
      else if (letter < '0' || letter > '9') // We want to keep 0-9!
      {
        word[i] = ' '; // replace everything else with a space
      }
    }
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Escapes the given string to be able to be used as a parameter.
  ///
  /// Escapes backslashes and double-quotes with an additional backslash and
  ///  adds double-quotes around the whole string.
  ///
  /// @param[in] param String to escape/paramify
  /// @return Escaped/Paramified string
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// const char *input = "some, very \\ odd \"string\"";
  /// const char *ref   = "\"some, very \\\\ odd \\\"string\\\"\"";
  ///
  /// std::string result = kodi::tools::StringUtils::Paramify(input);
  /// EXPECT_STREQ(ref, result.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string Paramify(const std::string& param)
  {
    std::string result = param;
    // escape backspaces
    StringUtils::Replace(result, "\\", "\\\\");
    // escape double quotes
    StringUtils::Replace(result, "\"", "\\\"");

    // add double quotes around the whole string
    return "\"" + result + "\"";
  }
  //----------------------------------------------------------------------------

  /*!@}*/

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_CompareControl String compare
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **Check strings for the desired state**\n
  /// With this, texts can be checked to see that they correspond to a required
  /// format.
  ///
  /*!@{*/

  //============================================================================
  /// @brief Compare two strings with ignore of lower-/uppercase.
  ///
  /// @param[in] str1 C++ string to compare
  /// @param[in] str2 C++ string to compare
  /// @return True if the strings are equal, false otherwise
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr = "TeSt";
  ///
  /// EXPECT_TRUE(kodi::tools::StringUtils::EqualsNoCase(refstr, "TeSt"));
  /// EXPECT_TRUE(kodi::tools::StringUtils::EqualsNoCase(refstr, "tEsT"));
  /// ~~~~~~~~~~~~~
  ///
  inline static bool EqualsNoCase(const std::string& str1, const std::string& str2)
  {
    // before we do the char-by-char comparison, first compare sizes of both strings.
    // This led to a 33% improvement in benchmarking on average. (size() just returns a member of std::string)
    if (str1.size() != str2.size())
      return false;
    return EqualsNoCase(str1.c_str(), str2.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Compare two strings with ignore of lower-/uppercase.
  ///
  /// @param[in] str1 C++ string to compare
  /// @param[in] s2 C string to compare
  /// @return True if the strings are equal, false otherwise
  ///
  inline static bool EqualsNoCase(const std::string& str1, const char* s2)
  {
    return EqualsNoCase(str1.c_str(), s2);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Compare two strings with ignore of lower-/uppercase.
  ///
  /// @param[in] s1 C string to compare
  /// @param[in] s2 C string to compare
  /// @return True if the strings are equal, false otherwise
  ///
  inline static bool EqualsNoCase(const char* s1, const char* s2)
  {
    char c2; // we need only one char outside the loop
    do
    {
      const char c1 = *s1++; // const local variable should help compiler to optimize
      c2 = *s2++;
      // This includes the possibility that one of the characters is the null-terminator,
      // which implies a string mismatch.
      if (c1 != c2 && ::tolower(c1) != ::tolower(c2))
        return false;
    } while (c2 != '\0'); // At this point, we know c1 == c2, so there's no need to test them both.
    return true;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Compare two strings with ignore of lower-/uppercase with given
  /// size.
  ///
  /// Equal to @ref EqualsNoCase only that size can defined and on return the
  /// difference between compared character becomes given.
  ///
  /// @param[in] str1 C++ string to compare
  /// @param[in] str2 C++ string to compare
  /// @param[in] n [opt] Length to check, 0 as default to make complete
  /// @return 0 if equal, otherwise difference of failed character in string to
  /// other ("a" - "b" = -1)
  ///
  inline static int CompareNoCase(const std::string& str1, const std::string& str2, size_t n = 0)
  {
    return CompareNoCase(str1.c_str(), str2.c_str(), n);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Compare two strings with ignore of lower-/uppercase with given
  /// size.
  ///
  /// Equal to @ref EqualsNoCase only that size can defined and on return the
  /// difference between compared character becomes given.
  ///
  /// @param[in] s1 C string to compare
  /// @param[in] s2 C string to compare
  /// @param[in] n [opt] Length to check, 0 as default to make complete
  /// @return 0 if equal, otherwise difference of failed character in string to
  /// other ("a" - "b" = -1)
  ///
  inline static int CompareNoCase(const char* s1, const char* s2, size_t n = 0)
  {
    char c2; // we need only one char outside the loop
    size_t index = 0;
    do
    {
      const char c1 = *s1++; // const local variable should help compiler to optimize
      c2 = *s2++;
      index++;
      // This includes the possibility that one of the characters is the null-terminator,
      // which implies a string mismatch.
      if (c1 != c2 && ::tolower(c1) != ::tolower(c2))
        return ::tolower(c1) - ::tolower(c2);
    } while (c2 != '\0' &&
             index != n); // At this point, we know c1 == c2, so there's no need to test them both.
    return 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] str2 C++ string with which text defined in str1 is checked at
  ///                 the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// bool ret;
  /// std::string refstr = "test";
  ///
  /// ret = kodi::tools::StringUtils::StartsWith(refstr, "te");
  /// fprintf(stderr, "Expect true for here and is '%s'\n", ret ? "true" : "false");
  ///
  /// ret = kodi::tools::StringUtils::StartsWith(refstr, "abc");
  /// fprintf(stderr, "Expect false for here and is '%s'\n", ret ? "true" : "false");
  /// ~~~~~~~~~~~~~
  ///
  inline static bool StartsWith(const std::string& str1, const std::string& str2)
  {
    return str1.compare(0, str2.size(), str2) == 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  inline static bool StartsWith(const std::string& str1, const char* s2)
  {
    return StartsWith(str1.c_str(), s2);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string.
  ///
  /// @param[in] s1 C string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  inline static bool StartsWith(const char* s1, const char* s2)
  {
    while (*s2 != '\0')
    {
      if (*s1 != *s2)
        return false;
      s1++;
      s2++;
    }
    return true;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string by ignore of
  /// upper-/lowercase.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] str2 C++ string with which text defined in str1 is checked at
  ///                 the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// bool ret;
  /// std::string refstr = "test";
  ///
  /// ret = kodi::tools::StringUtils::StartsWithNoCase(refstr, "te");
  /// fprintf(stderr, "Expect true for here and is '%s'\n", ret ? "true" : "false");
  ///
  /// ret = kodi::tools::StringUtils::StartsWithNoCase(refstr, "TEs");
  /// fprintf(stderr, "Expect true for here and is '%s'\n", ret ? "true" : "false");
  ///
  /// ret = kodi::tools::StringUtils::StartsWithNoCase(refstr, "abc");
  /// fprintf(stderr, "Expect false for here and is '%s'\n", ret ? "true" : "false");
  /// ~~~~~~~~~~~~~
  ///
  inline static bool StartsWithNoCase(const std::string& str1, const std::string& str2)
  {
    return StartsWithNoCase(str1.c_str(), str2.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string by ignore of
  /// upper-/lowercase.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  inline static bool StartsWithNoCase(const std::string& str1, const char* s2)
  {
    return StartsWithNoCase(str1.c_str(), s2);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the begin of another string by ignore of
  /// upper-/lowercase.
  ///
  /// @param[in] s1 C string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the beginning
  /// @return True if string started with asked text, false otherwise
  ///
  inline static bool StartsWithNoCase(const char* s1, const char* s2)
  {
    while (*s2 != '\0')
    {
      if (::tolower(*s1) != ::tolower(*s2))
        return false;
      s1++;
      s2++;
    }
    return true;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the ending of another string.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] str2 C++ string with which text defined in str1 is checked at
  ///                 the ending
  /// @return True if string ended with asked text, false otherwise
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// bool ret;
  /// std::string refstr = "test";
  ///
  /// ret = kodi::tools::StringUtils::EndsWith(refstr, "st");
  /// fprintf(stderr, "Expect true for here and is '%s'\n", ret ? "true" : "false");
  ///
  /// ret = kodi::tools::StringUtils::EndsWith(refstr, "abc");
  /// fprintf(stderr, "Expect false for here and is '%s'\n", ret ? "true" : "false");
  /// ~~~~~~~~~~~~~
  ///
  inline static bool EndsWith(const std::string& str1, const std::string& str2)
  {
    if (str1.size() < str2.size())
      return false;
    return str1.compare(str1.size() - str2.size(), str2.size(), str2) == 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the ending of another string.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the ending
  /// @return True if string ended with asked text, false otherwise
  ///
  inline static bool EndsWith(const std::string& str1, const char* s2)
  {
    size_t len2 = strlen(s2);
    if (str1.size() < len2)
      return false;
    return str1.compare(str1.size() - len2, len2, s2) == 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the ending of another string by ignore of
  /// upper-/lowercase.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] str2 C++ string with which text defined in str1 is checked at
  ///                 the ending
  /// @return True if string ended with asked text, false otherwise
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// bool ret;
  /// std::string refstr = "test";
  ///
  /// ret = kodi::tools::StringUtils::EndsWithNoCase(refstr, "ST");
  /// fprintf(stderr, "Expect true for here and is '%s'\n", ret ? "true" : "false");
  ///
  /// ret = kodi::tools::StringUtils::EndsWithNoCase(refstr, "ABC");
  /// fprintf(stderr, "Expect false for here and is '%s'\n", ret ? "true" : "false");
  /// ~~~~~~~~~~~~~
  ///
  inline static bool EndsWithNoCase(const std::string& str1, const std::string& str2)
  {
    if (str1.size() < str2.size())
      return false;
    const char* s1 = str1.c_str() + str1.size() - str2.size();
    const char* s2 = str2.c_str();
    while (*s2 != '\0')
    {
      if (::tolower(*s1) != ::tolower(*s2))
        return false;
      s1++;
      s2++;
    }
    return true;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a string for the ending of another string by ignore of
  /// upper-/lowercase.
  ///
  /// @param[in] str1 C++ string to be checked
  /// @param[in] s2 C string with which text defined in str1 is checked at
  ///               the ending
  /// @return True if string ended with asked text, false otherwise
  ///
  inline static bool EndsWithNoCase(const std::string& str1, const char* s2)
  {
    size_t len2 = strlen(s2);
    if (str1.size() < len2)
      return false;
    const char* s1 = str1.c_str() + str1.size() - len2;
    while (*s2 != '\0')
    {
      if (::tolower(*s1) != ::tolower(*s2))
        return false;
      s1++;
      s2++;
    }
    return true;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Compare two strings by his calculated alpha numeric values.
  ///
  /// @param[in] left Left string to compare with right
  /// @param[in] right Right string to compare with left
  /// @return Return about compare
  ///         - 0 if left and right the same
  ///         - -1 if right is longer
  ///         - 1 if left is longer
  ///         - < 0 if less equal
  ///         - > 0 if more equal
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// int64_t ref, var;
  ///
  /// ref = 0;
  /// var = kodi::tools::StringUtils::AlphaNumericCompare(L"123abc", L"abc123");
  /// EXPECT_LT(var, ref);
  /// ~~~~~~~~~~~~~
  ///
  inline static int64_t AlphaNumericCompare(const wchar_t* left, const wchar_t* right)
  {
    const wchar_t* l = left;
    const wchar_t* r = right;
    const wchar_t *ld, *rd;
    wchar_t lc, rc;
    int64_t lnum, rnum;
    const std::collate<wchar_t>& coll = std::use_facet<std::collate<wchar_t>>(std::locale());
    int cmp_res = 0;
    while (*l != 0 && *r != 0)
    {
      // check if we have a numerical value
      if (*l >= L'0' && *l <= L'9' && *r >= L'0' && *r <= L'9')
      {
        ld = l;
        lnum = 0;
        while (*ld >= L'0' && *ld <= L'9' && ld < l + 15)
        { // compare only up to 15 digits
          lnum *= 10;
          lnum += *ld++ - '0';
        }
        rd = r;
        rnum = 0;
        while (*rd >= L'0' && *rd <= L'9' && rd < r + 15)
        { // compare only up to 15 digits
          rnum *= 10;
          rnum += *rd++ - L'0';
        }
        // do we have numbers?
        if (lnum != rnum)
        { // yes - and they're different!
          return lnum - rnum;
        }
        l = ld;
        r = rd;
        continue;
      }
      // do case less comparison
      lc = *l;
      if (lc >= L'A' && lc <= L'Z')
        lc += L'a' - L'A';
      rc = *r;
      if (rc >= L'A' && rc <= L'Z')
        rc += L'a' - L'A';

      // ok, do a normal comparison, taking current locale into account. Add special case stuff (eg '(' characters)) in here later
      if ((cmp_res = coll.compare(&lc, &lc + 1, &rc, &rc + 1)) != 0)
      {
        return cmp_res;
      }
      l++;
      r++;
    }
    if (*r)
    { // r is longer
      return -1;
    }
    else if (*l)
    { // l is longer
      return 1;
    }
    return 0; // files are the same
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief UTF8 version of strlen
  ///
  /// Skips any non-starting bytes in the count, thus returning the number of
  /// utf8 characters.
  ///
  /// @param[in] s c-string to find the length of.
  /// @return The number of utf8 characters in the string.
  ///
  inline static size_t Utf8StringLength(const char* s)
  {
    size_t length = 0;
    while (*s)
    {
      if ((*s++ & 0xC0) != 0x80)
        length++;
    }
    return length;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check given character is a space.
  ///
  /// Hack to check only first byte of UTF-8 character
  /// without this hack "TrimX" functions failed on Win32 and OS X with UTF-8 strings
  ///
  /// @param[in] c Character to check
  /// @return true if space, false otherwise
  ///
  inline static int IsSpace(char c) { return (c & 0x80) == 0 && ::isspace(c); }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks given pointer in string is a UTF8 letter.
  ///
  /// @param[in] str Given character values to check, must be minimum array of 2
  /// @return return -1 if not, else return the utf8 char length.
  ///
  inline static int IsUTF8Letter(const unsigned char* str)
  {
    // reference:
    // unicode -> utf8 table: http://www.utf8-chartable.de/
    // latin characters in unicode: http://en.wikipedia.org/wiki/Latin_characters_in_Unicode
    unsigned char ch = str[0];
    if (!ch)
      return -1;
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
      return 1;
    if (!(ch & 0x80))
      return -1;
    unsigned char ch2 = str[1];
    if (!ch2)
      return -1;
    // check latin 1 letter table: http://en.wikipedia.org/wiki/C1_Controls_and_Latin-1_Supplement
    if (ch == 0xC3 && ch2 >= 0x80 && ch2 <= 0xBF && ch2 != 0x97 && ch2 != 0xB7)
      return 2;
    // check latin extended A table: http://en.wikipedia.org/wiki/Latin_Extended-A
    if (ch >= 0xC4 && ch <= 0xC7 && ch2 >= 0x80 && ch2 <= 0xBF)
      return 2;
    // check latin extended B table: http://en.wikipedia.org/wiki/Latin_Extended-B
    // and International Phonetic Alphabet: http://en.wikipedia.org/wiki/IPA_Extensions_(Unicode_block)
    if (((ch == 0xC8 || ch == 0xC9) && ch2 >= 0x80 && ch2 <= 0xBF) ||
        (ch == 0xCA && ch2 >= 0x80 && ch2 <= 0xAF))
      return 2;
    return -1;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check whether a string is a natural number.
  ///
  /// Matches `[ \t]*[0-9]+[ \t]*`
  ///
  /// @param[in] str The string to check
  /// @return true if the string is a natural number, false otherwise.
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsNaturalNumber("10"));
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsNaturalNumber(" 10"));
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsNaturalNumber("0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber(" 1 0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber("1.0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber("1.1"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber("0x1"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber("blah"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber("120 h"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber(" "));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsNaturalNumber(""));
  /// ~~~~~~~~~~~~~
  ///
  inline static bool IsNaturalNumber(const std::string& str)
  {
    size_t i = 0, n = 0;
    // allow whitespace,digits,whitespace
    while (i < str.size() && isspace(static_cast<unsigned char>(str[i])))
      i++;
    while (i < str.size() && isdigit(static_cast<unsigned char>(str[i])))
    {
      i++;
      n++;
    }
    while (i < str.size() && isspace(static_cast<unsigned char>(str[i])))
      i++;
    return i == str.size() && n > 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check whether a string is an integer.
  ///
  /// Matches `[ \t]*[\-]*[0-9]+[ \t]*`
  ///
  /// @param str The string to check
  /// @return true if the string is an integer, false otherwise.
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsInteger("10"));
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsInteger(" -10"));
  /// EXPECT_TRUE(kodi::tools::StringUtils::IsInteger("0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger(" 1 0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger("1.0"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger("1.1"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger("0x1"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger("blah"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger("120 h"));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger(" "));
  /// EXPECT_FALSE(kodi::tools::StringUtils::IsInteger(""));
  /// ~~~~~~~~~~~~~
  ///
  inline static bool IsInteger(const std::string& str)
  {
    size_t i = 0, n = 0;
    // allow whitespace,-,digits,whitespace
    while (i < str.size() && isspace(static_cast<unsigned char>(str[i])))
      i++;
    if (i < str.size() && str[i] == '-')
      i++;
    while (i < str.size() && isdigit(static_cast<unsigned char>(str[i])))
    {
      i++;
      n++;
    }
    while (i < str.size() && isspace(static_cast<unsigned char>(str[i])))
      i++;
    return i == str.size() && n > 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a character is ascii number.
  ///
  /// @param[in] chr Single character to test
  /// @return true if yes, false otherwise
  ///
  inline static bool IsAasciiDigit(char chr) // locale independent
  {
    return chr >= '0' && chr <= '9';
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a character is ascii hexadecimal number.
  ///
  /// @param[in] chr Single character to test
  /// @return true if yes, false otherwise
  ///
  inline static bool IsAsciiXDigit(char chr) // locale independent
  {
    return (chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f') || (chr >= 'A' && chr <= 'F');
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Translate a character where defined as a numerical value (0-9)
  /// string to right integer.
  ///
  /// @param[in] chr Single character to translate
  /// @return
  ///
  inline static int AsciiDigitValue(char chr) // locale independent
  {
    if (!IsAasciiDigit(chr))
      return -1;

    return chr - '0';
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Translate a character where defined as a hexadecimal value string
  /// to right integer.
  ///
  /// @param[in] chr Single character to translate
  /// @return Corresponding integer value, e.g. character is "A" becomes
  ///         returned as a integer with 10.
  ///
  inline static int AsciiXDigitValue(char chr) // locale independent
  {
    int v = AsciiDigitValue(chr);
    if (v >= 0)
      return v;
    if (chr >= 'a' && chr <= 'f')
      return chr - 'a' + 10;
    if (chr >= 'A' && chr <= 'F')
      return chr - 'A' + 10;

    return -1;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a character is ascii alphabetic lowercase.
  ///
  /// @param[in] chr Single character to test
  /// @return True if ascii uppercase letter, false otherwise
  ///
  inline static bool IsAsciiUppercaseLetter(char chr) // locale independent
  {
    return (chr >= 'A' && chr <= 'Z');
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a character is ascii alphabetic lowercase.
  ///
  /// @param[in] chr Single character to test
  /// @return True if ascii lowercase letter, false otherwise
  ///
  inline static bool IsAsciiLowercaseLetter(char chr) // locale independent
  {
    return (chr >= 'a' && chr <= 'z');
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Checks a character is within ascii alphabetic and numerical fields.
  ///
  /// @param[in] chr Single character to test
  /// @return true if alphabetic / numerical ascii value
  ///
  inline static bool IsAsciiAlphaNum(char chr) // locale independent
  {
    return IsAsciiUppercaseLetter(chr) || IsAsciiLowercaseLetter(chr) || IsAasciiDigit(chr);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check a string for another text.
  ///
  /// @param[in] str String to search for keywords
  /// @param[in] keywords List of keywords to search in text
  /// @return true if string contains word in list
  ///
  inline static bool ContainsKeyword(const std::string& str,
                                     const std::vector<std::string>& keywords)
  {
    for (const auto& it : keywords)
    {
      if (str.find(it) != str.npos)
        return true;
    }
    return false;
  }
  //----------------------------------------------------------------------------

  /*!@}*/

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_SearchControl String search
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **To search a string**\n
  /// Various functions are defined in here which allow you to search through a
  /// text in different ways.
  ///
  /*!@{*/

  //============================================================================
  /// @brief Search for a single word within a text.
  ///
  /// @param[in] str String to search within
  /// @param[in] wordLowerCase Word as lowercase to search
  /// @return Position in string where word is found, -1 (std::string::npos) if
  ///         not found
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// size_t ref, var;
  ///
  /// // The name "string" is alone within text and becomes found on position 5
  /// ref = 5;
  /// var = kodi::tools::StringUtils::FindWords("test string", "string");
  /// EXPECT_EQ(ref, var);
  ///
  /// // The 12 is included inside another word and then not found as it should alone (-1 return)
  /// ref = -1;
  /// var = kodi::tools::StringUtils::FindWords("apple2012", "12");
  /// EXPECT_EQ(ref, var);
  /// ~~~~~~~~~~~~~
  ///
  inline static size_t FindWords(const char* str, const char* wordLowerCase)
  {
    // NOTE: This assumes word is lowercase!
    const unsigned char* s = reinterpret_cast<const unsigned char*>(str);
    do
    {
      // start with a compare
      const unsigned char* c = s;
      const unsigned char* w = reinterpret_cast<const unsigned char*>(wordLowerCase);
      bool same = true;
      while (same && *c && *w)
      {
        unsigned char lc = *c++;
        if (lc >= 'A' && lc <= 'Z')
          lc += 'a' - 'A';

        if (lc != *w++) // different
          same = false;
      }
      if (same && *w == 0) // only the same if word has been exhausted
        return reinterpret_cast<const char*>(s) - str;

      // otherwise, skip current word (composed by latin letters) or number
      int l;
      if (*s >= '0' && *s <= '9')
      {
        ++s;
        while (*s >= '0' && *s <= '9')
          ++s;
      }
      else if ((l = IsUTF8Letter(s)) > 0)
      {
        s += l;
        while ((l = IsUTF8Letter(s)) > 0)
          s += l;
      }
      else
        ++s;
      while (*s && *s == ' ')
        s++;

      // and repeat until we're done
    } while (*s);

    return std::string::npos;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Search a string for a given bracket and give its end position.
  ///
  /// @param[in] str String to search within
  /// @param[in] opener Begin character to start search
  /// @param[in] closer End character to end search
  /// @param[in] startPos [opt] Position to start search in string, 0 as default
  ///                     to start from begin
  /// @return End position where found, -1 if failed
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// int ref, var;
  ///
  /// ref = 11;
  /// var = kodi::tools::StringUtils::FindEndBracket("atest testbb test", 'a', 'b');
  /// EXPECT_EQ(ref, var);
  /// ~~~~~~~~~~~~~
  ///
  inline static size_t FindEndBracket(const std::string& str,
                                      char opener,
                                      char closer,
                                      size_t startPos = 0)
  {
    size_t blocks = 1;
    for (size_t i = startPos; i < str.size(); i++)
    {
      if (str[i] == opener)
        blocks++;
      else if (str[i] == closer)
      {
        blocks--;
        if (!blocks)
          return i;
      }
    }

    return std::string::npos;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Search a text and return the number of parts found as a number.
  ///
  /// @param[in] strInput Input string to search for
  /// @param[in] strFind String to search in input
  /// @return Amount how much the string is found
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_EQ(3, kodi::tools::StringUtils::FindNumber("aabcaadeaa", "aa"));
  /// EXPECT_EQ(1, kodi::tools::StringUtils::FindNumber("aabcaadeaa", "b"));
  /// ~~~~~~~~~~~~~
  ///
  inline static int FindNumber(const std::string& strInput, const std::string& strFind)
  {
    size_t pos = strInput.find(strFind, 0);
    int numfound = 0;
    while (pos != std::string::npos)
    {
      numfound++;
      pos = strInput.find(strFind, pos + 1);
    }
    return numfound;
  }
  //----------------------------------------------------------------------------

  /*!@}*/

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_ListControl String list
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **Creating lists using a string**\n
  /// With this, either simple vectors or lists defined by templates can be given
  /// for the respective divided text.
  ///
  /*!@{*/

  //============================================================================
  /// @brief Concatenates the elements of a specified array or the members of a
  /// collection and uses the specified separator between each element or
  /// member.
  ///
  /// @param[in] strings An array of objects whose string representations are
  ///                    concatenated.
  /// @param[in] delimiter Delimiter to be used to join the input string
  /// @return A string consisting of the elements of values, separated by the
  /// separator character.
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string refstr, varstr;
  /// std::vector<std::string> strarray;
  ///
  /// strarray.emplace_back("a");
  /// strarray.emplace_back("b");
  /// strarray.emplace_back("c");
  /// strarray.emplace_back("de");
  /// strarray.emplace_back(",");
  /// strarray.emplace_back("fg");
  /// strarray.emplace_back(",");
  /// refstr = "a,b,c,de,,,fg,,";
  /// varstr = kodi::tools::StringUtils::Join(strarray, ",");
  /// EXPECT_STREQ(refstr.c_str(), varstr.c_str());
  /// ~~~~~~~~~~~~~
  ///
  template<typename CONTAINER>
  inline static std::string Join(const CONTAINER& strings, const std::string& delimiter)
  {
    std::string result;
    for (const auto& str : strings)
      result += str + delimiter;

    if (!result.empty())
      result.erase(result.size() - delimiter.size());
    return result;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty the result will be an empty array (not
  /// an array containing an empty string).
  ///
  /// @param[in] input Input string to be split
  /// @param[in] delimiter Delimiter to be used to split the input string
  /// @param[in] iMaxStrings [opt] Maximum number of resulting split strings
  /// @return List of splitted strings
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::vector<std::string> varresults;
  ///
  /// varresults = kodi::tools::StringUtils::Split("g,h,ij,k,lm,,n", ",");
  /// EXPECT_STREQ("g", varresults.at(0).c_str());
  /// EXPECT_STREQ("h", varresults.at(1).c_str());
  /// EXPECT_STREQ("ij", varresults.at(2).c_str());
  /// EXPECT_STREQ("k", varresults.at(3).c_str());
  /// EXPECT_STREQ("lm", varresults.at(4).c_str());
  /// EXPECT_STREQ("", varresults.at(5).c_str());
  /// EXPECT_STREQ("n", varresults.at(6).c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::vector<std::string> Split(const std::string& input,
                                               const std::string& delimiter,
                                               unsigned int iMaxStrings = 0)
  {
    std::vector<std::string> result;
    SplitTo(std::back_inserter(result), input, delimiter, iMaxStrings);
    return result;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty the result will be an empty array (not
  /// an array containing an empty string).
  ///
  /// @param[in] input Input string to be split
  /// @param[in] delimiter Delimiter to be used to split the input string
  /// @param[in] iMaxStrings [opt] Maximum number of resulting split strings
  /// @return List of splitted strings
  ///
  inline static std::vector<std::string> Split(const std::string& input,
                                               const char delimiter,
                                               int iMaxStrings = 0)
  {
    std::vector<std::string> result;
    SplitTo(std::back_inserter(result), input, delimiter, iMaxStrings);
    return result;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty the result will be an empty array (not
  /// an array containing an empty string).
  ///
  /// @param[in] input Input string to be split
  /// @param[in] delimiters Delimiter strings to be used to split the input
  ///                       strings
  /// @return List of splitted strings
  ///
  inline static std::vector<std::string> Split(const std::string& input,
                                               const std::vector<std::string>& delimiters)
  {
    std::vector<std::string> result;
    SplitTo(std::back_inserter(result), input, delimiters);
    return result;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty nothing will be put into the target
  /// iterator.
  ///
  /// @param[in] d_first The beginning of the destination range
  /// @param[in] input Input string to be split
  /// @param[in] delimiter Delimiter to be used to split the input string
  /// @param[in] iMaxStrings [opt] Maximum number of resulting split strings
  /// @return Output iterator to the element in the destination range, one past
  ///         the last element that was put there
  ///
  template<typename OutputIt>
  inline static OutputIt SplitTo(OutputIt d_first,
                                 const std::string& input,
                                 const std::string& delimiter,
                                 unsigned int iMaxStrings = 0)
  {
    OutputIt dest = d_first;

    if (input.empty())
      return dest;
    if (delimiter.empty())
    {
      *d_first++ = input;
      return dest;
    }

    const size_t delimLen = delimiter.length();
    size_t nextDelim;
    size_t textPos = 0;
    do
    {
      if (--iMaxStrings == 0)
      {
        *dest++ = input.substr(textPos);
        break;
      }
      nextDelim = input.find(delimiter, textPos);
      *dest++ = input.substr(textPos, nextDelim - textPos);
      textPos = nextDelim + delimLen;
    } while (nextDelim != std::string::npos);

    return dest;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty nothing will be put into the target
  /// iterator.
  ///
  /// @param[in] d_first The beginning of the destination range
  /// @param[in] input Input string to be split
  /// @param[in] delimiter Delimiter to be used to split the input string
  /// @param[in] iMaxStrings [opt] Maximum number of resulting split strings
  /// @return Output iterator to the element in the destination range, one past
  ///         the last element that was put there
  ///
  template<typename OutputIt>
  inline static OutputIt SplitTo(OutputIt d_first,
                                 const std::string& input,
                                 const char delimiter,
                                 int iMaxStrings = 0)
  {
    return SplitTo(d_first, input, std::string(1, delimiter), iMaxStrings);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input string using the given delimiter into
  /// separate strings.
  ///
  /// If the given input string is empty nothing will be put into the target
  /// iterator.
  ///
  /// @param[in] d_first The beginning of the destination range
  /// @param[in] input Input string to be split
  /// @param[in] delimiters Delimiter strings to be used to split the input
  ///                       strings
  /// @return Output iterator to the element in the destination range, one past
  ///         the last element that was put there
  ///
  template<typename OutputIt>
  inline static OutputIt SplitTo(OutputIt d_first,
                                 const std::string& input,
                                 const std::vector<std::string>& delimiters)
  {
    OutputIt dest = d_first;
    if (input.empty())
      return dest;

    if (delimiters.empty())
    {
      *dest++ = input;
      return dest;
    }
    std::string str = input;
    for (size_t di = 1; di < delimiters.size(); di++)
      StringUtils::Replace(str, delimiters[di], delimiters[0]);
    return SplitTo(dest, str, delimiters[0]);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Splits the given input strings using the given delimiters into
  /// further separate strings.
  ///
  /// If the given input string vector is empty the result will be an empty
  /// array (not an array containing an empty string).
  ///
  /// Delimiter strings are applied in order, so once the (optional) maximum
  /// number of items is produced no other delimiters are applied. This produces
  /// different results to applying all delimiters at once e.g. "a/b#c/d"
  /// becomes "a", "b#c", "d" rather than "a", "b", "c/d"
  ///
  /// @param[in] input Input vector of strings each to be split
  /// @param[in] delimiters Delimiter strings to be used to split the input
  ///                       strings
  /// @param[in] iMaxStrings [opt] Maximum number of resulting split strings
  /// @return List of splitted strings
  ///
  inline static std::vector<std::string> SplitMulti(const std::vector<std::string>& input,
                                                    const std::vector<std::string>& delimiters,
                                                    unsigned int iMaxStrings = 0)
  {
    if (input.empty())
      return std::vector<std::string>();

    std::vector<std::string> results(input);

    if (delimiters.empty() || (iMaxStrings > 0 && iMaxStrings <= input.size()))
      return results;

    std::vector<std::string> strings1;
    if (iMaxStrings == 0)
    {
      for (size_t di = 0; di < delimiters.size(); di++)
      {
        for (size_t i = 0; i < results.size(); i++)
        {
          std::vector<std::string> substrings = StringUtils::Split(results[i], delimiters[di]);
          for (size_t j = 0; j < substrings.size(); j++)
            strings1.push_back(substrings[j]);
        }
        results = strings1;
        strings1.clear();
      }
      return results;
    }

    // Control the number of strings input is split into, keeping the original strings.
    // Note iMaxStrings > input.size()
    size_t iNew = iMaxStrings - results.size();
    for (size_t di = 0; di < delimiters.size(); di++)
    {
      for (size_t i = 0; i < results.size(); i++)
      {
        if (iNew > 0)
        {
          std::vector<std::string> substrings =
              StringUtils::Split(results[i], delimiters[di], static_cast<int>(iNew + 1));
          iNew = iNew - substrings.size() + 1;
          for (size_t j = 0; j < substrings.size(); j++)
            strings1.push_back(substrings[j]);
        }
        else
          strings1.push_back(results[i]);
      }
      results = strings1;
      iNew = iMaxStrings - results.size();
      strings1.clear();
      if ((iNew <= 0))
        break; //Stop trying any more delimiters
    }
    return results;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Split a string by the specified delimiters.
  ///
  /// Splits a string using one or more delimiting characters, ignoring empty
  /// tokens.
  ///
  /// Differs from Split() in two ways:
  /// 1. The delimiters are treated as individual characters, rather than a single delimiting string.
  /// 2. Empty tokens are ignored.
  ///
  ///
  /// @param[in] input String to split
  /// @param[in] delimiters Delimiters
  /// @return A vector of tokens
  ///
  inline static std::vector<std::string> Tokenize(const std::string& input,
                                                  const std::string& delimiters)
  {
    std::vector<std::string> tokens;
    Tokenize(input, tokens, delimiters);
    return tokens;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Tokenizing a string denotes splitting a string with respect to a
  /// delimiter.
  ///
  /// @param[in] input String to split
  /// @param[out] tokens A vector of tokens
  /// @param[in] delimiters Delimiters
  ///
  inline static void Tokenize(const std::string& input,
                              std::vector<std::string>& tokens,
                              const std::string& delimiters)
  {
    tokens.clear();
    // Skip delimiters at beginning.
    std::string::size_type dataPos = input.find_first_not_of(delimiters);
    while (dataPos != std::string::npos)
    {
      // Find next delimiter
      const std::string::size_type nextDelimPos = input.find_first_of(delimiters, dataPos);
      // Found a token, add it to the vector.
      tokens.push_back(input.substr(dataPos, nextDelimPos - dataPos));
      // Skip delimiters.  Note the "not_of"
      dataPos = input.find_first_not_of(delimiters, nextDelimPos);
    }
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Tokenizing a string denotes splitting a string with respect to a
  /// delimiter.
  ///
  /// @param[in] input String to split
  /// @param[in] delimiter Delimiters
  /// @return A vector of tokens
  ///
  inline static std::vector<std::string> Tokenize(const std::string& input, const char delimiter)
  {
    std::vector<std::string> tokens;
    Tokenize(input, tokens, delimiter);
    return tokens;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Tokenizing a string denotes splitting a string with respect to a
  /// delimiter.
  ///
  /// @param[in] input String to split
  /// @param[out] tokens List of
  /// @param[in] delimiter Delimiters
  ///
  inline static void Tokenize(const std::string& input,
                              std::vector<std::string>& tokens,
                              const char delimiter)
  {
    tokens.clear();
    // Skip delimiters at beginning.
    std::string::size_type dataPos = input.find_first_not_of(delimiter);
    while (dataPos != std::string::npos)
    {
      // Find next delimiter
      const std::string::size_type nextDelimPos = input.find(delimiter, dataPos);
      // Found a token, add it to the vector.
      tokens.push_back(input.substr(dataPos, nextDelimPos - dataPos));
      // Skip delimiters.  Note the "not_of"
      dataPos = input.find_first_not_of(delimiter, nextDelimPos);
    }
  }
  //----------------------------------------------------------------------------

  /*!@}*/

  //----------------------------------------------------------------------------
  /// @defgroup cpp_kodi_tools_StringUtils_TimeControl Time value processing
  /// @ingroup cpp_kodi_tools_StringUtils
  /// @brief **String time formats**\n
  /// This is used to process the respective time formats in text fields.
  /*!@{*/

  //============================================================================
  /// @brief Converts a time string to the respective integer value.
  ///
  /// @param[in] timeString String with time.\n
  ///                       Following types are possible:
  ///                       - "MM min" (integer number with "min" on end)
  ///                       - "HH:MM:SS"
  /// @return Time in seconds
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// EXPECT_EQ(77455, kodi::tools::StringUtils::TimeStringToSeconds("21:30:55"));
  /// EXPECT_EQ(7*60, kodi::tools::StringUtils::TimeStringToSeconds("7 min"));
  /// EXPECT_EQ(7*60, kodi::tools::StringUtils::TimeStringToSeconds("7 min\t"));
  /// EXPECT_EQ(154*60, kodi::tools::StringUtils::TimeStringToSeconds("   154 min"));
  /// EXPECT_EQ(1*60+1, kodi::tools::StringUtils::TimeStringToSeconds("1:01"));
  /// EXPECT_EQ(4*60+3, kodi::tools::StringUtils::TimeStringToSeconds("4:03"));
  /// EXPECT_EQ(2*3600+4*60+3, kodi::tools::StringUtils::TimeStringToSeconds("2:04:03"));
  /// EXPECT_EQ(2*3600+4*60+3, kodi::tools::StringUtils::TimeStringToSeconds("   2:4:3"));
  /// EXPECT_EQ(2*3600+4*60+3, kodi::tools::StringUtils::TimeStringToSeconds("  \t\t 02:04:03 \n "));
  /// EXPECT_EQ(1*3600+5*60+2, kodi::tools::StringUtils::TimeStringToSeconds("01:05:02:04:03 \n "));
  /// EXPECT_EQ(0, kodi::tools::StringUtils::TimeStringToSeconds("blah"));
  /// EXPECT_EQ(0, kodi::tools::StringUtils::TimeStringToSeconds("ля-ля"));
  /// ~~~~~~~~~~~~~
  ///
  inline static long TimeStringToSeconds(const std::string& timeString)
  {
    std::string strCopy(timeString);
    StringUtils::Trim(strCopy);
    if (StringUtils::EndsWithNoCase(strCopy, " min"))
    {
      // this is imdb format of "XXX min"
      return 60 * atoi(strCopy.c_str());
    }
    else
    {
      std::vector<std::string> secs = StringUtils::Split(strCopy, ':');
      int timeInSecs = 0;
      for (unsigned int i = 0; i < 3 && i < secs.size(); i++)
      {
        timeInSecs *= 60;
        timeInSecs += atoi(secs[i].c_str());
      }
      return timeInSecs;
    }
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Convert a time in seconds to a string based on the given time
  /// format.
  ///
  /// @param[in] seconds time in seconds
  /// @param[in] format [opt] The format we want the time in
  /// @return The formatted time
  ///
  /// @sa TIME_FORMAT
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// std::string ref, var;
  ///
  /// ref = "21:30:55";
  /// var = kodi::tools::StringUtils::SecondsToTimeString(77455);
  /// EXPECT_STREQ(ref.c_str(), var.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline static std::string SecondsToTimeString(long seconds,
                                                TIME_FORMAT format = TIME_FORMAT_GUESS)
  {
    const bool isNegative = seconds < 0;
    seconds = std::abs(seconds);

    std::string strHMS;
    if (format == TIME_FORMAT_SECS)
      strHMS = std::to_string(seconds);
    else if (format == TIME_FORMAT_MINS)
      strHMS = std::to_string(std::lrintf(static_cast<float>(seconds) / 60.0f));
    else if (format == TIME_FORMAT_HOURS)
      strHMS = std::to_string(std::lrintf(static_cast<float>(seconds) / 3600.0f));
    else if (format & TIME_FORMAT_M)
      strHMS += std::to_string(seconds % 3600 / 60);
    else
    {
      const long hh = seconds / 3600;
      seconds = seconds % 3600;
      const long mm = seconds / 60;
      unsigned int ss = seconds % 60;

      if (format == TIME_FORMAT_GUESS)
        format = (hh >= 1) ? TIME_FORMAT_HH_MM_SS : TIME_FORMAT_MM_SS;
      if (format & TIME_FORMAT_HH)
        strHMS += StringUtils::Format("{:02}", hh);
      else if (format & TIME_FORMAT_H)
        strHMS += std::to_string(hh);
      if (format & TIME_FORMAT_MM)
        strHMS += StringUtils::Format(strHMS.empty() ? "{:02}" : ":{:02}", mm);
      if (format & TIME_FORMAT_SS)
        strHMS += StringUtils::Format(strHMS.empty() ? "{:02}" : ":{:02}", ss);
    }

    if (isNegative)
      strHMS = "-" + strHMS;

    return strHMS;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Converts a string in the format YYYYMMDD to the corresponding
  /// integer value.
  ///
  /// @param[in] dateString The date in the associated format, possible values
  ///                       are:
  ///                       - DD (for days only)
  ///                       - MM-DD (for days with month)
  ///                       - YYYY-MM-DD (for years, then month and last days)
  /// @return Corresponding integer, e.g. "2020-12-24" return as integer value
  ///         20201224
  ///
  ///
  /// --------------------------------------------------------------------------
  /// Example:
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/tools/StringUtils.h>
  ///
  /// int ref, var;
  ///
  /// ref = 20120706;
  /// var = kodi::tools::StringUtils::DateStringToYYYYMMDD("2012-07-06");
  /// EXPECT_EQ(ref, var);
  /// ~~~~~~~~~~~~~
  ///
  inline static int DateStringToYYYYMMDD(const std::string& dateString)
  {
    std::vector<std::string> days = StringUtils::Split(dateString, '-');
    if (days.size() == 1)
      return atoi(days[0].c_str());
    else if (days.size() == 2)
      return atoi(days[0].c_str()) * 100 + atoi(days[1].c_str());
    else if (days.size() == 3)
      return atoi(days[0].c_str()) * 10000 + atoi(days[1].c_str()) * 100 + atoi(days[2].c_str());
    else
      return -1;
  }
  //----------------------------------------------------------------------------

  /*!@}*/

private:
  inline static int compareWchar(const void* a, const void* b)
  {
    if (*static_cast<const wchar_t*>(a) < *static_cast<const wchar_t*>(b))
      return -1;
    else if (*static_cast<const wchar_t*>(a) > *static_cast<const wchar_t*>(b))
      return 1;
    return 0;
  }

  inline static wchar_t tolowerUnicode(const wchar_t& c)
  {
    wchar_t* p =
        static_cast<wchar_t*>(bsearch(&c, unicode_uppers, sizeof(unicode_uppers) / sizeof(wchar_t),
                                      sizeof(wchar_t), compareWchar));
    if (p)
      return *(unicode_lowers + (p - unicode_uppers));

    return c;
  }

  inline static wchar_t toupperUnicode(const wchar_t& c)
  {
    wchar_t* p =
        static_cast<wchar_t*>(bsearch(&c, unicode_lowers, sizeof(unicode_lowers) / sizeof(wchar_t),
                                      sizeof(wchar_t), compareWchar));
    if (p)
      return *(unicode_uppers + (p - unicode_lowers));

    return c;
  }

  static uint32_t UTF8ToUnicode(const unsigned char* z, int nKey, unsigned char& bytes)
  {
    // Lookup table used decode the first byte of a multi-byte UTF8 character
    // clang-format off
    static const unsigned char utf8Trans1[] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
    };
    // clang-format on

    uint32_t c;
    bytes = 0;
    c = z[0];
    if (c >= 0xc0)
    {
      c = utf8Trans1[c - 0xc0];
      int index = 1;
      while (index < nKey && (z[index] & 0xc0) == 0x80)
      {
        c = (c << 6) + (0x3f & z[index]);
        index++;
      }
      if (c < 0x80 || (c & 0xFFFFF800) == 0xD800 || (c & 0xFFFFFFFE) == 0xFFFE)
        c = 0xFFFD;
      bytes = static_cast<unsigned char>(index - 1);
    }
    return c;
  }
};
///@}
//------------------------------------------------------------------------------

} /* namespace tools */
} /* namespace kodi */

#endif /* __cplusplus */
