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

// Own includes
class ClientListTreeWidget;
#include "jackclienttreewidgetitem.h"

// Qt includes
#include <QObject>

class JackClientList : public QObject {
public:
    JackClientList(ClientListTreeWidget *listTreeWidget, bool readable);
    ~JackClientList();

    /** Do proper contents cleanup. */
    void clear();

    /** Client list primitive methods. */
    void addClient(JackClientTreeWidgetItem *pClient);
    void removeClient(JackClientTreeWidgetItem *pClient);

    /** Client finder. */
    JackClientTreeWidgetItem *findClient(const QString& clientName);

    /** Client:port finder. */
    JackPortTreeWidgetItem *findClientPort(const QString& clientPortName);

    /** List view accessor. */
    ClientListTreeWidget *listTreeWidget() const;

    /** Readable flag accessor. */
    bool isReadable() const;

    /** Client list accessor. */
    QList<JackClientTreeWidgetItem *>& clients();

    /** Client ports cleanup marker. */
    void markClientPorts(int iMark);
    int cleanClientPorts(int iMark);

    /** Client:port refreshner (return newest item count) */
    virtual int updateClientPorts() = 0;

    /** Client:port hilite update stabilization. */
    void hiliteClientPorts ();

    /** Do proper contents refresh/update. */
    void update();

    /** Natural decimal sorting comparator. */
    static bool lessThan(const QTreeWidgetItem& i1, const QTreeWidgetItem& i2);

private:
    ClientListTreeWidget *_listTreeWidget;
    bool _readable;

    QList<JackClientTreeWidgetItem *> _clients;

    QTreeWidgetItem *m_pHiliteItem;
};
