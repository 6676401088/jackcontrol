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
#include "MainWidget.h"

// Qt includes
#include <QPixmap>

qjackctlJackPort::qjackctlJackPort ( qjackctlJackClient *pClient,
	const QString& sPortName, jack_port_t *pJackPort )
	: JackPortTreeWidgetItem(pClient, sPortName)
{
	m_pJackPort = pJackPort;

    JackConnectionsModel *pJackConnect
        = static_cast<JackConnectionsModel *> (
			((pClient->clientList())->listView())->binding());

	if (pJackConnect) {
		unsigned long ulPortFlags = jack_port_flags(m_pJackPort);
		if (ulPortFlags & JackPortIsInput) {
			if (ulPortFlags & JackPortIsTerminal) {
				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
					ulPortFlags & JackPortIsPhysical
						? QJACKCTL_JACK_PORTPTI : QJACKCTL_JACK_PORTLTI)));
			} else {
				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
					ulPortFlags & JackPortIsPhysical
						? QJACKCTL_JACK_PORTPNI : QJACKCTL_JACK_PORTLNI)));
			}
		} else if (ulPortFlags & JackPortIsOutput) {
			if (ulPortFlags & JackPortIsTerminal) {
				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
					ulPortFlags & JackPortIsPhysical
						? QJACKCTL_JACK_PORTPTO : QJACKCTL_JACK_PORTLTO)));
			} else {
				QTreeWidgetItem::setIcon(0, QIcon(pJackConnect->pixmap(
					ulPortFlags & JackPortIsPhysical
						? QJACKCTL_JACK_PORTPNO : QJACKCTL_JACK_PORTLNO)));
			}
		}
	}
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
    JackConnectionsModel *pJackConnect
        = static_cast<JackConnectionsModel *> (
			(pClientList->listView())->binding());

	if (pJackConnect) {
		if (pClientList->isReadable()) {
			QTreeWidgetItem::setIcon(0,
				QIcon(pJackConnect->pixmap(QJACKCTL_JACK_CLIENTO)));
		} else {
			QTreeWidgetItem::setIcon(0,
				QIcon(pJackConnect->pixmap(QJACKCTL_JACK_CLIENTI)));
		}
	}
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
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return 0;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return 0;

    JackConnectionsModel *pJackConnect
        = static_cast<JackConnectionsModel *> (listView()->binding());
	if (pJackConnect == NULL)
		return 0;

	const char *pszJackType = JACK_DEFAULT_AUDIO_TYPE;
#ifdef CONFIG_JACK_MIDI
	if (pJackConnect->jackType() == QJACKCTL_JACK_MIDI)
		pszJackType = JACK_DEFAULT_MIDI_TYPE;
#endif

	char *aliases[2];
	if (g_iJackClientPortAlias > 0) {
		unsigned short alias_size = jack_port_name_size() + 1;
		aliases[0] = new char [alias_size];
		aliases[1] = new char [alias_size];
	}

	int iDirtyCount = 0;

	markClientPorts(0);

	const char **ppszClientPorts = jack_get_ports(pJackClient, 0,
		pszJackType, isReadable() ? JackPortIsOutput : JackPortIsInput);
	if (ppszClientPorts) {
		int iClientPort = 0;
		while (ppszClientPorts[iClientPort]) {
			QString sClientPort = QString::fromUtf8(ppszClientPorts[iClientPort]);
			qjackctlJackClient *pClient = 0;
			qjackctlJackPort   *pPort   = 0;
			jack_port_t *pJackPort = jack_port_by_name(pJackClient,
				ppszClientPorts[iClientPort]);
		#ifdef CONFIG_JACK_PORT_ALIASES
			if (g_iJackClientPortAlias > 0 &&
				jack_port_get_aliases(pJackPort, aliases) >= g_iJackClientPortAlias)
				sClientPort = QString::fromUtf8(aliases[g_iJackClientPortAlias - 1]);
		#endif
			int iColon = sClientPort.indexOf(':');
			if (pJackPort && iColon >= 0) {
				QString sClientName = sClientPort.left(iColon);
				QString sPortName   = sClientPort.right(sClientPort.length() - iColon - 1);
				pClient = static_cast<qjackctlJackClient *> (findClient(sClientName));
				if (pClient)
					pPort = static_cast<qjackctlJackPort *> (pClient->findPort(sPortName));
				if (pClient == 0) {
					pClient = new qjackctlJackClient(this, sClientName);
					iDirtyCount++;
				}
				if (pClient && pPort == 0) {
					pPort = new qjackctlJackPort(pClient, sPortName, pJackPort);
					iDirtyCount++;
				}
				if (pPort)
					pPort->markClientPort(1);
			}
			iClientPort++;
		}
		::free(ppszClientPorts);
	}

	iDirtyCount += cleanClientPorts(0);

	if (g_iJackClientPortAlias > 0) {
		delete [] aliases[0];
		delete [] aliases[1];
	}

	return iDirtyCount;
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

JackConnectionsModel::JackConnectionsModel (
	ConnectionsViewSplitter *pConnectView, int iJackType  )
	: ConnectionsModel(pConnectView)
{
	m_iJackType = iJackType;

	createIconPixmaps();

	setOClientList(new qjackctlJackClientList(
		connectView()->OListView(), true));
	setIClientList(new qjackctlJackClientList(
		connectView()->IListView(), false));
}

JackConnectionsModel::~JackConnectionsModel ()
{
	deleteIconPixmaps();
}

int JackConnectionsModel::jackType () const
{
	return m_iJackType;
}

void JackConnectionsModel::createIconPixmaps ()
{
	switch (m_iJackType) {
	case QJACKCTL_JACK_MIDI:
		m_apPixmaps[QJACKCTL_JACK_CLIENTI] = createIconPixmap("mclienti");
		m_apPixmaps[QJACKCTL_JACK_CLIENTO] = createIconPixmap("mcliento");
		m_apPixmaps[QJACKCTL_JACK_PORTPTI] = createIconPixmap("mporti");
		m_apPixmaps[QJACKCTL_JACK_PORTPTO] = createIconPixmap("mporto");
		m_apPixmaps[QJACKCTL_JACK_PORTPNI] = createIconPixmap("mporti");
		m_apPixmaps[QJACKCTL_JACK_PORTPNO] = createIconPixmap("mporto");
		m_apPixmaps[QJACKCTL_JACK_PORTLTI] = createIconPixmap("mporti");
		m_apPixmaps[QJACKCTL_JACK_PORTLTO] = createIconPixmap("mporto");
		m_apPixmaps[QJACKCTL_JACK_PORTLNI] = createIconPixmap("mporti");
		m_apPixmaps[QJACKCTL_JACK_PORTLNO] = createIconPixmap("mporto");
		break;
	case QJACKCTL_JACK_AUDIO:
	default:
		m_apPixmaps[QJACKCTL_JACK_CLIENTI] = createIconPixmap("aclienti");
		m_apPixmaps[QJACKCTL_JACK_CLIENTO] = createIconPixmap("acliento");
		m_apPixmaps[QJACKCTL_JACK_PORTPTI] = createIconPixmap("aportpti");
		m_apPixmaps[QJACKCTL_JACK_PORTPTO] = createIconPixmap("aportpto");
		m_apPixmaps[QJACKCTL_JACK_PORTPNI] = createIconPixmap("aportpni");
		m_apPixmaps[QJACKCTL_JACK_PORTPNO] = createIconPixmap("aportpno");
		m_apPixmaps[QJACKCTL_JACK_PORTLTI] = createIconPixmap("aportlti");
		m_apPixmaps[QJACKCTL_JACK_PORTLTO] = createIconPixmap("aportlto");
		m_apPixmaps[QJACKCTL_JACK_PORTLNI] = createIconPixmap("aportlni");
		m_apPixmaps[QJACKCTL_JACK_PORTLNO] = createIconPixmap("aportlno");
		break;
	}
}

void JackConnectionsModel::deleteIconPixmaps ()
{
	for (int i = 0; i < QJACKCTL_JACK_PIXMAPS; i++) {
		if (m_apPixmaps[i])
			delete m_apPixmaps[i];
		m_apPixmaps[i] = NULL;
	}
}


// Common pixmap accessor (static).
const QPixmap& JackConnectionsModel::pixmap ( int iPixmap ) const
{
	return *m_apPixmaps[iPixmap];
}


bool JackConnectionsModel::connectPorts ( JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return false;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return false;

	qjackctlJackPort *pOJack = static_cast<qjackctlJackPort *> (pOPort);
	qjackctlJackPort *pIJack = static_cast<qjackctlJackPort *> (pIPort);

	return (jack_connect(pJackClient,
		pOJack->clientPortName().toUtf8().constData(),
		pIJack->clientPortName().toUtf8().constData()) == 0);
}

bool JackConnectionsModel::disconnectPorts ( JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return false;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return false;

	qjackctlJackPort *pOJack = static_cast<qjackctlJackPort *> (pOPort);
	qjackctlJackPort *pIJack = static_cast<qjackctlJackPort *> (pIPort);

	return (jack_disconnect(pJackClient,
		pOJack->clientPortName().toUtf8().constData(),
		pIJack->clientPortName().toUtf8().constData()) == 0);
}


// Update port connection references.
void JackConnectionsModel::updateConnections ()
{
	MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return;

	jack_client_t *pJackClient = pMainForm->jackClient();
	if (pJackClient == NULL)
		return;

	qjackctlJackClientList *pIClientList
		= static_cast<qjackctlJackClientList *> (IClientList());
	if (pIClientList == NULL)
		return;

	// For each output client item...
	QListIterator<JackClientTreeWidgetItem *> oclient(OClientList()->clients());
	while (oclient.hasNext()) {
		JackClientTreeWidgetItem *pOClient = oclient.next();
		// For each output port item...
		QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
		while (oport.hasNext()) {
			JackPortTreeWidgetItem *pOPort = oport.next();
			// Are there already any connections?
			if (pOPort->connects().count() > 0)
				continue;
			// Hava a proper type cast.
			qjackctlJackPort *pOJack
				= static_cast<qjackctlJackPort *> (pOPort);
			// Get port connections...
			const char **ppszClientPorts
				= jack_port_get_all_connections(
					pJackClient, pOJack->jackPort());
			if (ppszClientPorts) {
				// Now, for each input client port...
				int iClientPort = 0;
				while (ppszClientPorts[iClientPort]) {
					jack_port_t *pJackPort = jack_port_by_name(
						pJackClient, ppszClientPorts[iClientPort]);
					if (pJackPort) {
						JackPortTreeWidgetItem *pIPort
							= pIClientList->findJackClientPort(pJackPort);
						if (pIPort) {
							pOPort->addConnect(pIPort);
							pIPort->addConnect(pOPort);
						}
					}
					iClientPort++;
				}
				::free(ppszClientPorts);
			}
		}
	}
}


// Update icon size implementation.
void JackConnectionsModel::updateIconPixmaps ()
{
	deleteIconPixmaps();
	createIconPixmaps();
}
