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
#include "jackcontrol.h"
#include "settings.h"

// Qt includes
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

void JackControl::initialize(int& argc, char **argv) {
    _application = new QApplication(argc, argv);
    setupStdOutRedirect();

    _mainWindow = new MainWindow();

    QMenu *contextMenu = new QMenu();
    QAction *showWindowAction = contextMenu->addAction(tr("Show window"));
    contextMenu->addSeparator();
    QAction *quitAction = contextMenu->addAction(tr("Quit"));

    _systemTrayIcon = new QSystemTrayIcon();
    _systemTrayIcon->setContextMenu(contextMenu);
    _systemTrayIcon->setIcon(QIcon(":/images/jacktray.svg"));

    connect(_systemTrayIcon, SIGNAL(messageClicked()),
            this, SLOT(systemTrayMessageClicked()));
    connect(_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(systemTrayActivated(QSystemTrayIcon::ActivationReason)));

    connect(showWindowAction, SIGNAL(triggered()), this, SLOT(showWindowTriggered()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitTriggered()));
}

int JackControl::run() {
    discoverPresets();
    loadPreset("default.preset");

    _mainWindow->show();
    _systemTrayIcon->show();

    // Run application
    int status = _application->exec();

    // Save current preset
    if(_currentPreset._presetName.isEmpty()) {
        _currentPreset._presetName = "default.preset";
    }
    Settings::savePreset(QDir::home().filePath(QString(".config/jackcontrol/presets/%1").arg(_currentPreset._presetName)), _currentPreset);

    // Exit
    return status;
}

void JackControl::showTrayMessage(QString message) {
    _systemTrayIcon->showMessage(tr("JACK Control"), message, QSystemTrayIcon::Information, 500);
}

void JackControl::discoverPresets() {
    QString presetsPath = QDir::home().filePath(".config/jackcontrol/presets");
    QDir presetsDir(presetsPath);

    QFileInfoList fileInfoList = presetsDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        QString fileName = fileInfo.fileName();
        if(fileName.endsWith(".preset")) {
            _availablePresets << fileName;
        }
    }
}

void JackControl::loadPreset(QString presetName) {
    QString presetsPath = QDir::home().filePath(".config/jackcontrol/presets");
    QDir presetsDir(presetsPath);

    // Load current preset
    setCurrentPreset(Settings::loadPreset(presetsDir.filePath(presetName)));
}

void JackControl::setCurrentPreset(Settings::JackServerPreset jackServerPreset) {
    _currentPreset = jackServerPreset;
    emit currentPresetChanged(_currentPreset);
}

void JackControl::indicateServerRunning() {
    _systemTrayIcon->setIcon(QIcon(":/images/jacktray-running.svg"));
}

void JackControl::indicateServerStopped() {
    _systemTrayIcon->setIcon(QIcon(":/images/jacktray-stopped.svg"));
}

void JackControl::systemTrayMessageClicked() {
    emit activated();
}

void JackControl::systemTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    Q_UNUSED(reason);
    emit activated();
}

void JackControl::quitTriggered() {
    // Note: For some reason the message box only works properly when the main
    // windows is shown before.
    _mainWindow->show();
    _mainWindow->raise();
    if(QMessageBox::warning(_mainWindow, tr("Closing JACK control"),
                            tr("Closing JACK control will also close the JACK server and probably affect running client. Are you really sure you want to quit?"),
                            QMessageBox::Yes,
                            QMessageBox::Cancel) == QMessageBox::Yes) {
        _application->quit();
    }
}

void JackControl::showWindowTriggered() {
    _mainWindow->show();
}

QStringList JackControl::availablePresets() {
    return _availablePresets;
}

Settings::JackServerPreset JackControl::currentPreset() {
    return _currentPreset;
}

QtJack::Server& JackControl::server() {
    return _server;
}

bool JackControl::configureDriverFromPreset(
    QtJack::Driver& driver,
    Settings::JackServerPreset preset) {
    QtJack::DriverMap drivers = _server.availableDrivers();
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

bool JackControl::startServer() {
    Settings::JackServerPreset preset = currentPreset();
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
        if(!_server.start(driver)) {
            return false;
        } else {
            showTrayMessage(tr("JACK server has been started."));
            indicateServerRunning();
        }
        emit jackServerHasStarted();
        return true;
    }
    return false;
}

bool JackControl::stopServer() {
    if(!_server.stop()) {
        return false;
    } else {
        showTrayMessage(tr("JACK server has been stopped."));
        indicateServerStopped();
        emit jackServerHasStopped();
        return true;
    }
}

void JackControl::setupStdOutRedirect() {
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

void JackControl::stdOutActivated(int fileDescriptor) {
    char achBuffer[1024];
    int  cchBuffer = ::read(fileDescriptor, achBuffer, sizeof(achBuffer) - 1);
    if (cchBuffer > 0) {
        achBuffer[cchBuffer] = (char) 0;
        emit standardOutMessage(achBuffer);
    }
}

int main(int argc, char **argv) {
    JackControl::instance().initialize(argc, argv);
    return JackControl::instance().run();
}
