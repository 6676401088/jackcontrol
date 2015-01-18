/****************************************************************************
   Copyright (C) 2003-2010, rncbc aka Rui Nuno Capela. All rights reserved.

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
#include "AlsaConnect.h"
#include "mainwidget.h"

// Qt includes
#include <QPixmap>

AlsaPortTreeWidgetItem::AlsaPortTreeWidgetItem ( qjackctlAlsaClient *pClient,
	const QString& sPortName, int iAlsaPort )
	: JackPortTreeWidgetItem(pClient, /* QString::number(iAlsaPort) + ":" + */ sPortName)
{
	m_iAlsaPort = iAlsaPort;

//	qjackctlAlsaConnect *pAlsaConnect
//		= static_cast<qjackctlAlsaConnect *> (
//			((pClient->clientList())->listTreeWidget())->binding());

//	if (pAlsaConnect) {
//		if (pClient->isReadable()) {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pAlsaConnect->pixmap(QJACKCTL_ALSA_PORTO)));
//		} else {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pAlsaConnect->pixmap(QJACKCTL_ALSA_PORTI)));
//		}
//	}
}

AlsaPortTreeWidgetItem::~AlsaPortTreeWidgetItem ()
{
}

int AlsaPortTreeWidgetItem::alsaClient () const
{
	return (static_cast<qjackctlAlsaClient *> (client()))->alsaClient();
}

int AlsaPortTreeWidgetItem::alsaPort () const
{
	return m_iAlsaPort;
}

qjackctlAlsaClient::qjackctlAlsaClient ( qjackctlAlsaClientList *pClientList,
	const QString& sClientName, int iAlsaClient )
	: JackClientTreeWidgetItem(pClientList, /* QString::number(iAlsaClient) + ":" + */ sClientName)
{
	m_iAlsaClient = iAlsaClient;

//	qjackctlAlsaConnect *pAlsaConnect
//		= static_cast<qjackctlAlsaConnect *> (
//			(pClientList->listTreeWidget())->binding());

//	if (pAlsaConnect) {
//		if (pClientList->isReadable()) {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pAlsaConnect->pixmap(QJACKCTL_ALSA_CLIENTO)));
//		} else {
//			QTreeWidgetItem::setIcon(0,
//				QIcon(pAlsaConnect->pixmap(QJACKCTL_ALSA_CLIENTI)));
//		}
//	}
}

qjackctlAlsaClient::~qjackctlAlsaClient ()
{
}

int qjackctlAlsaClient::alsaClient () const
{
	return m_iAlsaClient;
}

AlsaPortTreeWidgetItem *qjackctlAlsaClient::findPort ( int iAlsaPort )
{
	QListIterator<JackPortTreeWidgetItem *> iter(ports());
	while (iter.hasNext()) {
        AlsaPortTreeWidgetItem *pPort
            = static_cast<AlsaPortTreeWidgetItem *> (iter.next());
		if (iAlsaPort == pPort->alsaPort())
			return pPort;
	}

	return NULL;
}

qjackctlAlsaClientList::qjackctlAlsaClientList (
	ClientListTreeWidget *pListView, bool bReadable )
	: JackClientList(pListView, bReadable)
{
}

qjackctlAlsaClientList::~qjackctlAlsaClientList ()
{
}

qjackctlAlsaClient *qjackctlAlsaClientList::findClient ( int iAlsaClient )
{
	QListIterator<JackClientTreeWidgetItem *> iter(clients());
	while (iter.hasNext()) {
		qjackctlAlsaClient *pClient
			= static_cast<qjackctlAlsaClient *> (iter.next());
		if (iAlsaClient == pClient->alsaClient())
			return pClient;
	}

	return NULL;
}

AlsaPortTreeWidgetItem *qjackctlAlsaClientList::findClientPort ( int iAlsaClient,
	int iAlsaPort )
{
	qjackctlAlsaClient *pClient = findClient(iAlsaClient);
	if (pClient == NULL)
		return NULL;

	return pClient->findPort(iAlsaPort);
}

int qjackctlAlsaClientList::updateClientPorts ()
{
//	MainWidget *pMainForm = MainWidget::getInstance();
//	if (pMainForm == NULL)
//		return 0;

//	snd_seq_t *pAlsaSeq = pMainForm->alsaSeq();
//	if (pAlsaSeq == NULL)
//		return 0;

//	int iDirtyCount = 0;

//	markClientPorts(0);

//#ifdef CONFIG_ALSA_SEQ

//	unsigned int uiAlsaFlags;
//	if (isReadable())
//		uiAlsaFlags = SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ;
//	else
//		uiAlsaFlags = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

//	snd_seq_client_info_t *pClientInfo;
//	snd_seq_port_info_t   *pPortInfo;

//	snd_seq_client_info_alloca(&pClientInfo);
//	snd_seq_port_info_alloca(&pPortInfo);
//	snd_seq_client_info_set_client(pClientInfo, -1);

//	while (snd_seq_query_next_client(pAlsaSeq, pClientInfo) >= 0) {

//		int iAlsaClient = snd_seq_client_info_get_client(pClientInfo);
//		if (iAlsaClient > 0) {
//			qjackctlAlsaClient *pClient = findClient(iAlsaClient);
//			snd_seq_port_info_set_client(pPortInfo, iAlsaClient);
//			snd_seq_port_info_set_port(pPortInfo, -1);
//			while (snd_seq_query_next_port(pAlsaSeq, pPortInfo) >= 0) {
//				unsigned int uiPortCapability
//					= snd_seq_port_info_get_capability(pPortInfo);
//				if (((uiPortCapability & uiAlsaFlags) == uiAlsaFlags) &&
//					((uiPortCapability & SND_SEQ_PORT_CAP_NO_EXPORT) == 0)) {
//					QString sClientName = QString::number(iAlsaClient) + ':';
//					sClientName += QString::fromUtf8(
//						snd_seq_client_info_get_name(pClientInfo));
//                    AlsaPortTreeWidgetItem *pPort = 0;
//					int iAlsaPort = snd_seq_port_info_get_port(pPortInfo);
//					if (pClient == 0) {
//						pClient = new qjackctlAlsaClient(this,
//							sClientName, iAlsaClient);
//						iDirtyCount++;
//					} else {
//						pPort = pClient->findPort(iAlsaPort);
//						if (sClientName != pClient->clientName()) {
//							pClient->setClientName(sClientName);
//							iDirtyCount++;
//						}
//					}
//					if (pClient) {
//						QString sPortName = QString::number(iAlsaPort) + ':';
//						sPortName += QString::fromUtf8(
//							snd_seq_port_info_get_name(pPortInfo));
//						if (pPort == 0) {
//                            pPort = new AlsaPortTreeWidgetItem(pClient,
//								sPortName, iAlsaPort);
//							iDirtyCount++;
//						} else if (sPortName != pPort->portName()) {
//							pPort->setPortName(sPortName);
//							iDirtyCount++;
//						}
//					}
//					if (pPort)
//						pPort->markClientPort(1);
//				}
//			}
//		}
//	}

//#endif	// CONFIG_ALSA_SEQ

//	iDirtyCount += cleanClientPorts(0);

//	return iDirtyCount;
}
