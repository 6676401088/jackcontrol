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
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>

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

    QtJack::DriverMap drivers = JackService::instance().server().availableDrivers();
    foreach(QtJack::Driver driver, drivers) {
        _ui->audioDriverComboBox->addItem(driver.name());
    }

    if(drivers.size() > 0) {
        _ui->inputDeviceComboBox->setDriverName(drivers[0].name());
        _ui->outputDeviceComboBox->setDriverName(drivers[0].name());

        _ui->inputDeviceComboBox->update();
        _ui->outputDeviceComboBox->update();
    }

    _ui->inputDeviceComboBox->setOperationModeFilter(Settings::OperationModeCapture);
    _ui->outputDeviceComboBox->setOperationModeFilter(Settings::OperationModePlayback);
    _ui->operationModeComboBox->setCurrentIndex(0);
}

void JackPresetsWidget::on_importPresetPushButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import preset"), QDir::homePath(), tr("JACK Control presets (*.preset)"));
    if(!fileName.isEmpty()) {
        bool ok;
        Settings::JackServerPreset preset = Settings::loadPreset(fileName, &ok);
        if(ok) {
            JackControl::instance().setCurrentPreset(preset);
            QStringList errorReport;
            updateWithPreset(preset, &errorReport);
            if(!errorReport.isEmpty()) {
                QString report = tr("The preset has been imported, but please note:\n");
                foreach(QString reportLine, errorReport) {
                    report.append(reportLine + "\n");
                }
                QMessageBox::warning(this, tr("Import partially successful"), report);
            }
        } else {
            QMessageBox::critical(this, tr("Importing preset failed"), tr("An error occured during the export of the preset"));
        }
    }
}

void JackPresetsWidget::on_exportPresetPushButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export preset"), QDir::homePath(), tr("JACK Control presets (*.preset)"));
    if(!fileName.isEmpty()) {
        if(!fileName.endsWith(".preset")) {
            fileName.append(".preset");
        }
        if(!Settings::savePreset(fileName, JackControl::instance().currentPreset())) {
            QMessageBox::critical(this, tr("Exporting preset failed"), tr("An error occurred during the export of the preset."));
        }
    }
}

void JackPresetsWidget::on_deletePresetPushButton_clicked() {

}

void JackPresetsWidget::on_audioDriverComboBox_activated(QString driverName) {
    _ui->inputDeviceComboBox->setDriverName(driverName);
    _ui->outputDeviceComboBox->setDriverName(driverName);

    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._audioDriverName = driverName;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_operationModeComboBox_activated(int operationMode) {
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

void JackPresetsWidget::on_inputDeviceComboBox_activated(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._inputDeviceIdentifier = _ui->inputDeviceComboBox->itemData(index).toString();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_outputDeviceComboBox_activated(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._outputDeviceIdentifier = _ui->outputDeviceComboBox->itemData(index).toString();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_enableRealtimeProcessingCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._enableRealtimeProcessing = _ui->enableRealtimeProcessingCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_sampleRateComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._samplesPerSecond = _ui->sampleRateComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_samplesPerFrameComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._samplesPerFrame = _ui->samplesPerFrameComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_numberOfBuffersSpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._bufferSizeMultiplier = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_maximumNumberOfPortsComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._maximumNumberOfPorts = _ui->maximumNumberOfPortsComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_ditherComboBox_currentIndexChanged(int index) {

}

void JackPresetsWidget::on_realtimePrioritySpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._realtimePriority = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_enableHardwareMonitoringCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._enableHardwareMonitoring = _ui->enableHardwareMonitoringCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_enableHardwareMeteringCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._enableHardwareMetering = _ui->enableHardwareMeteringCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_provideMonitorPortsCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._provideMonitorPorts = _ui->provideMonitorPortsCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_clientTimeoutComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._clientTimeout = _ui->clientTimeoutComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_maximumNumberOfAudioChannelsSpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._maximumNumberOfAudioChannels = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_maximumNumberOfHardwareInputChannelsSpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._maximumNumberOfHardwareInputChannels = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_maximumNumberOfHardwareOutputChannelsSpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._maximumNumberOfHardwareOutputChannels = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_externalInputLatencySpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._externalInputLatency = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_externalOutputLatencySpinBox_valueChanged(int value) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._externalOutputLatency = value;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_dummyDriverProcessingDelayComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._dummyDriverProcessingDelay = _ui->dummyDriverProcessingDelayComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_wordLengthComboBox_currentIndexChanged(int index) {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._wordLength = _ui->wordLengthComboBox->itemText(index).toInt();
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_noMemoryLockCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._noMemoryLock = _ui->noMemoryLockCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_ignoreHardwareBufferSizeCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._ignoreHardwareBufferSize = _ui->ignoreHardwareBufferSizeCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_unlockMemoryCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._unlockMemory = _ui->unlockMemoryCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_enableSoftModeCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._enableSoftMode = _ui->enableSoftModeCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_force16BitWordLengthCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._force16BitWordLength = _ui->force16BitWordLengthCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::on_showVerboseMessagesCheckBox_clicked() {
    Settings::JackServerPreset preset = JackControl::instance().currentPreset();
    preset._showVerboseMessages = _ui->showVerboseMessagesCheckBox->checkState() == Qt::Checked;
    JackControl::instance().setCurrentPreset(preset);
}

void JackPresetsWidget::updateWithPreset(Settings::JackServerPreset preset, QStringList *errorReport) {
    // Preset management
    _ui->presetComboBox->lineEdit()->setText(preset._presetName);

    // Audio settings
    int numberOfAvailableAudioDrivers = _ui->audioDriverComboBox->count();
    bool foundAudioDriver = false;
    for(int i = 0; i < numberOfAvailableAudioDrivers; i++) {
        if(_ui->audioDriverComboBox->itemText(i) == preset._audioDriverName) {
            _ui->audioDriverComboBox    ->setCurrentIndex(i);

            // Repopulate devices after audio driver has changed
            _ui->inputDeviceComboBox    ->setDriverName(preset._audioDriverName);
            _ui->outputDeviceComboBox   ->setDriverName(preset._audioDriverName);
            _ui->inputDeviceComboBox    ->update();
            _ui->outputDeviceComboBox   ->update();

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
        _ui->operationModeComboBox  ->setCurrentIndex(0);
        _ui->inputDeviceComboBox    ->setEnabled(true);
        _ui->outputDeviceComboBox   ->setEnabled(true);
        break;
    case Settings::OperationModeCapture:
        _ui->operationModeComboBox  ->setCurrentIndex(1);
        _ui->inputDeviceComboBox    ->setEnabled(true);
        _ui->outputDeviceComboBox   ->setEnabled(false);
        break;
    case Settings::OperationModePlayback:
        _ui->operationModeComboBox  ->setCurrentIndex(2);
        _ui->inputDeviceComboBox    ->setEnabled(false);
        _ui->outputDeviceComboBox   ->setEnabled(true);
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

    _ui->enableRealtimeProcessingCheckBox->setChecked(preset._enableRealtimeProcessing);

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
            _ui->samplesPerFrameComboBox->lineEdit()->setText(QString("%1").arg(preset._samplesPerFrame));
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
        _ui->maximumNumberOfPortsComboBox->lineEdit()->setText(QString("%1").arg(preset._maximumNumberOfPorts));
    }

    // - Dithering
    // - Realtime Prio
    // - HW Mon
    // - HW Met
    // - Monitor Ports

    // Advanced configuration
    if(preset._clientTimeout < 0) {
        if(errorReport) {
            errorReport->append(tr("Client timeout is set to %1, but must be 0 or greater.").arg(preset._clientTimeout));
        }
    } else {
        _ui->clientTimeoutComboBox->lineEdit()->setText(QString("%1").arg(preset._clientTimeout));
    }

    if(preset._maximumNumberOfAudioChannels < -1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of audio channels is set to %1, but must be -1 or greater.").arg(preset._maximumNumberOfAudioChannels));
        }
    } else {
        _ui->maximumNumberOfAudioChannelsSpinBox->setValue(preset._maximumNumberOfAudioChannels);
    }

    if(preset._maximumNumberOfHardwareInputChannels < -1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of hardware input channels is set to %1, but must be -1 or greater.").arg(preset._maximumNumberOfHardwareInputChannels));
        }
    } else {
        _ui->maximumNumberOfHardwareInputChannelsSpinBox->setValue(preset._maximumNumberOfHardwareInputChannels);
    }

    if(preset._maximumNumberOfHardwareOutputChannels < -1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of hardware output channels is set to %1, but must be -1 or greater.").arg(preset._maximumNumberOfHardwareOutputChannels));
        }
    } else {
        _ui->maximumNumberOfHardwareOutputChannelsSpinBox->setValue(preset._maximumNumberOfHardwareOutputChannels);
    }

    if(preset._externalInputLatency < -1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of external input latency is set to %1, but must be -1 or greater.").arg(preset._externalInputLatency));
        }
    } else {
        _ui->externalInputLatencySpinBox->setValue(preset._externalInputLatency);
    }

    if(preset._externalOutputLatency < -1) {
        if(errorReport) {
            errorReport->append(tr("Maximum number of external output latency is set to %1, but must be -1 or greater.").arg(preset._externalOutputLatency));
        }
    } else {
        _ui->externalOutputLatencySpinBox->setValue(preset._externalOutputLatency);
    }

    if(preset._dummyDriverProcessingDelay < 0) {
        if(errorReport) {
            errorReport->append(tr("Dummy processing delay is set to %1, but must be -1 or greater.").arg(preset._dummyDriverProcessingDelay));
        }
    } else {
        _ui->dummyDriverProcessingDelayComboBox->lineEdit()->setText(QString("%1").arg(preset._dummyDriverProcessingDelay));
    }

    if(preset._wordLength < -1) {
        if(errorReport) {
            errorReport->append(tr("Word length is set to %1, but must be -1 or greater.").arg(preset._wordLength));
        }
    } else {
        _ui->wordLengthComboBox->lineEdit()->setText(QString("%1").arg(preset._wordLength));
    }

    _ui->noMemoryLockCheckBox               ->setChecked(preset._noMemoryLock);
    _ui->unlockMemoryCheckBox               ->setChecked(preset._unlockMemory);
    _ui->force16BitWordLengthCheckBox       ->setChecked(preset._force16BitWordLength);
    _ui->ignoreHardwareBufferSizeCheckBox   ->setChecked(preset._ignoreHardwareBufferSize);
    _ui->enableSoftModeCheckBox             ->setChecked(preset._enableSoftMode);
    _ui->showVerboseMessagesCheckBox        ->setChecked(preset._showVerboseMessages);
}


JackPresetsWidget::~JackPresetsWidget() {
    delete _ui;
}
