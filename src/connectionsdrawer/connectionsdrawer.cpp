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
    _returnTreeWidget        = new ClientTreeWidget();
    _sendTreeWidget       = new ClientTreeWidget();
    _portConnectionsWidget  = new PortConnectionsWidget(_sendTreeWidget,
                                                        _returnTreeWidget);

    // This is a bit hacky, but we need this to get the scrollbar to the left
    // without changing the layout direction while preserving the existing
    // scrollbar.
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_sendTreeWidget->verticalScrollBar());
    layout->addWidget(_sendTreeWidget);
    layout->setSpacing(3);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    addWidget(widget);
    addWidget(_portConnectionsWidget);
    addWidget(_returnTreeWidget);

    setCollapsible(0, false);
    setCollapsible(1, false);
    setCollapsible(2, false);

    _sendTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _returnTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setHandleWidth(0);

    _sendTreeWidget->setHeaderTitle(tr("Auxiliary sends"));
    _returnTreeWidget->setHeaderTitle(tr("Auxiliary returns"));

    connect(_sendTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
    connect(_sendTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
    connect(_sendTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(update()));
    connect(_sendTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(update()));

    connect(_sendTreeWidget, SIGNAL(itemSelectionChanged()),
        _portConnectionsWidget, SLOT(update()));
    connect(_sendTreeWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(itemSelectionChanged()));
    connect(_sendTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this, SLOT(itemChanged(QTreeWidgetItem*,int)));

    connect(_returnTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
    connect(_returnTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        _portConnectionsWidget, SLOT(update()));
    connect(_returnTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
        _portConnectionsWidget, SLOT(update()));
    connect(_returnTreeWidget->header(), SIGNAL(sectionClicked(int)),
        _portConnectionsWidget, SLOT(update()));

    connect(_returnTreeWidget, SIGNAL(itemSelectionChanged()),
        _portConnectionsWidget, SLOT(update()));
    connect(_returnTreeWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(itemSelectionChanged()));
    connect(_sendTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this, SLOT(itemChanged(QTreeWidgetItem*,int)));

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
    _sendTreeWidget->collapseAll();
    _returnTreeWidget->collapseAll();
}

void ConnectionsDrawer::expandAll() {
    _sendTreeWidget->expandAll();
    _returnTreeWidget->expandAll();
}

void ConnectionsDrawer::itemSelectionChanged() {
    emit itemSelectionChanged(_sendTreeWidget->selectedItems(),
                              _returnTreeWidget->selectedItems());
}

void ConnectionsDrawer::setSelectionMode(QAbstractItemView::SelectionMode selectionMode) {
    _sendTreeWidget->setSelectionMode(selectionMode);
    _returnTreeWidget->setSelectionMode(selectionMode);
}

void ConnectionsDrawer::itemChanged(QTreeWidgetItem* treeWidgetItem, int column) {
    Q_UNUSED(treeWidgetItem);
    Q_UNUSED(column);
}
