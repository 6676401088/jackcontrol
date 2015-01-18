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

qjackctlJackPort::qjackctlJackPort ( qjackctlJackClient *pClient,
	const QString& sPortName, jack_port_t *pJackPort )
	: JackPortTreeWidgetItem(pClient, sPortName)
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
	: JackClientTreeWidgetItem(pClientList, sClientName)
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
	QListIterator<JackPortTreeWidgetItem *> iter(ports());
	while (iter.hasNext()) {
		qjackctlJackPort *pPort
			= static_cast<qjackctlJackPort *> (iter.next());
		if (pPort && pPort->jackPort() == pJackPort)
			return pPort;
	}

	return NULL;
}

qjackctlJackClientList::qjackctlJackClientList (
	ClientListTreeWidget *pListView, bool bReadable )
	: JackClientList(pListView, bReadable)
{
}

qjackctlJackClientList::~qjackctlJackClientList ()
{
}


qjackctlJackPort *qjackctlJackClientList::findJackClientPort ( jack_port_t *pJackPort )
{
	QListIterator<JackClientTreeWidgetItem *> iter(clients());
	while (iter.hasNext()) {
		qjackctlJackClient *pClient
			= static_cast<qjackctlJackClient *> (iter.next());
		if (pClient) {
			qjackctlJackPort *pPort = pClient->findJackPort(pJackPort);
			if (pPort)
				return pPort;
		}
	}

	return NULL;
}


// Client:port refreshner.
int qjackctlJackClientList::updateClientPorts ()
{
    return 0;

//	MainWidget *pMainForm = MainWidget::getInstance();
//	if (pMainForm == NULL)
//		return 0;

//	jack_client_t *pJackClient = pMainForm->jackClient();
//	if (pJackClient == NULL)
//		return 0;

//    JackConnectionsModel *pJackConnect
//        = static_cast<JackConnectionsModel *> (listTreeWidget()->binding());
//	if (pJackConnect == NULL)
//		return 0;

//	const char *pszJackType = JACK_DEFAULT_AUDIO_TYPE;
//#ifdef CONFIG_JACK_MIDI
//	if (pJackConnect->jackType() == QJACKCTL_JACK_MIDI)
//		pszJackType = JACK_DEFAULT_MIDI_TYPE;
//#endif

//	char *aliases[2];
//	if (g_iJackClientPortAlias > 0) {
//		unsigned short alias_size = jack_port_name_size() + 1;
//		aliases[0] = new char [alias_size];
//		aliases[1] = new char [alias_size];
//	}

//	int iDirtyCount = 0;

//	markClientPorts(0);

//	const char **ppszClientPorts = jack_get_ports(pJackClient, 0,
//		pszJackType, isReadable() ? JackPortIsOutput : JackPortIsInput);
//	if (ppszClientPorts) {
//		int iClientPort = 0;
//		while (ppszClientPorts[iClientPort]) {
//			QString sClientPort = QString::fromUtf8(ppszClientPorts[iClientPort]);
//			qjackctlJackClient *pClient = 0;
//			qjackctlJackPort   *pPort   = 0;
//			jack_port_t *pJackPort = jack_port_by_name(pJackClient,
//				ppszClientPorts[iClientPort]);
//		#ifdef CONFIG_JACK_PORT_ALIASES
//			if (g_iJackClientPortAlias > 0 &&
//				jack_port_get_aliases(pJackPort, aliases) >= g_iJackClientPortAlias)
//				sClientPort = QString::fromUtf8(aliases[g_iJackClientPortAlias - 1]);
//		#endif
//			int iColon = sClientPort.indexOf(':');
//			if (pJackPort && iColon >= 0) {
//				QString sClientName = sClientPort.left(iColon);
//				QString sPortName   = sClientPort.right(sClientPort.length() - iColon - 1);
//				pClient = static_cast<qjackctlJackClient *> (findClient(sClientName));
//				if (pClient)
//					pPort = static_cast<qjackctlJackPort *> (pClient->findPort(sPortName));
//				if (pClient == 0) {
//					pClient = new qjackctlJackClient(this, sClientName);
//					iDirtyCount++;
//				}
//				if (pClient && pPort == 0) {
//					pPort = new qjackctlJackPort(pClient, sPortName, pJackPort);
//					iDirtyCount++;
//				}
//				if (pPort)
//					pPort->markClientPort(1);
//			}
//			iClientPort++;
//		}
//		::free(ppszClientPorts);
//	}

//	iDirtyCount += cleanClientPorts(0);

//	if (g_iJackClientPortAlias > 0) {
//		delete [] aliases[0];
//		delete [] aliases[1];
//	}

//	return iDirtyCount;
}


// Jack client port aliases mode.
int qjackctlJackClientList::g_iJackClientPortAlias = 0;

void qjackctlJackClientList::setJackClientPortAlias ( int iJackClientPortAlias )
{
	g_iJackClientPortAlias = iJackClientPortAlias;
}

int qjackctlJackClientList::jackClientPortAlias ()
{
	return g_iJackClientPortAlias;
}
