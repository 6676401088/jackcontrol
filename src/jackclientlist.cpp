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
#include "jackclientlist.h"
#include "clientlisttreewidget.h"

// Qt includes
#include <QHeaderView>

JackClientList::JackClientList (
    ClientListTreeWidget *pListView, bool bReadable ) {
    m_pListView = pListView;
    m_bReadable = bReadable;

    m_pHiliteItem = 0;
}

JackClientList::~JackClientList () {
    clear();
}

void JackClientList::clear () {
    qDeleteAll(m_clients);
    m_clients.clear();

    if (m_pListView)
        m_pListView->clear();
}

JackClientTreeWidgetItem *JackClientList::findClient (
    const QString& sClientName ) {
    QListIterator<JackClientTreeWidgetItem *> iter(m_clients);
    while (iter.hasNext()) {
        JackClientTreeWidgetItem *pClient = iter.next();
        if (sClientName == pClient->clientName())
            return pClient;
    }

    return NULL;
}

JackPortTreeWidgetItem *JackClientList::findClientPort (
    const QString& sClientPort ) {
    JackPortTreeWidgetItem *pPort = 0;
    int iColon = sClientPort.indexOf(':');
    if (iColon >= 0) {
        JackClientTreeWidgetItem *pClient = findClient(sClientPort.left(iColon));
        if (pClient) {
            pPort = pClient->findPort(
                sClientPort.right(sClientPort.length() - iColon - 1));
        }
    }
    return pPort;
}

QList<JackClientTreeWidgetItem *>& JackClientList::clients() {
    return m_clients;
}

ClientListTreeWidget *JackClientList::listView() const {
    return m_pListView;
}

bool JackClientList::isReadable() const {
    return m_bReadable;
}

void JackClientList::markClientPorts(int iMark) {
    m_pHiliteItem = 0;

    QListIterator<JackClientTreeWidgetItem *> iter(m_clients);
    while (iter.hasNext())
        (iter.next())->markClientPorts(iMark);
}

int JackClientList::cleanClientPorts(int iMark) {
    int iDirtyCount = 0;

    QMutableListIterator<JackClientTreeWidgetItem *> iter(m_clients);
    while (iter.hasNext()) {
        JackClientTreeWidgetItem *pClient = iter.next();
        if (pClient->clientMark() == iMark) {
            iter.remove();
            delete pClient;
            iDirtyCount++;
        } else {
            iDirtyCount += pClient->cleanClientPorts(iMark);
        }
    }

    return iDirtyCount;
}

void JackClientList::hiliteClientPorts() {
    JackClientTreeWidgetItem *pClient;
    JackPortTreeWidgetItem *pPort;

    QTreeWidgetItem *pItem = m_pListView->currentItem();

    // Dehilite the previous selected items.
    if (m_pHiliteItem && pItem != m_pHiliteItem) {
        if (m_pHiliteItem->type() == QJACKCTL_CLIENTITEM) {
            pClient = static_cast<JackClientTreeWidgetItem *> (m_pHiliteItem);
            QListIterator<JackPortTreeWidgetItem *> iter(pClient->ports());
            while (iter.hasNext()) {
                pPort = iter.next();
                QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
                while (it.hasNext())
                    (it.next())->setHilite(false);
            }
        } else {
            pPort = static_cast<JackPortTreeWidgetItem *> (m_pHiliteItem);
            QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
            while (it.hasNext())
                (it.next())->setHilite(false);
        }
    }

    // Hilite the now current selected items.
    if (pItem) {
        if (pItem->type() == QJACKCTL_CLIENTITEM) {
            pClient = static_cast<JackClientTreeWidgetItem *> (pItem);
            QListIterator<JackPortTreeWidgetItem *> iter(pClient->ports());
            while (iter.hasNext()) {
                pPort = iter.next();
                QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
                while (it.hasNext())
                    (it.next())->setHilite(true);
            }
        } else {
            pPort = static_cast<JackPortTreeWidgetItem *> (pItem);
            QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
            while (it.hasNext())
                (it.next())->setHilite(true);
        }
    }

    // Do remember this one, ever.
    m_pHiliteItem = pItem;
}

bool JackClientList::lessThan(const QTreeWidgetItem& i1, const QTreeWidgetItem& i2) {
    const QString& s1 = i1.text(0);
    const QString& s2 = i2.text(0);

    int ich1, ich2;

    int cch1 = s1.length();
    int cch2 = s2.length();

    for (ich1 = ich2 = 0; ich1 < cch1 && ich2 < cch2; ich1++, ich2++) {

        // Skip (white)spaces...
        while (s1.at(ich1).isSpace())
            ich1++;
        while (s2.at(ich2).isSpace())
            ich2++;

        // Normalize (to uppercase) the next characters...
        QChar ch1 = s1.at(ich1).toUpper();
        QChar ch2 = s2.at(ich2).toUpper();

        if (ch1.isDigit() && ch2.isDigit()) {
            // Find the whole length numbers...
            int iDigits1 = ich1++;
            while (ich1 < cch1 && s1.at(ich1).isDigit())
                ich1++;
            int iDigits2 = ich2++;
            while (ich2 < cch2 && s2.at(ich2).isDigit())
                ich2++;
            // Compare as natural decimal-numbers...
            int n1 = s1.mid(iDigits1, ich1 - iDigits1).toInt();
            int n2 = s2.mid(iDigits2, ich2 - iDigits2).toInt();
            if (n1 != n2)
                return (n1 < n2);
            // Never go out of bounds...
            if (ich1 >= cch1 || ich1 >= cch2)
                break;
            // Go on with this next char...
            ch1 = s1.at(ich1).toUpper();
            ch2 = s2.at(ich2).toUpper();
        }

        // Compare this char...
        if (ch1 != ch2)
            return (ch1 < ch2);
    }

    // Probable exact match.
    return false;
}

void JackClientList::refresh () {
    QHeaderView *pHeader = m_pListView->header();
    m_pListView->sortItems(
        pHeader->sortIndicatorSection(),
        pHeader->sortIndicatorOrder());
}
