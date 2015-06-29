/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include "mainwindow.h"
#include "settings.h"

// Qt includes
#include <QDir>

void JackControl::initialize(int& argc, char **argv) {
    _application = new QApplication(argc, argv);
}

int JackControl::run() {
    MainWindow mainWindow;
    mainWindow.show();

    setCurrentPreset(Settings::loadPreset(QDir::home().filePath(".config/jackcontrol/default.preset")));
    int status = _application->exec();

    Settings::savePreset(QDir::home().filePath(".config/jackcontrol/default.preset"), _currentPreset);
    return status;
}

void JackControl::setCurrentPreset(Settings::JackServerPreset jackServerPreset) {
    _currentPreset = jackServerPreset;
    emit currentPresetChanged(_currentPreset);
}

Settings::JackServerPreset JackControl::currentPreset() {
    return _currentPreset;
}

int main(int argc, char **argv) {
    JackControl::instance().initialize(argc, argv);
    return JackControl::instance().run();
}
