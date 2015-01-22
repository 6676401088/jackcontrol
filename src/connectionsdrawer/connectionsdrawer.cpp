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
#include <QHBoxLayout>

ConnectionsDrawer::ConnectionsDrawer(QWidget *parent)
    : QSplitter(Qt::Horizontal, parent) {
    _inputTreeWidget        = new ClientTreeWidget();
    _outputTreeWidget       = new ClientTreeWidget();
    _portConnectionsWidget  = new PortConnectionsWidget(_outputTreeWidget,
                                                        _inputTreeWidget);

    // This is a bit hacky, but we need this to get the scrollbar to the left
    // without changing the layout direction while preserving the existing
    // scrollbar.
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_outputTreeWidget->verticalScrollBar());
    layout->addWidget(_outputTreeWidget);
    layout->setSpacing(3);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    addWidget(widget);
    addWidget(_portConnectionsWidget);
    addWidget(_inputTreeWidget);

    setCollapsible(0, false);
    setCollapsible(1, false);
    setCollapsible(2, false);

    _outputTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _inputTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setHandleWidth(0);

    _outputTreeWidget->setHeaderTitle(tr("Auxiliary sends"));
    _inputTreeWidget->setHeaderTitle(tr("Auxiliary returns"));

	connect(_outputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
	connect(_outputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
	connect(_outputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(update()));
	connect(_outputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(update()));

    connect(_outputTreeWidget, SIGNAL(itemSelectionChanged()),
        _portConnectionsWidget, SLOT(update()));
    connect(_outputTreeWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(itemSelectionChanged()));

	connect(_inputTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
	connect(_inputTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
	connect(_inputTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(update()));
	connect(_inputTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(update()));

    connect(_inputTreeWidget, SIGNAL(itemSelectionChanged()),
        _portConnectionsWidget, SLOT(update()));
    connect(_inputTreeWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(itemSelectionChanged()));
}

ConnectionsDrawer::~ConnectionsDrawer () {
}

void ConnectionsDrawer::connectSelectedItems() {

}

void ConnectionsDrawer::disconnectSelectedItems() {

}

void ConnectionsDrawer::disconnectAll() {

}

void ConnectionsDrawer::collapseAll() {
    _outputTreeWidget->collapseAll();
    _inputTreeWidget->collapseAll();
}

void ConnectionsDrawer::expandAll() {
    _outputTreeWidget->expandAll();
    _inputTreeWidget->expandAll();
}

void ConnectionsDrawer::itemSelectionChanged() {
    emit itemSelectionChanged(_outputTreeWidget->selectedItems(),
                              _inputTreeWidget->selectedItems());
}
