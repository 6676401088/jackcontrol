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

// Own includes
#include "clientlisttreewidget.h"
#include "jackportconnectorwidget.h"
#include "jackclientlist.h"

// Qt includes
#include <QSplitter>

class ConnectionsSplitter : public QSplitter {
	Q_OBJECT
public:
    enum IconSize {
        IconSize16x16 = 0,
        IconSize32x32 = 1,
        IconSize64x64 = 2
    };

    ConnectionsSplitter(QWidget *parent = 0);
    ~ConnectionsSplitter();

    ClientListTreeWidget *outputTreeWidget() const {
        return _outputTreeWidget;
    }

    JackPortConnectorWidget *ConnectorView() const {
        return _connectorView;
    }

    ClientListTreeWidget *inputTreeWidget() const {
        return _inputTreeWidget;
    }

    /** Client list accessors. */
    JackClientList *outputClientList() const;
    JackClientList *inputClientList() const;

    /** Connector line style accessors. */
    void setDrawingBezierLines(bool drawingBezierLines);
    bool isDrawingBezierLines() const;

    /** Common icon size pixmap accessors. */
    void setIconSize(IconSize iconSize);
    IconSize iconSize() const;

signals:
    /** Contents change signal. */
	void contentsChanged();

private:
    ClientListTreeWidget *      _outputTreeWidget;
    JackPortConnectorWidget  *  _connectorView;
    ClientListTreeWidget *      _inputTreeWidget;

    bool _drawingBezierLines;

    IconSize _iconSize;
};


