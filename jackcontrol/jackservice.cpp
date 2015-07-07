/****************************************************************************
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
#include "jackservice.h"
#include "jackcontrol.h"

#include <QDebug>

#if !defined(WIN32)
#include <unistd.h>
#endif

JackService::JackService(QObject *parent)
    : QObject(parent) {
    setupStdOutRedirect();

    connect(&_jackClient, SIGNAL(clientRegistered(QString)),
            this, SLOT(clientRegistered(QString)));
    connect(&_jackClient, SIGNAL(clientUnregistered(QString)),
            this, SLOT(clientUnregistered(QString)));
}

bool JackService::configureDriverFromPreset(
    QtJack::Driver& driver,
    Settings::JackServerPreset preset) {
    QtJack::DriverMap drivers = _jackServer.availableDrivers();
    if(drivers.contains(preset._audioDriverName)) {
        driver = drivers[preset._audioDriverName];
        if(preset._audioDriverName.toLower() == "alsa") {
            QtJack::ParameterMap driverParameters = driver.parameters();
            switch(preset._operationMode) {
            default:
            case Settings::OperationModeDuplex:
                driverParameters["duplex"]  .setValue(true);
                driverParameters["capture"] .setValue(preset._inputDeviceIdentifier);
                driverParameters["playback"].setValue(preset._outputDeviceIdentifier);
                break;
            case Settings::OperationModeCapture:
                driverParameters["duplex"]  .setValue(false);
                driverParameters["capture"] .setValue(preset._inputDeviceIdentifier);
                driverParameters["playback"].setValue("none");
                break;
            case Settings::OperationModePlayback:
                driverParameters["duplex"]  .setValue(false);
                driverParameters["capture"] .setValue("none");
                driverParameters["playback"].setValue(preset._outputDeviceIdentifier);
                break;
            }

            // TODO: midi-driver
            // -X, --midi 	legacy (default: none)

            driverParameters["rate"]    .setValue(preset._samplesPerSecond);
            driverParameters["period"]  .setValue(preset._samplesPerFrame);
            driverParameters["nperiods"].setValue(preset._bufferSizeMultiplier);
            driverParameters["hwmon"]   .setValue(preset._enableHardwareMonitoring);
            driverParameters["hwmeter"] .setValue(preset._enableHardwareMetering);
            driverParameters["softmode"].setValue(preset._enableSoftMode);
            driverParameters["monitor"] .setValue(preset._provideMonitorPorts);

            switch (preset._ditherMode) {
            case Settings::DitherModeNone:
                driverParameters["dither"].setValue("n");
                break;
            case Settings::DitherModeRectangular:
                driverParameters["dither"].setValue("r");
                break;
            case Settings::DitherModeShaped:
                driverParameters["dither"].setValue("s");
                break;
            case Settings::DitherModeTriangular:
                driverParameters["dither"].setValue("t");
                break;
            }

            if(preset._maximumNumberOfHardwareInputChannels >= 0) {
                driverParameters["inchannels"].setValue(preset._maximumNumberOfHardwareInputChannels);
            }

            if(preset._maximumNumberOfHardwareOutputChannels >= 0) {
                driverParameters["outchannels"].setValue(preset._maximumNumberOfHardwareOutputChannels);
            }

            driverParameters["shorts"].setValue(preset._force16BitWordLength);

            if(preset._externalInputLatency >= 0) {
                driverParameters["input-latency"].setValue(preset._externalInputLatency);
            }

            if(preset._externalOutputLatency >= 0) {
                driverParameters["output-latency"].setValue(preset._externalOutputLatency);
            }

            return true;
        } else {
            // Unknown driver
        }
    }
    return false;
}

bool JackService::startServer() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    QtJack::ParameterMap serverParameters = server().parameters();
    serverParameters["realtime"].setValue(preset._enableRealtimeProcessing);
    if(preset._enableRealtimeProcessing && preset._realtimePriority >= 0) {
        serverParameters["realtime-priority"].setValue(preset._realtimePriority);
    }
    serverParameters["no-mlock"].setValue(preset._noMemoryLock);
    serverParameters["unlock"].setValue(preset._unlockMemory);
    serverParameters["timeout"].setValue(preset._clientTimeout);
    serverParameters["port-max"].setValue(preset._maximumNumberOfPorts);
    serverParameters["verbose"].setValue(preset._showVerboseMessages);

    QtJack::Driver driver;
    if(configureDriverFromPreset(driver, preset)) {
        if(!_jackServer.start(driver)) {
            qDebug() << "Could not start JACK server.";
            return false;
        } else {
            JackControl::instance().showTrayMessage(tr("JACK server has been started."));
            JackControl::instance().indicateServerRunning();
            qDebug() << "Started JACK server successfully.";
        }
        _jackClient.disconnectFromServer(); // Just in case
        _jackClient.connectToServer("JACK Control");
        _jackClient.activate();
        return true;
    }
    return false;
}

bool JackService::stopServer() {
    _jackClient.disconnectFromServer();

    if(!_jackServer.stop()) {
        qDebug() << "Could not stop JACK server.";
        return false;
    } else {
        JackControl::instance().showTrayMessage(tr("JACK server has been stopped."));
        JackControl::instance().indicateServerStopped();
        qDebug() << "Stopped JACK server successfully.";
        return true;
    }
}

QtJack::Client& JackService::client() {
    return _jackClient;
}

QtJack::Server& JackService::server() {
    return _jackServer;
}

void JackService::stdOutActivated(int fileDescriptor) {
    char achBuffer[1024];
    int  cchBuffer = ::read(fileDescriptor, achBuffer, sizeof(achBuffer) - 1);
    if (cchBuffer > 0) {
        achBuffer[cchBuffer] = (char) 0;
        emit message(achBuffer);
    }
}

void JackService::clientRegistered(QString clientName) {
    JackControl::instance().showTrayMessage(tr("Client \"%1\" has connected.").arg(clientName));
}

void JackService::clientUnregistered(QString clientName) {
    JackControl::instance().showTrayMessage(tr("Client \"%1\" has disconnected.").arg(clientName));
}

void JackService::setupStdOutRedirect() {
#if !defined(WIN32)
    // Redirect our own stdout/stderr.
    int pipeDescriptors[2];
    if(::pipe(pipeDescriptors) == 0) {
        ::dup2(pipeDescriptors[1], STDOUT_FILENO);
        ::dup2(pipeDescriptors[1], STDERR_FILENO);
        _stdOutSocketNotifier = new QSocketNotifier(pipeDescriptors[0], QSocketNotifier::Read, this);
        connect(_stdOutSocketNotifier, SIGNAL(activated(int)), this, SLOT(stdOutActivated(int)));
    }
#endif
}
