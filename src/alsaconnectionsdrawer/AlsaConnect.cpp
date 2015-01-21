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

AlsaPortTreeWidgetItem::AlsaPortTreeWidgetItem (
	const QString& sPortName, int iAlsaPort )
    : PortTreeWidgetItem(/* QString::number(iAlsaPort) + ":" + */ sPortName)
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

int AlsaPortTreeWidgetItem::alsaPort () const
{
	return m_iAlsaPort;
}

qjackctlAlsaClient::qjackctlAlsaClient ( qjackctlAlsaClientList *pClientList,
	const QString& sClientName, int iAlsaClient )
    : ClientTreeWidgetItem(sClientName)
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
//	QListIterator<PortTreeWidgetItem *> iter(ports());
//	while (iter.hasNext()) {
//        AlsaPortTreeWidgetItem *pPort
//            = static_cast<AlsaPortTreeWidgetItem *> (iter.next());
//		if (iAlsaPort == pPort->alsaPort())
//			return pPort;
//	}

	return NULL;
}
