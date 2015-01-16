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

// Own includes
#include "connectionsmodel.h"
#include "ConnectViewSplitter.h"
#include "About.h"

// Qt includes
#include <QMessageBox>

ConnectionsModel::ConnectionsModel ( ConnectionsViewSplitter *pConnectView ) {
    m_pConnectView = pConnectView;

    m_pOClientList = NULL;
    m_pIClientList = NULL;

    m_iMutex = 0;

    m_pConnectView->setBinding(this);
}

ConnectionsModel::~ConnectionsModel ()
{
    // Force end of works here.
    m_iMutex++;

    m_pConnectView->setBinding(NULL);

    if (m_pOClientList)
        delete m_pOClientList;
    if (m_pIClientList)
        delete m_pIClientList;

    m_pOClientList = NULL;
    m_pIClientList = NULL;

    m_pConnectView->ConnectorView()->update();
}

ConnectionsViewSplitter *ConnectionsModel::connectView () const
{
    return m_pConnectView;
}

void ConnectionsModel::setOClientList ( JackClientList *pOClientList )
{
    m_pOClientList = pOClientList;
}

void ConnectionsModel::setIClientList ( JackClientList *pIClientList )
{
    m_pIClientList = pIClientList;
}

bool ConnectionsModel::connectPortsEx (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
    if (pOPort->findConnectPtr(pIPort) != NULL)
        return false;

    emit connecting(pOPort, pIPort);

    if (!connectPorts(pOPort, pIPort))
        return false;

    pOPort->addConnect(pIPort);
    pIPort->addConnect(pOPort);
    return true;
}

bool ConnectionsModel::disconnectPortsEx (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
    if (pOPort->findConnectPtr(pIPort) == NULL)
        return false;

    emit disconnecting(pOPort, pIPort);

    if (!disconnectPorts(pOPort, pIPort))
        return false;

    pOPort->removeConnect(pIPort);
    pIPort->removeConnect(pOPort);
    return true;
}

bool ConnectionsModel::canConnectSelected ()
{
    bool bResult = false;

    if (startMutex()) {
        bResult = canConnectSelectedEx();
        endMutex();
    }

    return bResult;
}

bool ConnectionsModel::canConnectSelectedEx ()
{
    // Take this opportunity to highlight any current selections.
    m_pOClientList->hiliteClientPorts();
    m_pIClientList->hiliteClientPorts();

    // Now with our predicate work...
    QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
    if (pOItem == NULL)
        return false;

    QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
    if (pIItem == NULL)
        return false;

    if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
                if (pOPort->findConnectPtr(pIPort) == NULL)
                    return true;
            }
        } else {
            // Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            while (oport.hasNext()
                && pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
                if (pOPort->findConnectPtr(pIPort) == NULL)
                    return true;
                pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
            }
        }
    } else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
                if (pOPort->findConnectPtr(pIPort) == NULL)
                    return true;
            }
        } else {
            // One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
            return (pOPort->findConnectPtr(pIPort) == NULL);
        }
    }

    return false;
}

bool ConnectionsModel::connectSelected ()
{
    bool bResult = false;

    if (startMutex()) {
        bResult = connectSelectedEx();
        endMutex();
    }

    m_pConnectView->ConnectorView()->update();

    if (bResult)
        emit connectChanged();

    return bResult;
}

bool ConnectionsModel::connectSelectedEx ()
{
    QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
    if (pOItem == NULL)
        return false;

    QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
    if (pIItem == NULL)
        return false;

    if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
                connectPortsEx(pOPort, pIPort);
            }
        } else {
            // Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            while (oport.hasNext()
                && pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
                connectPortsEx(pOPort, pIPort);
                pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
            }
        }
    } else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
                connectPortsEx(pOPort, pIPort);
            }
        } else {
            // One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
            connectPortsEx(pOPort, pIPort);
        }
    }

    return true;
}

bool ConnectionsModel::canDisconnectSelected ()
{
    bool bResult = false;

    if (startMutex()) {
        bResult = canDisconnectSelectedEx();
        endMutex();
    }

    return bResult;
}

bool ConnectionsModel::canDisconnectSelectedEx ()
{
    QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
    if (!pOItem)
        return false;

    QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
    if (!pIItem)
        return false;

    if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
                if (pOPort->findConnectPtr(pIPort) != NULL)
                    return true;
            }
        } else {
            // Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            while (oport.hasNext()
                && pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
                if (pOPort->findConnectPtr(pIPort) != NULL)
                    return true;
                pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
            }
        }
    } else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
                if (pOPort->findConnectPtr(pIPort) != NULL)
                    return true;
            }
        } else {
            // One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
            return (pOPort->findConnectPtr(pIPort) != NULL);
        }
    }

    return false;
}

bool ConnectionsModel::disconnectSelected ()
{
    bool bResult = false;

    if (startMutex()) {
        bResult = disconnectSelectedEx();
        endMutex();
    }

    m_pConnectView->ConnectorView()->update();

    if (bResult)
        emit connectChanged();

    return bResult;
}

bool ConnectionsModel::disconnectSelectedEx ()
{
    QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
    if (pOItem == NULL)
        return false;

    QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
    if (pIItem == NULL)
        return false;

    if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
                disconnectPortsEx(pOPort, pIPort);
            }
        } else {
            // Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            while (oport.hasNext()
                && pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
                disconnectPortsEx(pOPort, pIPort);
                pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
            }
        }
    } else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
        if (pIItem->type() == QJACKCTL_CLIENTITEM) {
            // One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
            while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
                disconnectPortsEx(pOPort, pIPort);
            }
        } else {
            // One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
            disconnectPortsEx(pOPort, pIPort);
        }
    }

    return true;
}

bool ConnectionsModel::canDisconnectAll ()
{
    bool bResult = false;

    if (startMutex()) {
        bResult = canDisconnectAllEx();
        endMutex();
    }

    return bResult;
}

bool ConnectionsModel::canDisconnectAllEx ()
{
    QListIterator<JackClientTreeWidgetItem *> iter(m_pOClientList->clients());
    while (iter.hasNext()) {
        JackClientTreeWidgetItem *pOClient = iter.next();
        QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
        while (oport.hasNext()) {
            JackPortTreeWidgetItem *pOPort = oport.next();
            if (pOPort->connects().count() > 0)
                return true;
        }
    }
    return false;
}

bool ConnectionsModel::disconnectAll ()
{
    if (QMessageBox::warning(m_pConnectView,
        tr("Warning") + " - " QJACKCTL_SUBTITLE1,
        tr("This will suspend sound processing\n"
        "from all client applications.\n\nAre you sure?"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return false;
    }

    bool bResult = false;

    if (startMutex()) {
        bResult = disconnectAllEx();
        endMutex();
    }

    m_pConnectView->ConnectorView()->update();

    if (bResult)
        emit connectChanged();

    return bResult;
}

bool ConnectionsModel::disconnectAllEx ()
{
    QListIterator<JackClientTreeWidgetItem *> iter(m_pOClientList->clients());
    while (iter.hasNext()) {
        JackClientTreeWidgetItem *pOClient = iter.next();
        QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
        while (oport.hasNext()) {
            JackPortTreeWidgetItem *pOPort = oport.next();
            QListIterator<JackPortTreeWidgetItem *> iport(pOPort->connects());
            while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
                disconnectPortsEx(pOPort, pIPort);
            }
        }
    }
    return true;
}

void ConnectionsModel::expandAll ()
{
    (m_pOClientList->listView())->expandAll();
    (m_pIClientList->listView())->expandAll();
    (m_pConnectView->ConnectorView())->update();
}

void ConnectionsModel::updateContents ( bool bClear )
{
    int iDirtyCount = 0;

    if (startMutex()) {
        // Do we do a complete rebuild?
        if (bClear) {
            m_pOClientList->clear();
            m_pIClientList->clear();
            updateIconPixmaps();
        }
        // Add (newer) client:ports and respective connections...
        if (m_pOClientList->updateClientPorts() > 0) {
            m_pOClientList->refresh();
            iDirtyCount++;
        }
        if (m_pIClientList->updateClientPorts() > 0) {
            m_pIClientList->refresh();
            iDirtyCount++;
        }
        updateConnections();
        endMutex();
    }

    (m_pConnectView->ConnectorView())->update();

    if (!bClear && iDirtyCount > 0)
        emit connectChanged();
}

void ConnectionsModel::refresh ()
{
    updateContents(false);
}

bool ConnectionsModel::startMutex ()
{
    bool bMutex = (m_iMutex == 0);
    if (bMutex)
        m_iMutex++;
    return bMutex;
}

void ConnectionsModel::endMutex ()
{
    if (m_iMutex > 0)
        m_iMutex--;
}

JackClientList *ConnectionsModel::OClientList () const
{
    return m_pOClientList;
}

JackClientList *ConnectionsModel::IClientList () const
{
    return m_pIClientList;
}

QPixmap *ConnectionsModel::createIconPixmap ( const QString& sIconName )
{
    QString sName = sIconName;
    int     iSize = m_pConnectView->iconSize() * 32;

    if (iSize > 0)
        sName += QString("_%1x%2").arg(iSize).arg(iSize);

    return new QPixmap(":/images/" + sName + ".png");
}
