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

// Qt includes
#include <QWidget>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QVector>

// QtJack includes
#include "client.h"
#include "audioport.h"
#include "processor.h"
#include "ringbuffer.h"

namespace Ui {
class JackScopeWidget;
}

class JackScopeWidget :
    public QWidget,
    public QtJack::Processor {
    Q_OBJECT

    struct Probe {
        QtJack::AudioPort       _port;
        QtJack::AudioRingBuffer _ringBuffer;
        QVector<double>         _samples;
    };

public:
    explicit JackScopeWidget(QWidget *parent = 0);
    ~JackScopeWidget();

public slots:
    void on_resolutionSpinBox_valueChanged(int value);
    void on_timeSpanDoubleSpinBox_valueChanged(double value);
    void on_addProbePushButton_clicked();
    void on_removeProbePushButton_clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

    void process(int samples);

protected slots:
    void jackServerHasStarted();
    void jackServerHasStopped();

private:
    void addProbe();
    void removeProbe();

    Ui::JackScopeWidget *_ui;
    int _resolution;
    QtJack::Client _client;

    QList<Probe*> _probes;
};
