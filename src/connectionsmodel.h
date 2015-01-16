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
class ConnectionsViewSplitter;
#include "jackclientlist.h"
#include "jackporttreewidgetitem.h"

// Qt includes
#include <QObject>

class ConnectionsModel : public QObject
{
    Q_OBJECT

public:
    ConnectionsModel(ConnectionsViewSplitter *pConnectView);
    ~ConnectionsModel();

    // Explicit connection tests.
    bool canConnectSelected();
    bool canDisconnectSelected();
    bool canDisconnectAll();

    // Client list accessors.
    JackClientList *OClientList() const;
    JackClientList *IClientList() const;

public slots:

    // Incremental contents refreshner; check dirty status.
    void refresh();

    // Explicit connection slots.
    bool connectSelected();
    bool disconnectSelected();
    bool disconnectAll();

    // Expand all client ports.
    void expandAll();

    // Complete/incremental contents rebuilder; check dirty status if incremental.
    void updateContents(bool bClear);

signals:

    // Connection change signal.
    void connectChanged();

    // Pre-notification of (dis)connection.
    void connecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);
    void disconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);

protected:

    // Connect/Disconnection primitives.
    virtual bool connectPorts(
        JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort) = 0;
    virtual bool disconnectPorts(
        JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort) = 0;

    // Update port connection references.
    virtual void updateConnections() = 0;

    // These must be accessed by the descendant constructor.
    ConnectionsViewSplitter *connectView() const;
    void setOClientList(JackClientList *pOClientList);
    void setIClientList(JackClientList *pIClientList);

    // Common pixmap factory helper-method.
    QPixmap *createIconPixmap (const QString& sIconName);

    // Update icon size implementation.
    virtual void updateIconPixmaps() = 0;

private:

    // Dunno. But this may avoid some conflicts.
    bool startMutex();
    void endMutex();

    // Connection methods (unguarded).
    bool canConnectSelectedEx();
    bool canDisconnectSelectedEx();
    bool canDisconnectAllEx();
    bool connectSelectedEx();
    bool disconnectSelectedEx();
    bool disconnectAllEx();

    // Connect/Disconnection local primitives.
    bool connectPortsEx(JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort);
    bool disconnectPortsEx(JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort);

    // Instance variables.
    ConnectionsViewSplitter *m_pConnectView;
    // These must be created on the descendant constructor.
    JackClientList *m_pOClientList;
    JackClientList *m_pIClientList;
    int m_iMutex;
};
