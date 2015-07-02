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

// Own includes
#include "settings.h"
#include "jackcontrol.h"

// Qt includes
#include <QWidget>

namespace Ui {
class JackPresetsWidget;
}

class JackPresetsWidget :
    public QWidget {
    Q_OBJECT

public:
    explicit JackPresetsWidget(QWidget *parent = 0);
    ~JackPresetsWidget();

protected slots:
    // Preset management
    void on_importPresetPushButton_clicked();
    void on_exportPresetPushButton_clicked();
    void on_deletePresetPushButton_clicked();

    // Audio system
    void on_audioDriverComboBox_activated(QString driverName);
    // -- stub: midi driver --
    void on_operationModeComboBox_activated(int operationMode);
    void on_inputDeviceComboBox_activated(int index);
    void on_outputDeviceComboBox_activated(int index);

    // Audio processing
    void on_samplesPerFrameComboBox_currentIndexChanged(int index);
    void on_sampleRateComboBox_currentIndexChanged(int index);
    void on_numberOfBuffersSpinBox_valueChanged(int value);
    void on_maximumNumberOfPortsComboBox_currentIndexChanged(int index);
    void on_ditherComboBox_currentIndexChanged(int index);
    void on_realtimePrioritySpinBox_valueChanged(int value);
    void on_enableRealtimeProcessingCheckBox_clicked();
    void on_enableHardwareMonitoringCheckBox_clicked();
    void on_enableHardwareMeteringCheckBox_clicked();
    void on_provideMonitorPortsCheckBox_clicked();

    // Advanced parameters and tweaks
    void on_clientTimeoutComboBox_currentIndexChanged(int index);
    void on_maximumNumberOfAudioChannelsSpinBox_valueChanged(int value);
    void on_maximumNumberOfHardwareInputChannelsSpinBox_valueChanged(int value);
    void on_maximumNumberOfHardwareOutputChannelsSpinBox_valueChanged(int value);
    void on_externalInputLatencySpinBox_valueChanged(int value);
    void on_externalOutputLatencySpinBox_valueChanged(int value);
    void on_dummyDriverProcessingDelayComboBox_currentIndexChanged(int index);
    void on_wordLengthComboBox_currentIndexChanged(int index);
    void on_noMemoryLockCheckBox_clicked();
    void on_ignoreHardwareBufferSizeCheckBox_clicked();
    void on_unlockMemoryCheckBox_clicked();
    void on_enableSoftModeCheckBox_clicked();
    void on_force16BitWordLengthCheckBox_clicked();
    void on_showVerboseMessagesCheckBox_clicked();

    void updateWithPreset(Settings::JackServerPreset preset, QStringList *errorReport = 0);

private:
    Ui::JackPresetsWidget *_ui;
};
