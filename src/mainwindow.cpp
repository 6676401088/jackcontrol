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
#include "aboutdialog.h"

// uic includes
#include "ui_mainwindow.h"

// Qt includes
#include <QDebug>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupStatusTab();
//    ui->connectionsTab->setParent(0);
//    ui->connectionsTab->show();
    ui->actionStartJackServer->setEnabled(true);
    ui->actionStopJackServer->setEnabled(false);

    connect(&JackService::instance(), SIGNAL(message(QString,JackService::MessageType)),
            this, SLOT(message(QString,JackService::MessageType)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionStartJackServer_triggered() {
    JackService::instance().start();
    ui->actionStartJackServer->setEnabled(false);
}

void MainWindow::on_actionStopJackServer_triggered() {
    JackService::instance().stop();
    ui->actionStopJackServer->setEnabled(false);
    ui->actionStartJackServer->setEnabled(true);
}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionTransportRewind_triggered() {

}

void MainWindow::on_actionTransportBackward_triggered() {

}

void MainWindow::on_actionTransportStart_triggered() {
    JackService::instance().client().startTransport();
    ui->actionTransportStop->setChecked(false);
}

void MainWindow::on_actionTransportStop_triggered() {
    JackService::instance().client().stopTransport();
    ui->actionTransportStart->setChecked(false);
}

void MainWindow::on_actionTransportForward_triggered() {

}

void MainWindow::setupStatusTab() {

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
    ui->messagesTextEdit->append(QString("<font color=\"%1\">%2 %3</font>")
                                 .arg(color)
                                 .arg(timeStamp)
                                 .arg(message));
}
