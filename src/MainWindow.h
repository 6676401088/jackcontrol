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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Own includes
#include "Setup.h"
#include "MainWidget.h"

// Qt includes
#include <QMainWindow>
#include <QSocketNotifier>

// QJack includes
#include <Server>
#include <Client>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags windowFlags = 0);
    ~MainWindow();

    bool setup(Setup *pSetup);

public slots:
    void on_actionAbout_triggered();

    // JACK toolbar
    void on_actionStartJackServer_triggered();
    void on_actionStopJackServer_triggered();
    void on_actionSetup_triggered();

    // Transport toolbar
    void on_actionTransportRewind_triggered();
    void on_actionTransportBackward_triggered();
    void on_actionTransportStart_triggered();
    void on_actionTransportStop_triggered();
    void on_actionTransportForward_triggered();

    void handleConnectedToServer();
    void handleDisconnectedFromServer();
    void handleError(QString errorMessage);

private slots:
    void stdOutActivated(int fileDescriptor);

private:
    void setupStdOutRedirect();
    void setupStatusTab();

    enum MessageType {
        MessageTypeNormal,
        MessageTypeError,
        MessageTypeStdOut
    };

    void message(QString message, MessageType messageType = MessageTypeNormal);

    Ui::MainWindow *ui;
    Setup *_setup;

    QSocketNotifier *_stdOutSocketNotifier;
    QJack::Server *_jackServer;
    QJack::Client *_jackClient;
};

#endif // MAINWINDOW_H
