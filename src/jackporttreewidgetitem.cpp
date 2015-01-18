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
#include "jackporttreewidgetitem.h"
#include "jackclienttreewidgetitem.h"
#include "connectalias.h"
#include "jackclientlist.h"
#include "clientlisttreewidget.h"

JackPortTreeWidgetItem::JackPortTreeWidgetItem ( JackClientTreeWidgetItem *pClient,
    const QString& sPortName )
    : QTreeWidgetItem(pClient, QJACKCTL_PORTITEM) {
    m_pClient   = pClient;
    m_sPortName = sPortName;
    m_iPortMark = 0;
    m_bHilite   = false;

    m_pClient->ports().append(this);

    // Check aliasing...
    ConnectAlias *pAliases
        = ((pClient->clientList())->listTreeWidget())->aliases();
    if (pAliases) {
        QTreeWidgetItem::setText(0,
            pAliases->portAlias(pClient->clientName(), sPortName));
        if (((pClient->clientList())->listTreeWidget())->renameEnabled()) {
            QTreeWidgetItem::setFlags(QTreeWidgetItem::flags()
                | Qt::ItemIsEditable);
        }
    } else {
        QTreeWidgetItem::setText(0, sPortName);
    }
}

JackPortTreeWidgetItem::~JackPortTreeWidgetItem () {
    int iPort = m_pClient->ports().indexOf(this);
    if (iPort >= 0)
        m_pClient->ports().removeAt(iPort);

    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
    while (iter.hasNext())
        (iter.next())->removeConnect(this);

    m_connects.clear();
}

void JackPortTreeWidgetItem::setPortName ( const QString& sPortName ) {
    QTreeWidgetItem::setText(0, sPortName);

    m_sPortName = sPortName;
}

const QString& JackPortTreeWidgetItem::clientName () const {
    return m_pClient->clientName();
}

const QString& JackPortTreeWidgetItem::portName () const {
    return m_sPortName;
}

QString JackPortTreeWidgetItem::clientPortName () const {
    return m_pClient->clientName() + ':' + m_sPortName;
}

JackClientTreeWidgetItem *JackPortTreeWidgetItem::client () const {
    return m_pClient;
}

void JackPortTreeWidgetItem::markPort ( int iMark ) {
    setHilite(false);
    m_iPortMark = iMark;
    if (iMark > 0)
        m_connects.clear();
}

void JackPortTreeWidgetItem::markClientPort ( int iMark ) {
    markPort(iMark);

    m_pClient->markClient(iMark);
}

int JackPortTreeWidgetItem::portMark () const {
    return m_iPortMark;
}

void JackPortTreeWidgetItem::addConnect ( JackPortTreeWidgetItem *pPort ) {
    m_connects.append(pPort);
}

void JackPortTreeWidgetItem::removeConnect ( JackPortTreeWidgetItem *pPort ) {
    pPort->setHilite(false);

    int iPort = m_connects.indexOf(pPort);
    if (iPort >= 0)
        m_connects.removeAt(iPort);
}

JackPortTreeWidgetItem *JackPortTreeWidgetItem::findConnect ( const QString& sClientPortName ) {
    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
    while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
        if (sClientPortName == pPort->clientPortName())
            return pPort;
    }

    return NULL;
}

JackPortTreeWidgetItem *JackPortTreeWidgetItem::findConnectPtr ( JackPortTreeWidgetItem *pPortPtr ) {
    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
    while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
        if (pPortPtr == pPort)
            return pPort;
    }

    return NULL;
}

const QList<JackPortTreeWidgetItem *>& JackPortTreeWidgetItem::connects () const {
    return m_connects;
}

bool JackPortTreeWidgetItem::isHilite () const {
    return m_bHilite;
}

void JackPortTreeWidgetItem::setHilite ( bool bHilite ) {
    // Update the port highlightning if changed...
    if ((m_bHilite && !bHilite) || (!m_bHilite && bHilite)) {
        m_bHilite = bHilite;
        // Propagate this to the parent...
        m_pClient->setHilite(bHilite);
    }

    // Set the new color.
    QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
    if (pTreeWidget == NULL)
        return;

    const QPalette& pal = pTreeWidget->palette();
    QTreeWidgetItem::setTextColor(0, m_bHilite
        ? (pal.base().color().value() < 0x7f ? Qt::cyan : Qt::blue)
        : pal.text().color());
}

bool JackPortTreeWidgetItem::operator< ( const QTreeWidgetItem& other ) const {
    return JackClientList::lessThan(*this, other);
}
