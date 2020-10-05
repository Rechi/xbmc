/*
 *  Copyright (C) 2011-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "threads/Event.h"

#include <list>
#include <vector>

#include <CoreAudio/CoreAudio.h>
#include <IOKit/audio/IOAudioTypes.h>


typedef std::vector<AudioStreamID> AudioStreamIdList;
typedef std::vector<AudioStreamRangedDescription> StreamFormatList;

class CCoreAudioStream
{
public:
  CCoreAudioStream();
  virtual ~CCoreAudioStream();

  bool    Open(AudioStreamID streamId);
  void    Close(bool restore = true);

  AudioStreamID GetId() const {return m_StreamId;}
  UInt32 GetDirection() const;
  static UInt32 GetTerminalType(AudioStreamID id);
  UInt32 GetNumLatencyFrames() const;
  bool GetVirtualFormat(AudioStreamBasicDescription* pDesc) const;
  bool GetPhysicalFormat(AudioStreamBasicDescription* pDesc) const;
  bool    SetVirtualFormat(AudioStreamBasicDescription *pDesc);
  bool    SetPhysicalFormat(AudioStreamBasicDescription *pDesc);
  bool GetAvailableVirtualFormats(StreamFormatList* pList) const;
  bool GetAvailablePhysicalFormats(StreamFormatList* pList) const;
  static bool GetAvailableVirtualFormats(AudioStreamID id, StreamFormatList *pList);
  static bool GetAvailablePhysicalFormats(AudioStreamID id, StreamFormatList *pList);
  static bool IsDigitalOutput(AudioStreamID id);
  static bool GetStartingChannelInDevice(AudioStreamID id, UInt32 &startingChannel);

protected:
  static OSStatus HardwareStreamListener(AudioObjectID inObjectID,
    UInt32 inNumberAddresses, const AudioObjectPropertyAddress inAddresses[], void* inClientData);

  CEvent m_virtual_format_event;
  CEvent m_physical_format_event;

  AudioStreamID m_StreamId = 0;
  AudioStreamBasicDescription m_OriginalVirtualFormat;
  AudioStreamBasicDescription m_OriginalPhysicalFormat;
};
