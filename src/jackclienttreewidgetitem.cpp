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
#include "jackclienttreewidgetitem.h"
#include "jackclientlist.h"
#include "clientlisttreewidget.h"

JackClientTreeWidgetItem::JackClientTreeWidgetItem ( JackClientList *pClientList,
    const QString& sClientName )
    : QTreeWidgetItem(pClientList->listTreeWidget(), QJACKCTL_CLIENTITEM) {
    m_pClientList = pClientList;
    m_sClientName = sClientName;
    m_iClientMark = 0;
    m_iHilite     = 0;

    m_pClientList->clients().append(this);

    // Check aliasing...
    ConnectAlias *pAliases
        = (pClientList->listTreeWidget())->aliases();
    if (pAliases) {
        QTreeWidgetItem::setText(0,
            pAliases->clientAlias(sClientName));
        if ((pClientList->listTreeWidget())->renameEnabled()) {
            QTreeWidgetItem::setFlags(QTreeWidgetItem::flags()
                | Qt::ItemIsEditable);
        }
    } else {
        QTreeWidgetItem::setText(0, sClientName);
    }
}

JackClientTreeWidgetItem::~JackClientTreeWidgetItem () {
    qDeleteAll(m_ports);
    m_ports.clear();

    int iClient = m_pClientList->clients().indexOf(this);
    if (iClient >= 0)
        m_pClientList->clients().removeAt(iClient);
}

JackPortTreeWidgetItem *JackClientTreeWidgetItem::findPort (const QString& sPortName) {
    QListIterator<JackPortTreeWidgetItem *> iter(m_ports);
    while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
        if (sPortName == pPort->portName())
            return pPort;
    }

    return NULL;
}

JackClientList *JackClientTreeWidgetItem::clientList () const {
    return m_pClientList;
}

QList<JackPortTreeWidgetItem *>& JackClientTreeWidgetItem::ports () {
    return m_ports;
}

void JackClientTreeWidgetItem::setClientName ( const QString& sClientName ) {
    QTreeWidgetItem::setText(0, sClientName);

    m_sClientName = sClientName;
}

const QString& JackClientTreeWidgetItem::clientName () const {
    return m_sClientName;
}

bool JackClientTreeWidgetItem::isReadable () const {
    return m_pClientList->isReadable();
}

void JackClientTreeWidgetItem::markClient ( int iMark ) {
    setHilite(false);
    m_iClientMark = iMark;
}

void JackClientTreeWidgetItem::markClientPorts ( int iMark ) {
    markClient(iMark);

    QListIterator<JackPortTreeWidgetItem *> iter(m_ports);
    while (iter.hasNext())
        (iter.next())->markPort(iMark);
}

int JackClientTreeWidgetItem::cleanClientPorts ( int iMark ) {
    int iDirtyCount = 0;

    QMutableListIterator<JackPortTreeWidgetItem *> iter(m_ports);
    while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
        if (pPort->portMark() == iMark) {
            iter.remove();
            delete pPort;
            iDirtyCount++;
        }
    }

    return iDirtyCount;
}

int JackClientTreeWidgetItem::clientMark () const {
    return m_iClientMark;
}

bool JackClientTreeWidgetItem::isHilite () const {
    return (m_iHilite > 0);
}

void JackClientTreeWidgetItem::setHilite ( bool bHilite ) {
    if (bHilite)
        m_iHilite++;
    else
    if (m_iHilite > 0)
        m_iHilite--;

    // Set the new color.
    QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
    if (pTreeWidget == NULL)
        return;

    const QPalette& pal = pTreeWidget->palette();
    QTreeWidgetItem::setTextColor(0, m_iHilite > 0
        ? (pal.base().color().value() < 0x7f ? Qt::darkCyan : Qt::darkBlue)
        : pal.text().color());
}

void JackClientTreeWidgetItem::setOpen ( bool bOpen ) {
    QTreeWidgetItem::setExpanded(bOpen);
}

bool JackClientTreeWidgetItem::isOpen () const {
    return QTreeWidgetItem::isExpanded();
}

bool JackClientTreeWidgetItem::operator< ( const QTreeWidgetItem& other ) const {
    return JackClientList::lessThan(*this, other);
}
