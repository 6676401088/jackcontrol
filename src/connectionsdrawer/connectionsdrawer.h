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
class ClientTreeWidget;
class PortConnectionsWidget;

// Qt includes
#include <QList>
#include <QSplitter>
#include <QTreeWidgetItem>

class ConnectionsDrawer : public QSplitter {
	Q_OBJECT
public:
    ConnectionsDrawer(QWidget *parent = 0);
    virtual ~ConnectionsDrawer();

    void connectSelectedItems();
    void disconnectSelectedItems();
    void disconnectAll();
    void collapseAll();
    void expandAll();

public slots:
    void setSelectionMode(QAbstractItemView::SelectionMode selectionMode);

signals:
    void itemSelectionChanged(QList<QTreeWidgetItem*> selectedOutputItems,
                              QList<QTreeWidgetItem*> selectedInputItems);

protected slots:
    void itemSelectionChanged();

protected:
    ClientTreeWidget *      _sendTreeWidget;
    PortConnectionsWidget * _portConnectionsWidget;
    ClientTreeWidget *      _returnTreeWidget;
};


