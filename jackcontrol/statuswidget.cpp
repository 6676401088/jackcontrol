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
#include "settings.h"
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

    treeWidgetSubItem = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("XRUN count since last server startup") + c << n);
    _treeWidgetItems[XRunCount] = treeWidgetSubItem;
    _treeWidgetItems[XRunTime]  = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN last time detected") + c << n);
    _treeWidgetItems[XRunLast]  = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN last") + c << n);
    _treeWidgetItems[XRunMax]   = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN maximum") + c << n);
    _treeWidgetItems[XRunMin]   = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN minimum") + c << n);
    _treeWidgetItems[XRunAvg]   = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN average") + c << n);
    _treeWidgetItems[XRunTotal] = new QTreeWidgetItem(treeWidgetSubItem,
		QStringList() << s + tr("XRUN total") + c << n);
    _treeWidgetItems[MaxDelay]  = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Maximum scheduling delay") + c << n);
    _treeWidgetItems[ResetTime] = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Time of last reset") + c << n);

    ui->statusTreeWidget->resizeColumnToContents(0);	// Description.
    ui->statusTreeWidget->resizeColumnToContents(1);	// Value.

    startTimer(100);
}

StatusWidget::~StatusWidget() {
}

void StatusWidget::updateStatusItem(StatusItem statusItem, QString value) {
    _treeWidgetItems[statusItem]->setText(1, value);
}

void StatusWidget::timerEvent(QTimerEvent *timerEvent) {
    Q_UNUSED(timerEvent);

    QJack::Client& client = JackService::instance().client();

    updateStatusItem(SampleRate, QString("%1 Hz").arg(client.sampleRate()));

    float cpuLoad = client.cpuLoad();
    if(cpuLoad > 5) {
        updateStatusItem(DspLoad, QString("%1 %").arg(QString::number(cpuLoad, 'g', 3)));
    } else {
        updateStatusItem(DspLoad, "Idle");
    }

    updateStatusItem(BufferSize, QString("%1 Samples").arg(client.bufferSize()));
    updateStatusItem(Realtime, client.isRealtime() ? "Yes" : "No");

    QJack::TransportPosition transportPosition = client.transportPosition();
    updateStatusItem(TransportBBT, transportPosition.bbtDataValid() ?
        QString("%1 | %2 | %3")
            .arg(transportPosition._bbt._bar)
            .arg(transportPosition._bbt._beat)
            .arg(transportPosition._bbt._tick) :
        QString("Unavailable")
    );

    updateStatusItem(TransportBPM, transportPosition.bbtDataValid() ?
        QString("%1")
            .arg(transportPosition._bbt._beatsPerMinute) :
        QString("Unavailable")
    );

    updateStatusItem(TransportTimeCode, transportPosition.timeCodeValid() ?
        QString("%1s / %2s")
            .arg(transportPosition._timeCode._frameTimeSeconds)
            .arg(transportPosition._timeCode._nextFrameTimeSeconds) :
        QString("Unavailable")
    );
}
