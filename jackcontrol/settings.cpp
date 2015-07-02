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

// Own includes
#include "settings.h"

// Qt includes
#include <QFileInfo>
#include <QStringList>

Settings::Settings() {
}

Settings::~Settings() {
}

Settings::JackControlSettings Settings::loadJackControlSettings(
    QString fileName,
    bool *ok) {
    QSettings settings(fileName, QSettings::IniFormat);
    JackControlSettings jackControlSettings;

    bool success = false;
    jackControlSettings._version = settings.value("version", 1).toInt();
    switch(jackControlSettings._version) {
        default: // Unknown version, assume our application's version
        case 1:
            success = loadJackControlSettingsVersion1(settings, jackControlSettings);
            if(ok) { (*ok) = success; }
            break;
    }
    return jackControlSettings;
}

bool Settings::loadJackControlSettingsVersion1(
    QSettings& settings,
    Settings::JackControlSettings& jackControlSettings) {
    bool ok = true;


    return ok;
}

bool Settings::saveJackControlSettings(
    QString fileName,
    Settings::JackControlSettings jackControlSettings) {
    QSettings settings(fileName, QSettings::IniFormat);
    switch (jackControlSettings._version) {
    default: // If there is no version number provided, save as version 1
    case 1:
        return saveJackControlSettingsVersion1(settings, jackControlSettings);
        break;
    }

    return false;
}

bool Settings::saveJackControlSettingsVersion1(
    QSettings& settings,
    Settings::JackControlSettings& JackControlSettings) {
    // Settings versioning
    settings.setValue("version",                1);

    settings.sync();
    return true;
}

Settings::JackServerPreset Settings::loadPreset(QString fileName, bool *ok) {
    QSettings settings(fileName, QSettings::IniFormat);
    JackServerPreset jackServerPreset;

    QFileInfo fileInfo(fileName);
    jackServerPreset._presetName = fileInfo.fileName();

    bool success = false; 
    jackServerPreset._version = settings.value("version", 1).toInt();
    switch(jackServerPreset._version) {
        default: // Unknown version, assume our application's version
        case 1:
            success = loadJackServerPresetVersion1(
                settings,
                jackServerPreset
            );
            if(ok) { (*ok) = success; }
            break;
    }

    return jackServerPreset;
}

bool Settings::loadJackServerPresetVersion1(
    QSettings& settings,
    Settings::JackServerPreset& jackServerPreset) {
    QString operationMode;

    // Device settings
    jackServerPreset._audioDriverName         = settings.value("audioDriverName", "alsa").toString();
    jackServerPreset._midiDriverName          = settings.value("midiDriverName", "raw").toString();

    operationMode = settings.value("operationMode", "duplex").toString().toLower();
    if(operationMode == "duplex") {
        jackServerPreset._operationMode = Settings::OperationModeDuplex;
    } else
    if(operationMode == "capture") {
        jackServerPreset._operationMode = Settings::OperationModeCapture;
    } else
    if(operationMode == "playback") {
        jackServerPreset._operationMode = Settings::OperationModePlayback;
    }

    jackServerPreset._inputDeviceIdentifier     = settings.value("inputDeviceIdentifier", "hw:0").toString();
    jackServerPreset._outputDeviceIdentifier    = settings.value("outputDeviceIdentifier", "hw:0").toString();

    QString ditherMode = settings.value("ditherMode", "none").toString().toLower();
    if(ditherMode == "none") {
        jackServerPreset._ditherMode = Settings::DitherModeNone;
    } else
    if(ditherMode == "triangular") {
        jackServerPreset._ditherMode = Settings::DitherModeTriangular;
    } else
    if(ditherMode == "shaped") {
        jackServerPreset._ditherMode = Settings::DitherModeShaped;
    } else
    if(ditherMode == "rectangular") {
        jackServerPreset._ditherMode = Settings::DitherModeRectangular;
    }

    jackServerPreset._realtimePriority          = settings.value("realtimePriority", 10).toInt();

    // Audio processing
    jackServerPreset._enableRealtimeProcessing  = settings.value("enableRealtimeProcessing", true).toBool();
    jackServerPreset._samplesPerFrame           = settings.value("samplesPerFrame", 256).toInt();
    jackServerPreset._samplesPerSecond          = settings.value("samplesPerSecond", 44100).toInt();
    jackServerPreset._bufferSizeMultiplier      = settings.value("bufferSizeMultiplier", 2).toInt();
    jackServerPreset._maximumNumberOfPorts      = settings.value("maximumNumberOfPorts", 512).toInt();
    jackServerPreset._enableHardwareMetering    = settings.value("enableHardwareMetering", false).toBool();
    jackServerPreset._enableHardwareMonitoring  = settings.value("enableHardwareMonitoring", false).toBool();
    jackServerPreset._provideMonitorPorts       = settings.value("provideMonitorPorts", false).toBool();

    // Advanced settings
    jackServerPreset._clientTimeout                     = settings.value("clientTimeout", 500).toInt();
    jackServerPreset._maximumNumberOfAudioChannels             = settings.value("maximumNumberOfAudioChannels", -1).toInt();
    jackServerPreset._maximumNumberOfHardwareInputChannels     = settings.value("maximumNumberOfHardwareInputChannels", -1).toInt();
    jackServerPreset._maximumNumberOfHardwareOutputChannels    = settings.value("maximumNumberOfHardwareOutputChannels", -1).toInt();
    jackServerPreset._externalInputLatency              = settings.value("externalInputLatency", -1).toInt();
    jackServerPreset._externalOutputLatency             = settings.value("externalOutputLatency", -1).toInt();
    jackServerPreset._dummyDriverProcessingDelay        = settings.value("dummyDriverProcessingDelay", 20000).toInt();
    jackServerPreset._wordLength                        = settings.value("wordLength", 32).toInt();

    jackServerPreset._noMemoryLock                  = settings.value("noMemoryLock", false).toBool();
    jackServerPreset._unlockMemory                  = settings.value("unlockMemory", false).toBool();
    jackServerPreset._force16BitWordLength          = settings.value("force16BitWordLength", false).toBool();
    jackServerPreset._ignoreHardwareBufferSize      = settings.value("ignoreHardwareBufferSize", false).toBool();
    jackServerPreset._enableSoftMode                = settings.value("enableSoftMode", false).toBool();
    jackServerPreset._showVerboseMessages           = settings.value("showVerboseMessages", false).toBool();
    return true;
}

bool Settings::savePreset(QString fileName, JackServerPreset jackServerPreset) {
    QSettings settings(fileName, QSettings::IniFormat);
    switch (jackServerPreset._version) {
    default: // If there is no version number provided, save as version 1
    case 1:
        return saveJackServerPresetVersion1(settings, jackServerPreset);
        break;
    }

    return false;
}

bool Settings::saveJackServerPresetVersion1(
    QSettings& settings,
    Settings::JackServerPreset& jackServerPreset) {
    // Settings versioning
    settings.setValue("version",                1);

    // Device settings
    settings.setValue("audioDriverName",        jackServerPreset._audioDriverName);
    settings.setValue("midiDriverName",         jackServerPreset._midiDriverName);

    switch(jackServerPreset._operationMode) {
        case OperationModeDuplex:
            settings.setValue("operationMode", "duplex");
            break;
        case OperationModeCapture:
            settings.setValue("operationMode", "capture");
            break;
        case OperationModePlayback:
            settings.setValue("operationMode", "playback");
            break;
    }

    settings.setValue("inputDeviceIdentifier",      jackServerPreset._inputDeviceIdentifier);
    settings.setValue("outputDeviceIdentifier",     jackServerPreset._outputDeviceIdentifier);

    switch(jackServerPreset._ditherMode) {
        case DitherModeNone:
            settings.setValue("ditherMode", "none");
            break;
        case DitherModeTriangular:
            settings.setValue("ditherMode", "triangular");
            break;
        case DitherModeShaped:
            settings.setValue("ditherMode", "shaped");
            break;
        case DitherModeRectangular:
            settings.setValue("ditherMode", "rectangular");
            break;
    }

    settings.setValue("realtimePriority",               jackServerPreset._realtimePriority);

    // Audio processing
    settings.setValue("enableRealtimeProcessing",       jackServerPreset._enableRealtimeProcessing);
    settings.setValue("samplesPerFrame",                jackServerPreset._samplesPerFrame);
    settings.setValue("samplesPerSecond",               jackServerPreset._samplesPerSecond);
    settings.setValue("bufferSizeMultiplier",           jackServerPreset._bufferSizeMultiplier);
    settings.setValue("maximumNumberOfPorts",           jackServerPreset._maximumNumberOfPorts);
    settings.setValue("enableHardwareMetering",         jackServerPreset._enableHardwareMetering);
    settings.setValue("enableHardwareMonitoring",       jackServerPreset._enableHardwareMonitoring);
    settings.setValue("provideMonitorPorts",            jackServerPreset._provideMonitorPorts);

    // Advanced settings
    settings.setValue("clientTimeout",                  jackServerPreset._clientTimeout);
    settings.setValue("maximumNumberOfAudioChannels",          jackServerPreset._maximumNumberOfAudioChannels);
    settings.setValue("maximumNumberOfHardwareInputChannels",  jackServerPreset._maximumNumberOfHardwareInputChannels);
    settings.setValue("maximumNumberOfHardwareOutputChannels", jackServerPreset._maximumNumberOfHardwareOutputChannels);
    settings.setValue("externalInputLatency",           jackServerPreset._externalInputLatency);
    settings.setValue("externalOutputLatency",          jackServerPreset._externalOutputLatency);
    settings.setValue("dummyDriverProcessingDelay",     jackServerPreset._dummyDriverProcessingDelay);
    settings.setValue("wordLength",                     jackServerPreset._wordLength);

    settings.setValue("noMemoryLock",                   jackServerPreset._noMemoryLock);
    settings.setValue("unlockMemory",                   jackServerPreset._unlockMemory);
    settings.setValue("force16WordLength",              jackServerPreset._force16BitWordLength);
    settings.setValue("ignoreHardwareBufferSize",       jackServerPreset._ignoreHardwareBufferSize);
    settings.setValue("enableSoftMode",                 jackServerPreset._enableSoftMode);
    settings.setValue("showVerboseMessages",            jackServerPreset._showVerboseMessages);
    settings.sync();
    return true;
}

