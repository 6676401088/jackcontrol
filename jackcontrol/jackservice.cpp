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
}

bool JackService::startServer() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();

    QtJack::DriverMap drivers = _jackServer.availableDrivers();
    if(drivers.contains(preset._audioDriverName)) {
        QtJack::Driver driver = drivers[preset._audioDriverName];
        QtJack::ParameterMap driverParameters = driver.parameters();
        driverParameters["rate"].setValue(preset._samplesPerSecond);
        driverParameters["device"].setValue(preset._inputDeviceIdentifier);

        if(!_jackServer.start(driver)) {
            qDebug() << "Could not start JACK server.";
            return false;
        } else {
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
        emit message(achBuffer, MessageTypeStdOut);
    }
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
