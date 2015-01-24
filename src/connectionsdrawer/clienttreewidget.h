/****************************************************************************
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

// Qt includes
#include <QTreeWidget>
class QWheelEvent;

// Own includes
#include "connectionsdrawer.h"
#include "porttreewidgetitem.h"

/**
 * An actual client tree.
 */
class ClientTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    ClientTreeWidget(QWidget *parent = 0);
    virtual ~ClientTreeWidget();

    void setHeaderTitle(QString headerTitle);

    QList<PortTreeWidgetItem *> ports();

    void propagateWheelEvent(QWheelEvent *wheelEvent);

protected:
    void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
    void dragMoveEvent(QDragMoveEvent *pDragMoveEvent);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *pDropEvent);

    // Context menu request event handler.
    void contextMenuEvent(QContextMenuEvent *);
};
