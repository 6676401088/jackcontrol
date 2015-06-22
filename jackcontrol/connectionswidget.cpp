/****************************************************************************
   Copyright (C) 2003-2011, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include "connectionswidget.h"

ConnectionsWidget::ConnectionsWidget(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);

    // Set default selection mode
    ui.jackConnectionsDrawer->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui.comboBoxSelectionMode->setCurrentIndex(2); // Extended
}

ConnectionsWidget::~ConnectionsWidget() {
}

void ConnectionsWidget::on_pushButtonConnect_clicked() {
    ui.jackConnectionsDrawer->connectSelectedItems();
}

void ConnectionsWidget::on_pushButtonDisconnect_clicked() {
    ui.jackConnectionsDrawer->disconnectSelectedItems();
}

void ConnectionsWidget::on_comboBoxSelectionMode_activated(int index) {
    switch(index) {
    default:
    case 0: // Single selection
        ui.jackConnectionsDrawer->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case 1: // Multiple selection
        ui.jackConnectionsDrawer->setSelectionMode(QAbstractItemView::MultiSelection);
        break;
    case 2: // Extended selection
        ui.jackConnectionsDrawer->setSelectionMode(QAbstractItemView::ExtendedSelection);
        break;
    case 3: // Contiguous selection
        ui.jackConnectionsDrawer->setSelectionMode(QAbstractItemView::ContiguousSelection);
        break;
    }
}

void ConnectionsWidget::on_pushButtonDisconnectAll_clicked() {
    ui.jackConnectionsDrawer->disconnectAll();
}

void ConnectionsWidget::on_pushButtonCollapseAll_clicked() {
    ui.jackConnectionsDrawer->collapseAll();
}

void ConnectionsWidget::on_pushButtonExpandAll_clicked() {
    ui.jackConnectionsDrawer->expandAll();
}

