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
class JackClientTreeWidgetItem;

// Qt includes
#include <QTreeWidgetItem>

#define QJACKCTL_PORTITEM      1002

class JackPortTreeWidgetItem : public QTreeWidgetItem {
public:
    JackPortTreeWidgetItem(JackClientTreeWidgetItem *pClient, const QString& sPortName);
    ~JackPortTreeWidgetItem();

    // Instance accessors.
    void setPortName(const QString& sPortName);
    const QString& clientName() const;
    const QString& portName() const;

    // Complete client:port name helper.
    QString clientPortName() const;

    // Connections client item accessor.
    JackClientTreeWidgetItem *client() const;

    // Client port cleanup marker.
    void markPort(int iMark);
    void markClientPort(int iMark);

    int portMark() const;

    // Connected port list primitives.
    void addConnect(JackPortTreeWidgetItem *pPort);
    void removeConnect(JackPortTreeWidgetItem *pPort);

    // Connected port finders.
    JackPortTreeWidgetItem *findConnect(const QString& sClientPortName);
    JackPortTreeWidgetItem *findConnectPtr(JackPortTreeWidgetItem *pPortPtr);

    // Connection list accessor.
    const QList<JackPortTreeWidgetItem *>& connects() const;

    // Connectiopn highlight methods.
    bool isHilite() const;
    void setHilite (bool bHilite);

    // Proxy sort override method.
    // - Natural decimal sorting comparator.
    bool operator< (const QTreeWidgetItem& other) const;

private:

    // Instance variables.
    JackClientTreeWidgetItem *m_pClient;

    QString m_sPortName;
    int     m_iPortMark;
    bool    m_bHilite;

    // Connection cache list.
    QList<JackPortTreeWidgetItem *> m_connects;
};
