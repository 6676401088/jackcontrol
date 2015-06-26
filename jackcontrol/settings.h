/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#pragma once

// Qt includes
#include <QSettings>

class Settings {
public:
    enum OperationMode {
        OperationModeCapture,
        OperationModePlayback,
        OperationModeDuplex
    };

    struct JackServerPreset {
        // Device settings
        QString         _audioDriverName;
        QString         _midiDriverName;
        QString         _interfaceName;
        OperationMode   _operationMode;
        QString         _inputDeviceName;
        QString         _outputDeviceName;

        // Audio processing
        bool            _realTimeProcessing;
        int             _samplesPerFrame;
        int             _samplesPerSecond;
        int             _bufferSizeMultiplier;
        int             _maximumNumberOfPorts;

        // Advanced configuration
        bool    softMode;
        bool    monitor;
        bool    shorts;
        bool    noMemoryLock;
        bool    unlockMemory;
        bool    HWMonitor;
        bool    HWMeter;
        bool    ignoreHW;
        int     priority;
        int     wordLength;
        int     wait;
        int     channels;

        int     dither;
        int     timeout;

        int     inputChannels;
        int     outputChannels;
        int     inputLatency;
        int     outputLatency;
        int     startDelay;
        bool    verbose;
        int     maximumNumberOfPorts;
    };

    Settings();
    ~Settings();

private:
    QSettings _settings;
};

