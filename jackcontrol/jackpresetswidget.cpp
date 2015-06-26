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
#include "jackpresetswidget.h"
#include "jackservice.h"

// UIC includes
#include "ui_jackpresetswidget.h"

JackPresetsWidget::JackPresetsWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::JackPresetsWidget) {
    _ui->setupUi(this);

    connect(_ui->audioDriverComboBox, SIGNAL(activated(QString)),
            this, SLOT(audioDriverNameChanged(QString)));
    connect(_ui->operationModeComboBox, SIGNAL(activated(int)),
            this, SLOT(audioOperationModeChanged(int)));

    _ui->inputDeviceComboBox->setOperationModeFilter(Settings::OperationModeCapture);
    _ui->outputDeviceComboBox->setOperationModeFilter(Settings::OperationModePlayback);

    QtJack::DriverMap drivers = JackService::instance().server().availableDrivers();
    foreach(QtJack::Driver driver, drivers) {
        _ui->audioDriverComboBox->addItem(driver.name());
    }
}

void JackPresetsWidget::audioDriverNameChanged(QString driverName) {
    _ui->inputDeviceComboBox->setDriverName(driverName);
    _ui->outputDeviceComboBox->setDriverName(driverName);
}

void JackPresetsWidget::audioOperationModeChanged(int operationMode) {
    Q_UNUSED(operationMode);
    switch(operationMode) {
    case 0:
        _ui->inputDeviceComboBox->setEnabled(true);
        _ui->outputDeviceComboBox->setEnabled(true);
        break;
    case 1:
        _ui->inputDeviceComboBox->setEnabled(true);
        _ui->outputDeviceComboBox->setEnabled(false);
        break;
    case 2:
        _ui->inputDeviceComboBox->setEnabled(false);
        _ui->outputDeviceComboBox->setEnabled(true);
        break;
    }
}

JackPresetsWidget::~JackPresetsWidget() {
    delete _ui;
}
