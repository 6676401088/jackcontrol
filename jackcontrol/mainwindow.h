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
#include <QMainWindow>
#include <QCloseEvent>
#include <QTimerEvent>

// QtJack includes
#include "client.h"

namespace Ui {
    class MainWindow;
}

/**
 * @brief The application's main window.
 * @author Jacob Dawid <jacob@omg-it.works>
 */
class MainWindow :
    public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    // JACK toolbar
    void on_actionStartJackServer_triggered();
    void on_actionStopJackServer_triggered();

    // Transport toolbar
    void on_actionTransportRewind_triggered();
    void on_actionTransportBackward_triggered();
    void on_actionTransportStart_triggered();
    void on_actionTransportStop_triggered();
    void on_actionTransportForward_triggered();

    // Jack Control toolbar
    void on_actionAlwaysOnTop_triggered();

    // Messages tab
    void on_clearMessagesPushButton_clicked();
    void on_exportLogPushButton_clicked();

    void appendMessage(QString appendMessage);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

    Ui::MainWindow *_ui;

private slots:
    void jackServerHasStarted();
    void jackServerHasStopped();

private:
    QtJack::Client _client;
};
