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

// Qt includes
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QFont>
#include <QFileDialog>
#include <QFile>

// Own includes
#include "mainwindow.h"
#include "messageshighlighter.h"

// uic includes
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui->setupUi(this);

    QFontDatabase::addApplicationFont(":/fonts/digital-dream/DIGITALDREAM.ttf");
    QFont fieldNameFont("Digital Dream", 12, 400, true);
    QFont valueFont("Digital Dream", 18);
    _ui->transportDisplayLabel->setFont(fieldNameFont);
    _ui->transportValueDisplayLabel->setFont(valueFont);
    _ui->cpuLoadDisplayLabel->setFont(fieldNameFont);
    _ui->cpuLoadValueDisplayLabel->setFont(valueFont);
    _ui->serverStateDisplayLabel->setFont(fieldNameFont);
    _ui->serverStateValueDisplayLabel->setFont(valueFont);

    _ui->actionStopJackServer->setEnabled(false);
    _ui->actionStartJackServer->setEnabled(true);
    _ui->tabConfiguration->setEnabled(true);
    _ui->tabConnections->setEnabled(false);

    _ui->serverStateValueDisplayLabel->setText("UNKNOWN");

    new MessagesHighlighter(_ui->messagesTextEdit->document());

    connect(&JackService::instance(), SIGNAL(message(QString)),
            this, SLOT(message(QString)));

    startTimer(200);
}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::on_actionStartJackServer_triggered() {
    if(JackService::instance().startServer()) {
        _ui->actionStopJackServer->setEnabled(true);
        _ui->actionStartJackServer->setEnabled(false);
        _ui->tabConfiguration->setEnabled(false);
        _ui->tabConnections->setEnabled(true);

        _ui->tabWidget->setCurrentWidget(_ui->tabConnections);

        _ui->serverStateValueDisplayLabel->setText("RUNNING");
    } else {
        _ui->tabWidget->setCurrentWidget(_ui->tabMessages);

        _ui->serverStateValueDisplayLabel->setText("ERROR");
    }
}

void MainWindow::on_actionStopJackServer_triggered() {
    if(JackService::instance().stopServer()) {
        _ui->actionStopJackServer->setEnabled(false);
        _ui->actionStartJackServer->setEnabled(true);
        _ui->tabConfiguration->setEnabled(true);
        _ui->tabConnections->setEnabled(false);
        _ui->serverStateValueDisplayLabel->setText("STOPPED");
    }
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

void MainWindow::on_exportLogPushButton_clicked() {
    QString logFileName = QFileDialog::getSaveFileName(
        this,
        tr("Export log file"),
        QDir::homePath(),
        tr("Logfile (*.log)"));

    if(!logFileName.isEmpty()) {
        if(!logFileName.endsWith(".log")) {
            logFileName.append(".log");
        }
        QFile logFile(logFileName);
        logFile.open(QFile::ReadWrite);
        if(logFile.isOpen()) {
            logFile.write(_ui->messagesTextEdit->toPlainText().toUtf8());
            logFile.close();
        } else {
            QMessageBox::critical(
                this,
                tr("Could not write log file"),
                tr("The targetted log file could not be opened for writing. You probably do not have the permissions to write to that file."));
        }
    }
}

void MainWindow::message(QString message) {
    QString timeStamp = QTime::currentTime().toString("[hh:mm:ss.zzz]");
    _ui->messagesTextEdit->append(QString("%1 %2")
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

void MainWindow::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    QtJack::Client& client = JackService::instance().client();
    if(client.isValid()) {
        QtJack::TransportPosition transportPosition = client.queryTransportPosition();

        if(transportPosition.bbtDataValid()) {
            _ui->transportValueDisplayLabel->setText(
                QString("%1:%2:%3")
                    .arg(QString("%1").arg(transportPosition._bbt._bar), 2, QChar('0'))
                    .arg(QString("%1").arg(transportPosition._bbt._beat), 2, QChar('0'))
                    .arg(QString("%1").arg(transportPosition._bbt._tick), 4, QChar('0'))
            );
        } else {
            _ui->transportValueDisplayLabel->setText("--:--:----");
        }

        int cpuLoad = (int)client.cpuLoad();
        _ui->cpuLoadValueDisplayLabel->setText(QString("%1 %").arg(QString("%1").arg(cpuLoad), 3, QChar('0')));
    } else {
        _ui->cpuLoadValueDisplayLabel->setText("--- %");
        _ui->transportValueDisplayLabel->setText("--:--:----");
    }
}
