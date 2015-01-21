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
#include <QTreeWidgetItem>

#define QJACKCTL_CLIENTITEM    1001

/**
 * A tree widget item that represents a client in the tree.
 */
class ClientTreeWidgetItem : public QTreeWidgetItem {
public:
    ClientTreeWidgetItem(QString clientName);
    virtual ~ClientTreeWidgetItem();

    // Instance accessors.
    void setClientName(QString clientName);
    QString clientName() const;

    // Client port cleanup marker.
    void markClient(int mark);
    int clientMark() const;

    // Connectiopn highlight methods.
    bool isHighlighted() const;
    void setHighlighted(bool highlighted);

    // Client item openness status.
    void setOpen(bool bOpen);
    bool isOpen() const;

private:
    QString _clientName;
    int     _clientMark;
    int     _hilight;
};

