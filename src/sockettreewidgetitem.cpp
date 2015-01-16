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

// Qt includes
#include <QTreeWidgetItem>

// Own includes
#include "sockettreewidgetitem.h"
#include "sockettreewidget.h"
#include "socketlist.h"
#include "Patchbay.h"

SocketTreeWidgetItem::SocketTreeWidgetItem ( SocketList *pSocketList,
    const QString& sSocketName, const QString& sClientName,
    int iSocketType, SocketTreeWidgetItem *pSocketAfter )
    : QTreeWidgetItem(pSocketList->listView(), pSocketAfter, QJACKCTL_SOCKETITEM) {
    QTreeWidgetItem::setText(0, sSocketName);

    m_pSocketList = pSocketList;
    m_sSocketName = sSocketName;
    m_sClientName = sClientName;
    m_iSocketType = iSocketType;
    m_bExclusive  = false;
    m_sSocketForward.clear();

    m_pSocketList->sockets().append(this);

    updatePixmap();
}

// Default destructor.
SocketTreeWidgetItem::~SocketTreeWidgetItem ()
{
    QListIterator<SocketTreeWidgetItem *> iter(m_connects);
    while (iter.hasNext())
        (iter.next())->removeConnect(this);

    m_connects.clear();
    m_plugs.clear();

    int iSocket = m_pSocketList->sockets().indexOf(this);
    if (iSocket >= 0)
        m_pSocketList->sockets().removeAt(iSocket);
}


// Instance accessors.
const QString& SocketTreeWidgetItem::socketName () const
{
    return m_sSocketName;
}

const QString& SocketTreeWidgetItem::clientName () const
{
    return m_sClientName;
}

int SocketTreeWidgetItem::socketType () const
{
    return m_iSocketType;
}

bool SocketTreeWidgetItem::isExclusive () const
{
    return m_bExclusive;
}

const QString& SocketTreeWidgetItem::forward () const
{
    return m_sSocketForward;
}


void SocketTreeWidgetItem::setSocketName ( const QString& sSocketName )
{
    m_sSocketName = sSocketName;
}

void SocketTreeWidgetItem::setClientName ( const QString& sClientName )
{
    m_sClientName = sClientName;
}

void SocketTreeWidgetItem::setSocketType ( int iSocketType )
{
    m_iSocketType = iSocketType;
}

void SocketTreeWidgetItem::setExclusive ( bool bExclusive )
{
    m_bExclusive = bExclusive;
}

void SocketTreeWidgetItem::setForward ( const QString& sSocketForward )
{
    m_sSocketForward = sSocketForward;
}


// Socket flags accessor.
bool SocketTreeWidgetItem::isReadable () const
{
    return m_pSocketList->isReadable();
}


// Plug finder.
PlugTreeWidgetItem *SocketTreeWidgetItem::findPlug ( const QString& sPlugName )
{
    QListIterator<PlugTreeWidgetItem *> iter(m_plugs);
    while (iter.hasNext()) {
        PlugTreeWidgetItem *pPlug = iter.next();
        if (sPlugName == pPlug->plugName())
            return pPlug;
    }

    return NULL;
}


// Plug list accessor.
QList<PlugTreeWidgetItem *>& SocketTreeWidgetItem::plugs ()
{
    return m_plugs;
}


// Connected socket list primitives.
void SocketTreeWidgetItem::addConnect( SocketTreeWidgetItem *pSocket )
{
    m_connects.append(pSocket);
}

void SocketTreeWidgetItem::removeConnect( SocketTreeWidgetItem *pSocket )
{
    int iSocket = m_connects.indexOf(pSocket);
    if (iSocket >= 0)
        m_connects.removeAt(iSocket);
}



// Connected socket finder.
SocketTreeWidgetItem *SocketTreeWidgetItem::findConnectPtr (
    SocketTreeWidgetItem *pSocketPtr )
{
    QListIterator<SocketTreeWidgetItem *> iter(m_connects);
    while (iter.hasNext()) {
        SocketTreeWidgetItem *pSocket = iter.next();
        if (pSocketPtr == pSocket)
            return pSocket;
    }

    return NULL;
}


// Connection cache list accessor.
const QList<SocketTreeWidgetItem *>& SocketTreeWidgetItem::connects () const
{
    return m_connects;
}


// Plug list cleaner.
void SocketTreeWidgetItem::clear ()
{
    qDeleteAll(m_plugs);
    m_plugs.clear();
}


// Socket item pixmap peeker.
const QPixmap& SocketTreeWidgetItem::pixmap ( int iPixmap ) const
{
    return m_pSocketList->pixmap(iPixmap);
}


// Update pixmap to its proper context.
void SocketTreeWidgetItem::updatePixmap ()
{
    int iPixmap;
    if (m_iSocketType == QJACKCTL_SOCKETTYPE_JACK_AUDIO) {
        iPixmap = (m_bExclusive
            ? QJACKCTL_XPM_AUDIO_SOCKET_X
            : QJACKCTL_XPM_AUDIO_SOCKET);
    } else {
        iPixmap = (m_bExclusive
            ? QJACKCTL_XPM_MIDI_SOCKET_X
            : QJACKCTL_XPM_MIDI_SOCKET);
    }
    QTreeWidgetItem::setIcon(0, QIcon(pixmap(iPixmap)));
}


// Socket item openness status.
void SocketTreeWidgetItem::setOpen ( bool bOpen )
{
    QTreeWidgetItem::setExpanded(bOpen);
}


bool SocketTreeWidgetItem::isOpen () const
{
    return QTreeWidgetItem::isExpanded();
}
