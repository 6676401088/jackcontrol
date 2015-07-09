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
        painter.drawLine(_ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * (xx) / 1024, _ui->labelTimeDomain->pos().y(),
                         _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * (xx) / 1024, _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height());
    }

    for(int yy = gridY; yy < 1024; yy += gridY) {
        painter.drawLine(_ui->labelTimeDomain->pos().x(), _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() * (yy) / 1024,
                         _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width(), _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() * (yy) / 1024);
    }

    pen.setColor(Qt::white);
    pen.setWidth(2);
    painter.setPen(pen);

    if(_client.isValid()) {
        int numberOfSamples = 2048;
        jack_default_audio_sample_t samples[numberOfSamples];
        while(_ringBuffer.numberOfElementsAvailableForRead() > numberOfSamples) {
            _ringBuffer.read(samples, numberOfSamples);

            for(int i = 1; i < numberOfSamples; i++) {
                int x1 = _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * (i - 1) / numberOfSamples;
                int x2 = _ui->labelTimeDomain->pos().x() + _ui->labelTimeDomain->width() * (i - 0) / numberOfSamples;
                int y1 = _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() / 2 + _ui->labelTimeDomain->height() * samples[i - 1] / 2.0;
                int y2 = _ui->labelTimeDomain->pos().y() + _ui->labelTimeDomain->height() / 2 + _ui->labelTimeDomain->height() * samples[i - 0] / 2.0;

                painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
            }
        }

    }
}

void JackScopeWidget::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    update();
}

void JackScopeWidget::process(int samples) {
    _probePort.buffer(samples).push(_ringBuffer);
}

void JackScopeWidget::jackServerHasStarted() {
    _client.disconnectFromServer(); // Just in case
    _client.connectToServer("JACK Control Scope");
    _probePort = _client.registerAudioInPort("probe_1");
    _client.setProcessor(this);
    _client.activate();
}

void JackScopeWidget::jackServerHasStopped() {
    _client.disconnectFromServer();
}

