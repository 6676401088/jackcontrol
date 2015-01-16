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
#include "sockettreewidgetitem.h"
#include "Patchbay.h"

// Own includes
#include "plugtreewidgetitem.h"

PlugTreeWidgetItem::PlugTreeWidgetItem ( SocketTreeWidgetItem *pSocket,
    const QString& sPlugName, PlugTreeWidgetItem *pPlugAfter )
    : QTreeWidgetItem(pSocket, pPlugAfter, QJACKCTL_PLUGITEM)
{
    QTreeWidgetItem::setText(0, sPlugName);

    m_pSocket   = pSocket;
    m_sPlugName = sPlugName;

    m_pSocket->plugs().append(this);

    int iPixmap;
    if (pSocket->socketType() == QJACKCTL_SOCKETTYPE_JACK_AUDIO)
        iPixmap = QJACKCTL_XPM_AUDIO_PLUG;
    else
        iPixmap = QJACKCTL_XPM_MIDI_PLUG;
    QTreeWidgetItem::setIcon(0, QIcon(pSocket->pixmap(iPixmap)));

    QTreeWidgetItem::setFlags(QTreeWidgetItem::flags()
        & ~Qt::ItemIsSelectable);
}

// Default destructor.
PlugTreeWidgetItem::~PlugTreeWidgetItem ()
{
    int iPlug = m_pSocket->plugs().indexOf(this);
    if (iPlug >= 0)
        m_pSocket->plugs().removeAt(iPlug);
}


// Instance accessors.
const QString& PlugTreeWidgetItem::socketName () const
{
    return m_pSocket->socketName();
}

const QString& PlugTreeWidgetItem::plugName () const
{
    return m_sPlugName;
}

// Patchbay socket item accessor.
SocketTreeWidgetItem *PlugTreeWidgetItem::socket () const
{
    return m_pSocket;
}
