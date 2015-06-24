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
#include "about.h"
#include "statuswidget.h"
#include "jackservice.h"

// Qt includes
#include <QHeaderView>
#include <QTimerEvent>

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::StatusWidget()) {
    ui->setupUi(this);

	const QString s = " ";
	const QString c = ":" + s;
	const QString n = "--";
    QTreeWidgetItem *treeWidgetSubItem;

	// Status list view...
    QHeaderView *headerView = ui->statusTreeWidget->header();
    headerView->setDefaultAlignment(Qt::AlignLeft);
    headerView->setSectionsMovable(false);
    headerView->setStretchLastSection(true);

    _treeWidgetItems[ServerState] = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Server state") + c << n);
    _treeWidgetItems[DspLoad]     = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("DSP Load") + c << n);
    _treeWidgetItems[SampleRate]  = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Sample Rate") + c << n);
    _treeWidgetItems[BufferSize]  = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Buffer Size") + c << n);
    _treeWidgetItems[Realtime]     = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Realtime Mode") + c << n);

    treeWidgetSubItem = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Transport state") + c << n);
    _treeWidgetItems[TransportState] = treeWidgetSubItem;
    _treeWidgetItems[TransportTimeCode] = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("Transport Timecode") + c << n);
    _treeWidgetItems[TransportBBT]  = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("Transport BBT") + c << n);
    _treeWidgetItems[TransportBPM]  = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("Transport BPM") + c << n);
    _treeWidgetItems[TicksPerBeat]  = new QTreeWidgetItem(treeWidgetSubItem,
        QStringList() << s + tr("Ticks per beat") + c << n);
    _treeWidgetItems[TimeSignature]  = new QTreeWidgetItem(treeWidgetSubItem,
        QStringList() << s + tr("Time signature") + c << n);

    ui->statusTreeWidget->resizeColumnToContents(0);	// Description.
    ui->statusTreeWidget->resizeColumnToContents(1);	// Value.
    ui->statusTreeWidget->expandAll();

    startTimer(200);
}

StatusWidget::~StatusWidget() {
}

void StatusWidget::updateStatusItem(StatusItem statusItem, QString value) {
    _treeWidgetItems[statusItem]->setText(1, value);
}

void StatusWidget::timerEvent(QTimerEvent *timerEvent) {
    Q_UNUSED(timerEvent);

    QtJack::Client& client = JackService::instance().client();
    if(client.isValid()) {
        setEnabled(true);
        updateStatusItem(SampleRate, QString("%1 Hz").arg(client.sampleRate()));

        float cpuLoad = client.cpuLoad();
        if(cpuLoad > 5) {
            updateStatusItem(DspLoad, QString("%1 %").arg(QString::number(cpuLoad, 'g', 3)));
        } else {
            updateStatusItem(DspLoad, tr("Idle"));
        }

        updateStatusItem(BufferSize, QString("%1 Samples").arg(client.bufferSize()));
        updateStatusItem(Realtime, client.isRealtime() ? tr("Yes") : tr("No"));

        QtJack::TransportState transportState = client.transportState();
        QString transportStateString = tr("Unknown");
        switch(transportState) {
            case QtJack::TransportStateStopped:
                transportStateString = tr("Stopped");
                break;
            case QtJack::TransportStateRolling:
                transportStateString = tr("Rolling");
                break;
            case QtJack::TransportStateLooping:
                transportStateString = tr("Looping");
                break;
            case QtJack::TransportStateStarting:
                transportStateString = tr("Starting");
                break;
            case QtJack::TransportStateNetStarting:
                transportStateString = tr("NetStarting");
                break;
            default:
                break;
        }
        updateStatusItem(TransportState, transportStateString);

        QtJack::TransportPosition transportPosition = client.queryTransportPosition();
        updateStatusItem(TransportBBT, transportPosition.bbtDataValid() ?
            QString("%1 | %2 | %3")
                .arg(transportPosition._bbt._bar)
                .arg(transportPosition._bbt._beat)
                .arg(transportPosition._bbt._tick) :
            tr("Unavailable")
        );

        updateStatusItem(TransportBPM, transportPosition.bbtDataValid() ?
            QString("%1")
                .arg(transportPosition._bbt._beatsPerMinute) :
            tr("Unavailable")
        );

        updateStatusItem(TransportTimeCode, transportPosition.timeCodeValid() ?
            QString("%1s / %2s")
                .arg(transportPosition._timeCode._frameTimeSeconds)
                .arg(transportPosition._timeCode._nextFrameTimeSeconds) :
            tr("Unavailable")
        );

        updateStatusItem(TicksPerBeat, transportPosition.bbtDataValid() ?
            QString("%1")
                .arg(transportPosition._bbt._ticksPerBeat) :
            tr("Unavailable")
        );

        updateStatusItem(TimeSignature, transportPosition.bbtDataValid() ?
            QString("%1 / %2")
                .arg(transportPosition._bbt._timeSignatureNominator)
                .arg(transportPosition._bbt._timeSignatureDenominator) :
            tr("Unavailable")
        );
    } else {
        setEnabled(false);
    }
}
