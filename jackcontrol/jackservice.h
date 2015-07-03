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
#include <QObject>
#include <QSocketNotifier>

// QtJack includes
#include <System>
#include <Server>
#include <Client>

// Own includes
#include "settings.h"

/**
 * @brief Central JACK service class
 * @author Jacob Dawid <jacob@omg-it.works>
 * Singleton. This class is used to represent this application as a JACK client
 * to the server.
 */
class JackService :
    public QObject {
    Q_OBJECT
public:
    static JackService& instance() {
        static JackService jackService;
        return jackService;
    }

    bool startServer();
    bool stopServer();

    QtJack::Client& client();
    QtJack::Server& server();

signals:
    void message(QString message);

private slots:
    void stdOutActivated(int fileDescriptor);

private:
    void setupStdOutRedirect();

private:
    JackService(QObject *parent = 0);

    bool configureDriverFromPreset(QtJack::Driver& driver, Settings::JackServerPreset preset);

    QSocketNotifier *_stdOutSocketNotifier;
    QtJack::Server _jackServer;
    QtJack::Client _jackClient;
};
