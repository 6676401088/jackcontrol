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
class SocketList;
#include "plugtreewidgetitem.h"

// Qt includes
#include <QTreeWidgetItem>

class SocketTreeWidgetItem : public QTreeWidgetItem {
public:

    // Constructor.
    SocketTreeWidgetItem(SocketList *pSocketList,
        const QString& sSocketName, const QString& sClientName,
        int iSocketType,SocketTreeWidgetItem *pSocketAfter);

    // Default destructor.
    ~SocketTreeWidgetItem();

    // Instance accessors.
    const QString& socketName() const;
    const QString& clientName() const;
    int socketType() const;
    bool isExclusive() const;
    const QString& forward() const;

    void setSocketName (const QString& sSocketName);
    void setClientName (const QString& sClientName);
    void setSocketType (int iSocketType);
    void setExclusive  (bool bExclusive);
    void setForward    (const QString& sSocketForward);

    // Socket flags accessor.
    bool isReadable() const;

    // Connected plug list primitives.
    void addConnect(SocketTreeWidgetItem *pSocket);
    void removeConnect(SocketTreeWidgetItem *pSocket);

    // Connected plug  finders.
    SocketTreeWidgetItem *findConnectPtr(SocketTreeWidgetItem *pSocketPtr);

    // Connection list accessor.
    const QList<SocketTreeWidgetItem *>& connects() const;

    // Plug list primitive methods.
    void addPlug(PlugTreeWidgetItem *pPlug);
    void removePlug(PlugTreeWidgetItem *pPlug);

    // Plug finder.
    PlugTreeWidgetItem *findPlug(const QString& sPlugName);

    // Plug list accessor.
    QList<PlugTreeWidgetItem *>& plugs();

    // Plug list cleaner.
    void clear();

    // Retrieve a context pixmap.
    const QPixmap& pixmap(int iPixmap) const;

    // Update pixmap to its proper context.
    void updatePixmap();

    // Client item openness status.
    void setOpen(bool bOpen);
    bool isOpen() const;

private:

    // Instance variables.
    SocketList *m_pSocketList;
    QString m_sSocketName;
    QString m_sClientName;
    int m_iSocketType;
    bool m_bExclusive;
    QString m_sSocketForward;

    // Plug (port) list.
    QList<PlugTreeWidgetItem *> m_plugs;

    // Connection cache list.
    QList<SocketTreeWidgetItem *> m_connects;
};
