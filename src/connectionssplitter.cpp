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

#include <Qt>

// Own includes
#include "About.h"
#include "connectionssplitter.h"

// Qt includes
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolTip>
#include <QPainter>
#include <QPolygon>
#include <QPainterPath>
#include <QTimer>
#include <QMenu>
#include <QMimeData>
#include <QDrag>
#include <QHelpEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

ConnectionsSplitter::ConnectionsSplitter (QWidget *parent )
    : QSplitter(Qt::Horizontal, parent) {
    _outputTreeWidget   = new ClientListTreeWidget(this, true);
    _connectorView      = new JackPortConnectorWidget(this);
    _inputTreeWidget    = new ClientListTreeWidget(this, false);

    _drawingBezierLines        = false;
    _iconSize           = IconSize16x16;

	connect(_outputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _connectorView, SLOT(contentsChanged()));
	connect(_outputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _connectorView, SLOT(contentsChanged()));
	connect(_outputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _connectorView, SLOT(contentsChanged()));
	connect(_outputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _connectorView, SLOT(contentsChanged()));

	connect(_inputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _connectorView, SLOT(contentsChanged()));
	connect(_inputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _connectorView, SLOT(contentsChanged()));
	connect(_inputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _connectorView, SLOT(contentsChanged()));
	connect(_inputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _connectorView, SLOT(contentsChanged()));
}

ConnectionsSplitter::~ConnectionsSplitter () {
}

JackClientList *ConnectionsSplitter::outputClientList() const {
    return 0;
}

JackClientList *ConnectionsSplitter::inputClientList() const {
    return 0;
}

void ConnectionsSplitter::setDrawingBezierLines(bool drawingBezierLines) {
    _drawingBezierLines = drawingBezierLines;
}

bool ConnectionsSplitter::isDrawingBezierLines() const {
    return _drawingBezierLines;
}

void ConnectionsSplitter::setIconSize(IconSize iconSize) {
    if (iconSize == _iconSize) {
		return;
    }

    _iconSize = iconSize;

	// Update item sizes properly...
    int px = (16 << _iconSize);
	const QSize iconsize(px, px);
	_outputTreeWidget->setIconSize(iconsize);
	_inputTreeWidget->setIconSize(iconsize);
}

ConnectionsSplitter::IconSize ConnectionsSplitter::iconSize() const {
    return _iconSize;
}
