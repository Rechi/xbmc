/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(__ppc__) || defined(__powerpc__)
#define PIXEL_ASHIFT 0
#define PIXEL_RSHIFT 8
#define PIXEL_GSHIFT 16
#define PIXEL_BSHIFT 24
#else
#define PIXEL_ASHIFT 24
#define PIXEL_RSHIFT 16
#define PIXEL_GSHIFT 8
#define PIXEL_BSHIFT 0
#endif

#define _fdopen fdopen
#define _vsnprintf vsnprintf

#define __stdcall
#define __cdecl
#define WINAPI      __stdcall
struct CXHandle; // forward declaration
typedef CXHandle* HANDLE;

typedef void* HINSTANCE;
typedef void* HMODULE;

typedef unsigned int  DWORD;
#define INVALID_HANDLE_VALUE     ((HANDLE)~0U)

#define MAXWORD   0xffff

typedef union _LARGE_INTEGER
{
  struct {
    DWORD LowPart;
    int32_t HighPart;
  } u;
  long long QuadPart;
} LARGE_INTEGER;

 typedef union _ULARGE_INTEGER {
  struct {
      DWORD LowPart;
      DWORD HighPart;
  } u;
  unsigned long long QuadPart;
} ULARGE_INTEGER;

// Network
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)
#define closesocket(s)  close(s)
#define ioctlsocket(s, f, v) ioctl(s, f, v)
#define WSAGetLastError() (errno)
#define WSAECONNRESET ECONNRESET

typedef int SOCKET;

// File
#define O_BINARY 0
#define _O_TRUNC O_TRUNC
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY

#if defined(TARGET_DARWIN) || defined(TARGET_FREEBSD)
  #define stat64 stat
  #define __stat64 stat
  #define fstat64 fstat
  typedef int64_t off64_t;
  #if defined(TARGET_FREEBSD)
    #define statfs64 statfs
  #endif
#else
  #define __stat64 stat64
#endif

struct _stati64 {
  dev_t st_dev;
  ino_t st_ino;
  unsigned short st_mode;
  short          st_nlink;
  short          st_uid;
  short          st_gid;
  dev_t st_rdev;
  long long  st_size;
  time_t _st_atime;
  time_t _st_mtime;
  time_t _st_ctime;
};

#define _S_IFREG  S_IFREG
#define _S_IFDIR  S_IFDIR
#define MAX_PATH PATH_MAX
