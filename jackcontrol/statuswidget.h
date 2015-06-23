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

#pragma once

// uic includes
#include "ui_statuswidget.h"

// Qt includes
#include <QMap>

class QTreeWidgetItem;

class StatusWidget : public QWidget {
	Q_OBJECT

public:
    enum StatusItem {
        ServerState = 0,
        DspLoad,
        SampleRate,
        BufferSize,
        Realtime,
        TransportState,
        TransportTimeCode,
        TransportBBT,
        TransportBPM,
        XRunCount,
        XRunTime,
        XRunLast,
        XRunMax,
        XRunMin,
        XRunAvg,
        XRunTotal,
        ResetTime,
        MaxDelay
    };

    StatusWidget(QWidget *parent = 0);
    ~StatusWidget();

    void updateStatusItem(StatusItem statusItem, QString value);

protected:
    void timerEvent(QTimerEvent *timerEvent);

private:
    Ui::StatusWidget *ui;

    QMap<StatusItem, QTreeWidgetItem*> _treeWidgetItems;
};

