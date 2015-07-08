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

#pragma once

// Qt includes
#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QSocketNotifier>

// QtJack includes
#include "client.h"
#include "server.h"

// Own includes
#include "settings.h"
#include "mainwindow.h"

class JackControl :
    public QObject {
    Q_OBJECT
public:
    static JackControl& instance() {
        static JackControl jackControl;
        return jackControl;
    }

    ~JackControl() {
    }

    void initialize(int& argc, char **argv);
    int run();

    void showTrayMessage(QString standardOutMessage);

    bool startServer();
    bool stopServer();

    QtJack::Server& server();

    QStringList availablePresets();
    Settings::JackServerPreset currentPreset();

public slots:
    void discoverPresets();
    void loadPreset(QString presetName);
    void setCurrentPreset(Settings::JackServerPreset jackServerPreset);

signals:
    void jackServerHasStarted();
    void jackServerHasStopped();

    void currentPresetChanged(Settings::JackServerPreset preset);
    void activated();
    void standardOutMessage(QString standardOutMessage);

private slots:
    void systemTrayMessageClicked();
    void systemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void quitTriggered();
    void showWindowTriggered();

    void stdOutActivated(int fileDescriptor);

private:
    JackControl() :
        QObject() {
    }

    void indicateServerRunning();
    void indicateServerStopped();

    void setupStdOutRedirect();
    bool configureDriverFromPreset(QtJack::Driver& driver, Settings::JackServerPreset preset);

    QApplication *              _application;

    MainWindow *                _mainWindow;
    QSystemTrayIcon *           _systemTrayIcon;

    Settings::JackServerPreset  _currentPreset;
    QStringList                 _availablePresets;

    QSocketNotifier *_stdOutSocketNotifier;
    QtJack::Server _server;
};

