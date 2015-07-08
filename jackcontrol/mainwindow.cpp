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
#include <QScrollBar>

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

    connect(&JackControl::instance(), SIGNAL(standardOutMessage(QString)),
            this, SLOT(appendMessage(QString)));

    connect(&JackControl::instance(), SIGNAL(activated()),
            this, SLOT(show()));

    connect(&JackControl::instance(), SIGNAL(jackServerHasStarted()),
            this, SLOT(jackServerHasStarted()));
    connect(&JackControl::instance(), SIGNAL(jackServerHasStopped()),
            this, SLOT(jackServerHasStopped()));

    startTimer(200);
}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::on_actionStartJackServer_triggered() {
    if(JackControl::instance().startServer()) {
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
    if(JackControl::instance().stopServer()) {
        _ui->actionStopJackServer->setEnabled(false);
        _ui->actionStartJackServer->setEnabled(true);
        _ui->tabConfiguration->setEnabled(true);
        _ui->tabConnections->setEnabled(false);
        _ui->serverStateValueDisplayLabel->setText("STOPPED");
    }
}

void MainWindow::on_actionTransportRewind_triggered() {
    // TODO
}

void MainWindow::on_actionTransportBackward_triggered() {
    // TODO
}

void MainWindow::on_actionTransportStart_triggered() {
    _client.startTransport();
    _ui->actionTransportStop->setChecked(false);
}

void MainWindow::on_actionTransportStop_triggered() {
    _client.stopTransport();
    _ui->actionTransportStart->setChecked(false);
}

void MainWindow::on_actionTransportForward_triggered() {
    // TODO
}

void MainWindow::on_actionAlwaysOnTop_triggered() {
    if(_ui->actionAlwaysOnTop->isChecked()) {
        setWindowFlags(Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(0);
    }
    show();
}

void MainWindow::on_clearMessagesPushButton_clicked() {
    _ui->messagesTextEdit->clear();
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

void MainWindow::appendMessage(QString message) {
    QString timeStamp = QTime::currentTime().toString("[hh:mm:ss.zzz]");
    _ui->messagesTextEdit->append(QString("%1 %2")
                                 .arg(timeStamp)
                                 .arg(message));

    _ui->messagesTextEdit->horizontalScrollBar()->setValue(_ui->messagesTextEdit->horizontalScrollBar()->maximum());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    hide();
    JackControl::instance().showTrayMessage(tr("JACK Control has been minimized to tray."));
}

void MainWindow::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);

    if(_client.isValid()) {
        QtJack::TransportPosition transportPosition = _client.queryTransportPosition();

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

        int cpuLoad = (int)_client.cpuLoad();
        _ui->cpuLoadValueDisplayLabel->setText(QString("%1 %").arg(QString("%1").arg(cpuLoad), 3, QChar('0')));
    } else {
        _ui->cpuLoadValueDisplayLabel->setText("--- %");
        _ui->transportValueDisplayLabel->setText("--:--:----");
    }
}

void MainWindow::jackServerHasStarted() {
    _client.disconnectFromServer(); // Just in case
    _client.connectToServer("JACK Control Status Poll");
    _client.activate();
}

void MainWindow::jackServerHasStopped() {
    _client.disconnectFromServer();
}

