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
#include "connectionsdrawer.h"
#include "clienttreewidget.h"
#include "portconnectionswidget.h"

// Qt includes
#include <QHeaderView>
#include <QScrollBar>

ConnectionsDrawer::ConnectionsDrawer(QWidget *parent)
    : QSplitter(Qt::Horizontal, parent) {
    _inputTreeWidget        = new ClientTreeWidget();
    _outputTreeWidget       = new ClientTreeWidget();
    _portConnectionsWidget  = new PortConnectionsWidget(_outputTreeWidget,
                                                        _inputTreeWidget);

    addWidget(_outputTreeWidget);
    addWidget(_portConnectionsWidget);
    addWidget(_inputTreeWidget);

    _iconSize               = IconSize16x16;

    _outputTreeWidget->setHeaderTitle("Outputs (Readable clients)");
    _inputTreeWidget->setHeaderTitle("Inputs (Writable clients)");

	connect(_outputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_outputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_outputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_outputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(contentsChanged()));

	connect(_inputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_inputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_inputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(contentsChanged()));
	connect(_inputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(contentsChanged()));
}

ConnectionsDrawer::~ConnectionsDrawer () {
}

void ConnectionsDrawer::setDrawingBezierLines(bool drawingBezierLines) {
    _portConnectionsWidget->setDrawingBezierLines(drawingBezierLines);
}

bool ConnectionsDrawer::isDrawingBezierLines() const {
    return _portConnectionsWidget->isDrawingBezierLines();
}

void ConnectionsDrawer::setIconSize(IconSize iconSize) {
    if(iconSize == _iconSize) {
		return;
    }

    _iconSize = iconSize;

	// Update item sizes properly...
    int px = (16 << _iconSize);
	const QSize iconsize(px, px);
	_outputTreeWidget->setIconSize(iconsize);
	_inputTreeWidget->setIconSize(iconsize);
}

ConnectionsDrawer::IconSize ConnectionsDrawer::iconSize() const {
    return _iconSize;
}
