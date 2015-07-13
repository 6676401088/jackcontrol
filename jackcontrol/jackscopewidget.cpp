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
#include <QPainter>
#include <QDebug>

// Own includes
#include "jackscopewidget.h"
#include "jackcontrol.h"

// UIC includes
#include "ui_jackscopewidget.h"

JackScopeWidget::JackScopeWidget(QWidget *parent) :
    QWidget(parent),
    QtJack::Processor(),
    _ui(new Ui::JackScopeWidget) {
    _ui->setupUi(this);

    _ui->labelTimeDomain->setStyleSheet("");
    _ui->labelFrequencyDomain->setStyleSheet("");

    connect(&JackControl::instance(), SIGNAL(jackServerHasStarted()),
            this, SLOT(jackServerHasStarted()));
    connect(&JackControl::instance(), SIGNAL(jackServerHasStopped()),
            this, SLOT(jackServerHasStopped()));

    startTimer(50);
}

JackScopeWidget::~JackScopeWidget() {
    delete _ui;
}

void JackScopeWidget::on_resolutionSpinBox_valueChanged(int value) {
    _resolution = value;

    if(_resolution < 16) {
        _resolution = 16;
    }

    if(_resolution > _client.sampleRate() * 20) {
        _resolution = _client.sampleRate();
    }

    _ui->resolutionSpinBox->setValue(_resolution);
    _ui->timeSpanDoubleSpinBox->setValue((double)_resolution * 1000.0 / (double)_client.sampleRate());
}

void JackScopeWidget::on_timeSpanDoubleSpinBox_valueChanged(double value) {
    _resolution = value * (double)_client.sampleRate() / 1000.0;
    if(_resolution < 16) {
        _resolution = 16;
    }

    if(_resolution > _client.sampleRate() * 20) {
        _resolution = _client.sampleRate();
    }

    _ui->resolutionSpinBox->setValue(_resolution);
    _ui->resolutionSpinBox->setValue(_resolution);
}

void JackScopeWidget::on_addProbePushButton_clicked() {
    addProbe();
}

void JackScopeWidget::on_removeProbePushButton_clicked() {
    removeProbe();
}

void JackScopeWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen = painter.pen();

    painter.fillRect(QRect(_ui->labelTimeDomain->pos(), _ui->labelTimeDomain->size()), Qt::black);
    painter.fillRect(QRect(_ui->labelFrequencyDomain->pos(), _ui->labelFrequencyDomain->size()), Qt::black);

    pen.setColor(Qt::gray);
    pen.setWidthF(0.5);
    painter.setPen(pen);

    int gridX = 64, gridY = 128;

    for(int xx = gridX; xx < 1024; xx += gridX) {
        painter.drawLine(
            _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * xx / 1024,
            _ui->labelTimeDomain->pos().y(),
            _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * xx / 1024,
            _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height()
        );

        painter.drawLine(
            _ui->labelFrequencyDomain->pos().x() + _ui->labelFrequencyDomain->width() * xx / 1024,
            _ui->labelFrequencyDomain->pos().y(),
            _ui->labelFrequencyDomain->pos().x() + _ui->labelFrequencyDomain->width() * xx / 1024,
            _ui->labelFrequencyDomain->pos().y() + _ui->labelFrequencyDomain->height()
        );
    }

    for(int yy = gridY; yy < 1024; yy += gridY) {
        painter.drawLine(
            _ui->labelTimeDomain->pos().x(),
            _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() * (yy) / 1024,
            _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width(),
            _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() * (yy) / 1024
        );
        painter.drawLine(
            _ui->labelFrequencyDomain->pos().x(),
            _ui->labelFrequencyDomain->pos().y() + _ui->labelFrequencyDomain->height() * (yy) / 1024,
            _ui->labelFrequencyDomain->pos().x() + _ui->labelFrequencyDomain->width(),
            _ui->labelFrequencyDomain->pos().y() + _ui->labelFrequencyDomain->height() * (yy) / 1024
        );
    }

    if(_client.isValid()) {
        jack_default_audio_sample_t samples[16];

        int totalNumberOfProbes = _probes.count();
        int numberOfProbe = 0;
        foreach(Probe* probe, _probes) {
            QColor probeColor = QColor::fromHsv(359 * numberOfProbe / totalNumberOfProbes, 255, 255);
            pen.setColor(probeColor);
            pen.setWidthF(1.0);
            painter.setPen(pen);

            painter.fillRect(QRect(
                                _ui->labelTimeDomain->pos() + QPoint(10, 10 + 20 * numberOfProbe),
                                QSize(10, 10)
                             ),
                             probeColor);

            painter.drawText(_ui->labelTimeDomain->pos() + QPoint(30, 20 + 20 * numberOfProbe),
                             QString("Probe %1").arg(numberOfProbe + 1));

            int numberOfSamples = _resolution;

            // Pad drawing buffer if necessary
            if(probe->_samples.size() < numberOfSamples) {
                probe->_samples.resize(numberOfSamples);
            }

            // Shelve samples into probe buffer
            while(probe->_ringBuffer.numberOfElementsAvailableForRead() > 16) {
                probe->_ringBuffer.read(samples, 16);
                for(int i = 0; i < 16; i++) {
                    probe->_samples.append(samples[i]);
                }
            }

            if(probe->_samples.size() > numberOfSamples) {
                probe->_samples.remove(0, probe->_samples.size() - numberOfSamples);
            }

            for(int i = 1; i < numberOfSamples; i++) {
                int x1 = _ui->labelTimeDomain->pos().x() +
                         _ui->labelTimeDomain->width() * (i - 1) / numberOfSamples;
                int x2 = _ui->labelTimeDomain->pos().x() +
                         _ui->labelTimeDomain->width() * (i - 0) / numberOfSamples;
                int y1 = _ui->labelTimeDomain->pos().y() +
                         _ui->labelTimeDomain->height() / 2 +
                         _ui->labelTimeDomain->height() * probe->_samples[i - 1] / 2.0;
                int y2 = _ui->labelTimeDomain->pos().y() +
                         _ui->labelTimeDomain->height() / 2 +
                         _ui->labelTimeDomain->height() * probe->_samples[i - 0] / 2.0;
                painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
            }

            numberOfProbe++;
        }
    }
}

void JackScopeWidget::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    update();
}

void JackScopeWidget::process(int samples) {
    foreach(Probe* probe, _probes) {
        probe->_port.buffer(samples).push(probe->_ringBuffer);
    }
}

void JackScopeWidget::jackServerHasStarted() {
    _client.disconnectFromServer(); // Just in case

    _client.connectToServer("JACK Control Scope");
    _client.setProcessor(this);
    _client.activate();

    addProbe();

    _resolution = _ui->resolutionSpinBox->value();
}

void JackScopeWidget::jackServerHasStopped() {
    _client.disconnectFromServer();
}

void JackScopeWidget::addProbe() {
    if(_client.isValid()) {
        if(_probes.count() < 4) {
            Probe *probe = new Probe();
            probe->_port = _client.registerAudioInPort(QString("probe_%1").arg(_probes.size() + 1));
            probe->_ringBuffer = QtJack::AudioRingBuffer(_client.sampleRate() * 2);
            _probes.append(probe);
        }
    }
}

void JackScopeWidget::removeProbe() {
    if(_client.isValid()) {
        if(_probes.count() > 1) {
            Probe *probe = _probes.at(_probes.size() - 1);
            _client.unregisterPort(probe->_port);
            _probes.removeLast();
        }
    }
}


