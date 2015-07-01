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
    QStringList keys = settings.allKeys();
    if(keys.contains("version")) {
        jackControlSettings._version = settings.value("version").toInt();
        switch(jackControlSettings._version) {
            case 1:
                success = loadJackControlSettingsVersion1(settings, jackControlSettings);
                if(ok) { (*ok) = success; }
                break;
            default:
                // Deny all unknown versions
                if(ok) { (*ok) = false; }
                break;
        }
    } else {
        if(ok) { (*ok) = false; }
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
    QStringList keys = settings.allKeys();
    if(keys.contains("version")) {
        jackServerPreset._version = settings.value("version").toInt();
        switch(jackServerPreset._version) {
            case 1:
                success = loadJackServerPresetVersion1(
                    settings,
                    jackServerPreset
                );
                if(ok) { (*ok) = success; }
                break;
            default:
                // Deny all unknown versions
                if(ok) { (*ok) = false; }
                break;
        }
    } else {
        if(ok) { (*ok) = false; }
    }
    return jackServerPreset;
}

bool Settings::loadJackServerPresetVersion1(
    QSettings& settings,
    Settings::JackServerPreset& jackServerPreset) {
    QString operationMode;

    // Device settings
    jackServerPreset._audioDriverName         = settings.value("audioDriverName").toString();
    jackServerPreset._midiDriverName          = settings.value("midiDriverName").toString();

    operationMode = settings.value("operationMode").toString().toLower();
    if(operationMode == "duplex") {
        jackServerPreset._operationMode = Settings::OperationModeDuplex;
    } else
    if(operationMode == "capture") {
        jackServerPreset._operationMode = Settings::OperationModeCapture;
    } else
    if(operationMode == "playback") {
        jackServerPreset._operationMode = Settings::OperationModePlayback;
    }

    QString ditherMode = settings.value("ditherMode").toString().toLower();
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

    jackServerPreset._inputDeviceIdentifier     = settings.value("inputDeviceIdentifier").toString();
    jackServerPreset._outputDeviceIdentifier    = settings.value("outputDeviceIdentifier").toString();

    // Audio processing
    jackServerPreset._enableRealtimeProcessing  = settings.value("enableRealtimeProcessing").toBool();
    jackServerPreset._samplesPerFrame           = settings.value("samplesPerFrame").toInt();
    jackServerPreset._samplesPerSecond          = settings.value("samplesPerSecond").toInt();
    jackServerPreset._bufferSizeMultiplier      = settings.value("bufferSizeMultiplier").toInt();
    jackServerPreset._maximumNumberOfPorts      = settings.value("maximumNumberOfPorts").toInt();
    jackServerPreset._enableHardwareMetering    = settings.value("enableHardwareMetering").toBool();
    jackServerPreset._enableHardwareMonitoring  = settings.value("enableHardwareMonitoring").toBool();
    jackServerPreset._provideMonitorPorts       = settings.value("provideMonitorPorts").toBool();
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

    settings.setValue("inputDeviceIdentifier",      jackServerPreset._inputDeviceIdentifier);
    settings.setValue("outputDeviceIdentifier",     jackServerPreset._outputDeviceIdentifier);

    // Audio processing
    settings.setValue("enableRealtimeProcessing",   jackServerPreset._enableRealtimeProcessing);
    settings.setValue("samplesPerFrame",            jackServerPreset._samplesPerFrame);
    settings.setValue("samplesPerSecond",           jackServerPreset._samplesPerSecond);
    settings.setValue("bufferSizeMultiplier",       jackServerPreset._bufferSizeMultiplier);
    settings.setValue("maximumNumberOfPorts",       jackServerPreset._maximumNumberOfPorts);
    settings.setValue("enableHardwareMetering",     jackServerPreset._enableHardwareMetering);
    settings.setValue("enableHardwareMetering",     jackServerPreset._enableHardwareMonitoring);
    settings.setValue("provideMonitorPorts",        jackServerPreset._provideMonitorPorts);
    settings.sync();
    return true;
}

