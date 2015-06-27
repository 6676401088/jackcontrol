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
#include <QSettings>

Settings::Settings() {
}

Settings::~Settings() {
}

Settings::JackServerPreset Settings::loadPreset(QString fileName, bool *ok) {
    QSettings settings(fileName, QSettings::IniFormat);
    JackServerPreset preset;
    QString operationMode;
    QStringList keys = settings.allKeys();
    if(keys.contains("version")) {
        switch(settings.value("version").toInt()) {
            case 1:
                if(ok) { (*ok) = true; }

                preset._version = 1;

                // Device settings
                preset._audioDriverName = settings.value("audioDriverName").toString();
                preset._midiDriverName = settings.value("audioDriverName").toString();

                operationMode = settings.value("audioDriverName").toString().toLower();
                if(operationMode == "duplex") {
                    preset._operationMode = Settings::OperationModeDuplex;
                } else
                if(operationMode == "capture") {
                    preset._operationMode = Settings::OperationModeCapture;
                } else
                if(operationMode == "playback") {
                    preset._operationMode = Settings::OperationModePlayback;
                }

                preset._inputDeviceIdentifier = settings.value("audioDriverName").toString();
                preset._outputDeviceIdentifier = settings.value("audioDriverName").toString();

                // Audio processing
                preset._realTimeProcessing = settings.value("audioDriverName").toBool();
                preset._samplesPerFrame = settings.value("audioDriverName").toInt();
                preset._samplesPerSecond = settings.value("audioDriverName").toInt();
                preset._bufferSizeMultiplier = settings.value("audioDriverName").toInt();
                preset._maximumNumberOfPorts = settings.value("audioDriverName").toInt();
                break;
            default:
                if(ok) { (*ok) = false; }
                break;
        }
    } else {
        if(ok) { (*ok) = false; }
    }
    return preset;
}

bool Settings::savePreset(QString fileName, JackServerPreset preset) {
    switch (preset._version) {
    default:
    case 1:
        break;
    }

    return true;
}

