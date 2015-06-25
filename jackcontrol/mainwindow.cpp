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
#include "mainwindow.h"

// uic includes
#include "ui_mainwindow.h"

// Qt includes
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _ui->actionStopJackServer->setEnabled(false);
    _ui->actionStartJackServer->setEnabled(true);
    _ui->tabConfiguration->setEnabled(true);
    _ui->tabConnections->setEnabled(false);

    connect(&JackService::instance(), SIGNAL(message(QString,JackService::MessageType)),
            this, SLOT(message(QString,JackService::MessageType)));
}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::on_actionStartJackServer_triggered() {
    JackService::instance().start();
    _ui->actionStopJackServer->setEnabled(true);
    _ui->actionStartJackServer->setEnabled(false);
    _ui->tabConfiguration->setEnabled(false);
    _ui->tabConnections->setEnabled(true);
}

void MainWindow::on_actionStopJackServer_triggered() {
    JackService::instance().stop();
    _ui->actionStopJackServer->setEnabled(false);
    _ui->actionStartJackServer->setEnabled(true);
    _ui->tabConfiguration->setEnabled(true);
    _ui->tabConnections->setEnabled(false);
}

void MainWindow::on_actionTransportRewind_triggered() {

}

void MainWindow::on_actionTransportBackward_triggered() {

}

void MainWindow::on_actionTransportStart_triggered() {
    JackService::instance().client().startTransport();
    _ui->actionTransportStop->setChecked(false);
}

void MainWindow::on_actionTransportStop_triggered() {
    JackService::instance().client().stopTransport();
    _ui->actionTransportStart->setChecked(false);
}

void MainWindow::on_actionTransportForward_triggered() {

}

void MainWindow::message(QString message, JackService::MessageType messageType) {
    QString color;
    QString timeStamp = QTime::currentTime().toString("[hh:mm:ss.zzz]");

    switch (messageType) {
    case JackService::MessageTypeNormal:
        color = "#888888";
        break;
    case JackService::MessageTypeError:
        color = "#ff3333";
        break;
    case JackService::MessageTypeStdOut:
        color = "#229922";
        break;
    }
    message.replace("\n", "<br />");
    _ui->messagesTextEdit->append(QString("<font color=\"%1\">%2 %3</font>")
                                 .arg(color)
                                 .arg(timeStamp)
                                 .arg(message));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(QMessageBox::warning(this, tr("Closing JACK control"),
                            tr("Closing JACK control will also close the JACK server and probably affect running client. Are you really sure you want to quit?"),
                            QMessageBox::Yes,
                            QMessageBox::Cancel) == QMessageBox::Yes) {
        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
}

