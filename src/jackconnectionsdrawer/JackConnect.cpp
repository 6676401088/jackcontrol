/****************************************************************************
   Copyright (C) 2003-2010, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include "JackConnect.h"
#include "mainwidget.h"

// Qt includes
#include <QPixmap>

qjackctlJackPort::qjackctlJackPort (
	const QString& sPortName, jack_port_t *pJackPort )
    : PortTreeWidgetItem(sPortName)
{
	m_pJackPort = pJackPort;

//    JackConnectionsModel *pJackConnect
//        = static_cast<JackConnectionsModel *> (
//			((pClient->clientList())->listTreeWidget())->binding());

//	if (pJackConnect) {
//		unsigned long ulPortFlags = jack_port_flags(m_pJackPort);
//		if (ulPortFlags & JackPortIsInput) {
//			if (ulPortFlags & JackPortIsTerminal) {
//				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
//					ulPortFlags & JackPortIsPhysical
//						? QJACKCTL_JACK_PORTPTI : QJACKCTL_JACK_PORTLTI)));
//			} else {
//				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
//					ulPortFlags & JackPortIsPhysical
//						? QJACKCTL_JACK_PORTPNI : QJACKCTL_JACK_PORTLNI)));
//			}
//		} else if (ulPortFlags & JackPortIsOutput) {
//			if (ulPortFlags & JackPortIsTerminal) {
//				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
//					ulPortFlags & JackPortIsPhysical
//						? QJACKCTL_JACK_PORTPTO : QJACKCTL_JACK_PORTLTO)));
//			} else {
//				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
//					ulPortFlags & JackPortIsPhysical
//						? QJACKCTL_JACK_PORTPNO : QJACKCTL_JACK_PORTLNO)));
//			}
//		}
//	}
}

qjackctlJackPort::~qjackctlJackPort ()
{
}

jack_port_t *qjackctlJackPort::jackPort () const
{
	return m_pJackPort;
}

qjackctlJackClient::qjackctlJackClient ( qjackctlJackClientList *pClientList,
	const QString& sClientName )
    : ClientTreeWidgetItem(sClientName)
{
//    JackConnectionsModel *pJackConnect
//        = static_cast<JackConnectionsModel *> (
//			(pClientList->listTreeWidget())->binding());

//	if (pJackConnect) {
//		if (pClientList->isReadable()) {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pJackConnect->pixmap(QJACKCTL_JACK_CLIENTO)));
//		} else {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pJackConnect->pixmap(QJACKCTL_JACK_CLIENTI)));
//		}
//	}
}

qjackctlJackClient::~qjackctlJackClient ()
{
}

qjackctlJackPort *qjackctlJackClient::findJackPort ( jack_port_t *pJackPort )
{
//	QListIterator<PortTreeWidgetItem *> iter(ports());
//	while (iter.hasNext()) {
//		qjackctlJackPort *pPort
//			= static_cast<qjackctlJackPort *> (iter.next());
//		if (pPort && pPort->jackPort() == pJackPort)
//			return pPort;
//	}

	return NULL;
}
