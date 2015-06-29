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
#include "jackcontrol.h"

// UIC includes
#include "ui_jackpresetswidget.h"

JackPresetsWidget::JackPresetsWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::JackPresetsWidget) {
    _ui->setupUi(this);

    connect(&JackControl::instance(), SIGNAL(currentPresetChanged(Settings::JackServerPreset)),
            this, SLOT(updateWithPreset(Settings::JackServerPreset)));

    connect(_ui->audioDriverComboBox, SIGNAL(activated(QString)),
            this, SLOT(audioDriverNameChanged(QString)));
    connect(_ui->operationModeComboBox, SIGNAL(activated(int)),
            this, SLOT(audioOperationModeChanged(int)));

    connect(_ui->inputDeviceComboBox, SIGNAL(activated(int)),
            this, SLOT(inputDeviceActivated(int)));
    connect(_ui->inputDeviceComboBox, SIGNAL(activated(int)),
            this, SLOT(outputDeviceActivated(int)));

    connect(_ui->sampleRateComboBox, SIGNAL(activated(QString)),
            this, SLOT(sampleRateActivated(QString)));

    QtJack::DriverMap drivers = JackService::instance().server().availableDrivers();
    foreach(QtJack::Driver driver, drivers) {
        _ui->audioDriverComboBox->addItem(driver.name());
    }

    if(drivers.size() > 0) {
        _ui->inputDeviceComboBox->setDriverName(drivers.first().name());
        _ui->outputDeviceComboBox->setDriverName(drivers.first().name());

        _ui->inputDeviceComboBox->update();
        _ui->outputDeviceComboBox->update();
    }

    _ui->inputDeviceComboBox->setOperationModeFilter(Settings::OperationModeCapture);
    _ui->outputDeviceComboBox->setOperationModeFilter(Settings::OperationModePlayback);

    _ui->operationModeComboBox->setCurrentIndex(0);
}

void JackPresetsWidget::on_importPresetPushButton_clicked() {

}

void JackPresetsWidget::on_exportPresetPushButton_clicked() {

}

void JackPresetsWidget::on_savePresetPushButton_clicked() {

}

void JackPresetsWidget::on_deletePresetPushButton_clicked() {

}

void JackPresetsWidget::on_enableRealtimeProcessingCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._realTimeProcessing = _ui->enableRealtimeProcessingCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_sampleRateComboBox_currentTextChanged(QString text) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._samplesPerSecond = text.toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_samplesPerFrameComboBox_currentTextChanged(QString text) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._samplesPerFrame = text.toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_numberOfBuffersSpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._bufferSizeMultiplier = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_maximumNumberOfPortsComboBox_currentTextChanged(QString text) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._maximumNumberOfPorts = text.toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::updateWithPreset(Settings::JackServerPreset preset, QStringList *errorReport) {

    _ui->presetComboBox->setCurrentText(preset._presetName);
    // Device settings
    int numberOfAvailableAudioDrivers = _ui->audioDriverComboBox->count();
    bool foundAudioDriver = false;
    for(int i = 0; i < numberOfAvailableAudioDrivers; i++) {
        if(_ui->audioDriverComboBox->itemText(i) == preset._audioDriverName) {
            _ui->audioDriverComboBox->setCurrentIndex(i);
            foundAudioDriver = true;
            break;
        }
    }

    if(!foundAudioDriver) {
        if(errorReport) {
            errorReport->append(tr("The audio driver is %1, which is not supported on this system or an invalid driver.").arg(preset._audioDriverName));
        }
    }

    int numberOfAvailableMidiDrivers = _ui->midiDriverComboBox->count();
    bool foundMidiDriver = false;
    for(int i = 0; i < numberOfAvailableMidiDrivers; i++) {
        if(_ui->midiDriverComboBox->itemText(i) == preset._midiDriverName) {
            _ui->midiDriverComboBox->setCurrentIndex(i);
            foundMidiDriver = true;
            break;
        }
    }

    if(!foundMidiDriver) {
        if(errorReport) {
            errorReport->append(tr("The MIDI driver is %1, which is not supported on this system or an invalid driver.").arg(preset._midiDriverName));
        }
    }

    switch(preset._operationMode) {
    case Settings::OperationModeDuplex:
        _ui->operationModeComboBox->setCurrentIndex(0);
        break;
    case Settings::OperationModeCapture:
        _ui->operationModeComboBox->setCurrentIndex(1);
        break;
    case Settings::OperationModePlayback:
        _ui->operationModeComboBox->setCurrentIndex(2);
        break;
    default:
        if(errorReport) {
            errorReport->append(tr("The operation mode is invalid."));
        }
        break;
    }

    int numberOfInputDevices = _ui->inputDeviceComboBox->count();
    bool foundInputDevice = false;
    for(int i = 0; i < numberOfInputDevices; i++) {
        if(_ui->inputDeviceComboBox->itemData(i).toString()
        == preset._inputDeviceIdentifier) {
            _ui->inputDeviceComboBox->setCurrentIndex(i);
            foundInputDevice = true;
            break;
        }
    }

    if(!foundInputDevice) {
        if(errorReport) {
            errorReport->append(tr("The input device %1 could not be found on this system.").arg(preset._inputDeviceIdentifier));
        }
    }

    int numberOfOutputDevices = _ui->outputDeviceComboBox->count();
    bool foundOutputDevice = false;
    for(int i = 0; i < numberOfOutputDevices; i++) {
        if(_ui->outputDeviceComboBox->itemData(i).toString()
        == preset._outputDeviceIdentifier) {
            _ui->outputDeviceComboBox->setCurrentIndex(i);
            foundOutputDevice = true;
            break;
        }
    }

    if(!foundOutputDevice) {
        if(errorReport) {
            errorReport->append(tr("The output device %1 could not be found on this system.").arg(preset._outputDeviceIdentifier));
        }
    }

    _ui->enableRealtimeProcessingCheckBox->setChecked(preset._realTimeProcessing);

    // Audio processing
    if(preset._samplesPerFrame < 1) {
        if(errorReport) {
            errorReport->append(tr("Samples per frame is %1, but must be at least 1.").arg(preset._samplesPerFrame));
        }
    } else {
        if(preset._samplesPerFrame > 4096) {
            if(errorReport) {
                errorReport->append(tr("Samples per frame is %1, but must be 4096 or less.").arg(preset._samplesPerFrame));
            }
        } else {
            _ui->samplesPerFrameComboBox->setCurrentText(QString("%1").arg(preset._samplesPerFrame));
        }
    }

    int numberOfSampleRates = _ui->sampleRateComboBox->count();
    bool sampleRateFound = false;
    for(int i = 0; i < numberOfSampleRates; i++) {
        if(_ui->sampleRateComboBox->itemText(i).toInt() == preset._samplesPerSecond) {
            _ui->sampleRateComboBox->setCurrentIndex(i);
            sampleRateFound = true;
            break;
        }
    }

    if(!sampleRateFound) {
        if(errorReport) {
            errorReport->append(tr("A sample rate of %1 is not supported.").arg(preset._samplesPerSecond));
        }
    }

    if(preset._bufferSizeMultiplier < 2) {
        if(errorReport) {
            errorReport->append(tr("Number of buffers is %1, but must be 2 or greater.").arg(preset._bufferSizeMultiplier));
        }
    } else {
        _ui->numberOfBuffersSpinBox->setValue(preset._bufferSizeMultiplier);
    }

    if(preset._maximumNumberOfPorts < 1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of ports is %1, but must be 1 or greater.").arg(preset._maximumNumberOfPorts));
        }
    } else {
        _ui->maximumNumberOfPortsComboBox->setCurrentText(QString("%1").arg(preset._maximumNumberOfPorts));
    }

    // Advanced configuration
//    bool    softMode;
//    bool    monitor;
//    bool    shorts;
//    bool    noMemoryLock;
//    bool    unlockMemory;
//    bool    HWMonitor;
//    bool    HWMeter;
//    bool    ignoreHW;
//    int     priority;
//    int     wordLength;
//    int     wait;
//    int     channels;

//    int     dither;
//    int     timeout;

//    int     inputChannels;
//    int     outputChannels;
//    int     inputLatency;
//    int     outputLatency;
//    int     startDelay;
//    bool    verbose;
//    int     maximumNumberOfPorts;
}

void JackPresetsWidget::audioDriverNameChanged(QString driverName) {
    _ui->inputDeviceComboBox->setDriverName(driverName);
    _ui->outputDeviceComboBox->setDriverName(driverName);

    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._audioDriverName = driverName;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::audioOperationModeChanged(int operationMode) {
    Q_UNUSED(operationMode);
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    switch(operationMode) {
    case 0:
        _ui->inputDeviceComboBox->setEnabled(true);
        _ui->outputDeviceComboBox->setEnabled(true);
        preset._operationMode = Settings::OperationModeDuplex;
        break;
    case 1:
        _ui->inputDeviceComboBox->setEnabled(true);
        _ui->outputDeviceComboBox->setEnabled(false);
        preset._operationMode = Settings::OperationModeCapture;
        break;
    case 2:
        _ui->inputDeviceComboBox->setEnabled(false);
        _ui->outputDeviceComboBox->setEnabled(true);
        preset._operationMode = Settings::OperationModePlayback;
        break;
    }
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::inputDeviceActivated(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._inputDeviceIdentifier = _ui->inputDeviceComboBox->itemData(index).toString();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::outputDeviceActivated(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._outputDeviceIdentifier = _ui->outputDeviceComboBox->itemData(index).toString();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::sampleRateActivated(QString sampleRate) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._samplesPerFrame = sampleRate.toInt();
    JackControl::instance().setCurrentPreset(preset);
}

JackPresetsWidget::~JackPresetsWidget() {
    delete _ui;
}
