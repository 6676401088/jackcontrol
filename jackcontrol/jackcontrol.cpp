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

void JackControl::initialize(int& argc, char **argv) {
    _application = new QApplication(argc, argv);
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

int main(int argc, char **argv) {
    JackControl::instance().initialize(argc, argv);
    return JackControl::instance().run();
}
