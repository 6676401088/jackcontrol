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
#include "About.h"
#include "statuswidget.h"
#include "Status.h"
#include "settings.h"
#include "mainwidget.h"

// Qt includes
#include <QFile>
#include <QDateTime>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
#include <QHeaderView>
#include <QShowEvent>
#include <QHideEvent>

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::StatusWidget()) {
    ui->setupUi(this);

	// Create the list view items 'a priori'...
	const QString s = " ";
	const QString c = ":" + s;
	const QString n = "--";
    QTreeWidgetItem *treeWidgetSubItem;

	// Status list view...
    QHeaderView *pHeader = ui->statusTreeWidget->header();
    pHeader->setDefaultAlignment(Qt::AlignLeft);
	pHeader->setSectionsMovable(false);
	pHeader->setStretchLastSection(true);

    _treeWidgetItems[ServerName] = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Server name") + c << n);
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
    _treeWidgetItems[TransportTime] = new QTreeWidgetItem(treeWidgetSubItem,
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

#ifdef CONFIG_JACK_MAX_DELAY
    _treeWidgetItems[MaxDelay]  = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Maximum scheduling delay") + c << n);
#endif
    _treeWidgetItems[ResetTime] = new QTreeWidgetItem(ui->statusTreeWidget,
		QStringList() << s + tr("Time of last reset") + c << n);

    ui->statusTreeWidget->resizeColumnToContents(0);	// Description.
    ui->statusTreeWidget->resizeColumnToContents(1);	// Value.
}

StatusWidget::~StatusWidget() {
}

void StatusWidget::updateStatusItem(StatusItem statusItem, QString value) {
    _treeWidgetItems[statusItem]->setText(1, value);
}
