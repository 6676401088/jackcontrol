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
class JackClientList;
#include "jackporttreewidgetitem.h"

// Qt includes
#include <QTreeWidgetItem>

#define QJACKCTL_CLIENTITEM    1001

class JackClientTreeWidgetItem : public QTreeWidgetItem {
public:
    JackClientTreeWidgetItem(JackClientList *pClientList,
        const QString& sClientName);
    ~JackClientTreeWidgetItem();

    // Port list primitive methods.
    void addPort(JackPortTreeWidgetItem *pPort);
    void removePort(JackPortTreeWidgetItem *pPort);

    // Port finder.
    JackPortTreeWidgetItem *findPort(const QString& sPortName);

    // Instance accessors.
    void setClientName(const QString& sClientName);
    const QString& clientName() const;

    // Readable flag accessor.
    bool isReadable() const;

    // Client list accessor.
    JackClientList *clientList() const;

    // Port list accessor.
    QList<JackPortTreeWidgetItem *>& ports();

    // Client port cleanup marker.
    void markClient(int iMark);
    void markClientPorts(int iMark);
    int cleanClientPorts(int iMark);
    int clientMark() const;

    // Connectiopn highlight methods.
    bool isHilite() const;
    void setHilite (bool bHilite);

    // Client item openness status.
    void setOpen(bool bOpen);
    bool isOpen() const;

    // Proxy sort override method.
    // - Natural decimal sorting comparator.
    bool operator< (const QTreeWidgetItem& other) const;

private:

    // Instance variables.
    JackClientList *m_pClientList;

    QString m_sClientName;
    int     m_iClientMark;
    int     m_iHilite;

    QList<JackPortTreeWidgetItem *> m_ports;
};

