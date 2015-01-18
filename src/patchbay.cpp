/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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

#include <Qt>

// Own includes
#include "patchbay.h"
#include "mainwidget.h"
#include "patchbaysplitter.h"

// Qt includes
#include <QMessageBox>
#include <QHeaderView>
#include <QPainterPath>
#include <QPainter>
#include <QPolygon>
#include <QPixmap>
#include <QBitmap>
#include <QRegExp>
#include <QTimer>
#include <QMenu>
#include <QToolTip>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QDrag>

// Interactivity socket form.
#include "SocketDialog.h"

// External patchbay loader.
#include "PatchbayFile.h"

Patchbay::Patchbay(PatchbaySplitter *pPatchbayView) {
	m_pPatchbayView = pPatchbayView;

    m_pOSocketList = new SocketList(m_pPatchbayView->OListView(), true);
    m_pISocketList = new SocketList(m_pPatchbayView->IListView(), false);

	m_pPatchbayView->setBinding(this);
}

Patchbay::~Patchbay() {
	m_pPatchbayView->setBinding(NULL);

	delete m_pOSocketList;
	m_pOSocketList = NULL;

	delete m_pISocketList;
	m_pISocketList = NULL;

	(m_pPatchbayView->PatchworkView())->update();
}

void Patchbay::connectSockets ( SocketTreeWidgetItem *pOSocket,
    SocketTreeWidgetItem *pISocket )
{
	if (pOSocket->findConnectPtr(pISocket) == NULL) {
		pOSocket->addConnect(pISocket);
		pISocket->addConnect(pOSocket);
	}
}

void Patchbay::disconnectSockets ( SocketTreeWidgetItem *pOSocket,
    SocketTreeWidgetItem *pISocket )
{
	if (pOSocket->findConnectPtr(pISocket) != NULL) {
		pOSocket->removeConnect(pISocket);
		pISocket->removeConnect(pOSocket);
	}
}

bool Patchbay::canConnectSelected ()
{
	QTreeWidgetItem *pOItem = (m_pOSocketList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pISocketList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

    SocketTreeWidgetItem *pOSocket = NULL;
	switch (pOItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pOSocket = static_cast<SocketTreeWidgetItem *> (pOItem);
		break;
	case QJACKCTL_PLUGITEM:
        pOSocket = (static_cast<PlugTreeWidgetItem *> (pOItem))->socket();
		break;
	default:
		return false;
	}

    SocketTreeWidgetItem *pISocket = NULL;
	switch (pIItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pISocket = static_cast<SocketTreeWidgetItem *> (pIItem);
		break;
	case QJACKCTL_PLUGITEM:
        pISocket = (static_cast<PlugTreeWidgetItem *> (pIItem))->socket();
		break;
	default:
		return false;
	}

	// Sockets must be of the same type...
	if (pOSocket->socketType() != pISocket->socketType())
		return false;

	// Exclusive sockets may not accept more than one cable.
	if (pOSocket->isExclusive() && pOSocket->connects().count() > 0)
		return false;
	if (pISocket->isExclusive() && pISocket->connects().count() > 0)
		return false;

	// One-to-one connection...
	return (pOSocket->findConnectPtr(pISocket) == NULL);
}

bool Patchbay::connectSelected ()
{
	QTreeWidgetItem *pOItem = (m_pOSocketList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pISocketList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

    SocketTreeWidgetItem *pOSocket = NULL;
	switch (pOItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pOSocket = static_cast<SocketTreeWidgetItem *> (pOItem);
		break;
	case QJACKCTL_PLUGITEM:
        pOSocket = (static_cast<PlugTreeWidgetItem *> (pOItem))->socket();
		break;
	default:
		return false;
	}

    SocketTreeWidgetItem *pISocket = NULL;
	switch (pIItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pISocket = static_cast<SocketTreeWidgetItem *> (pIItem);
		break;
	case QJACKCTL_PLUGITEM:
        pISocket = (static_cast<PlugTreeWidgetItem *> (pIItem))->socket();
		break;
	default:
		return false;
	}

	// Sockets must be of the same type...
	if (pOSocket->socketType() != pISocket->socketType())
		return false;

	// Exclusive sockets may not accept more than one cable.
	if (pOSocket->isExclusive() && pOSocket->connects().count() > 0)
		return false;
	if (pISocket->isExclusive() && pISocket->connects().count() > 0)
		return false;

	// One-to-one connection...
	connectSockets(pOSocket, pISocket);

	// Making one list dirty will take care of the rest...
	m_pPatchbayView->setDirty(true);

	return true;
}

bool Patchbay::canDisconnectSelected ()
{
	QTreeWidgetItem *pOItem = (m_pOSocketList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pISocketList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

    SocketTreeWidgetItem *pOSocket = NULL;
	switch (pOItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pOSocket = static_cast<SocketTreeWidgetItem *> (pOItem);
		break;
	case QJACKCTL_PLUGITEM:
        pOSocket = (static_cast<PlugTreeWidgetItem *> (pOItem))->socket();
		break;
	default:
		return false;
	}

    SocketTreeWidgetItem *pISocket = NULL;
	switch (pIItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pISocket = static_cast<SocketTreeWidgetItem *> (pIItem);
		break;
	case QJACKCTL_PLUGITEM:
        pISocket = (static_cast<PlugTreeWidgetItem *> (pIItem))->socket();
		break;
	default:
		return false;
	}

	// Sockets must be of the same type...
	if (pOSocket->socketType() != pISocket->socketType())
		return false;

	return (pOSocket->findConnectPtr(pISocket) != 0);
}

bool Patchbay::disconnectSelected ()
{
	QTreeWidgetItem *pOItem = (m_pOSocketList->listView())->currentItem();
	if (!pOItem)
		return false;

	QTreeWidgetItem *pIItem = (m_pISocketList->listView())->currentItem();
	if (!pIItem)
		return false;

    SocketTreeWidgetItem *pOSocket = NULL;
	switch (pOItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pOSocket = static_cast<SocketTreeWidgetItem *> (pOItem);
		break;
	case QJACKCTL_PLUGITEM:
        pOSocket = (static_cast<PlugTreeWidgetItem *> (pOItem))->socket();
		break;
	default:
		return false;
	}

    SocketTreeWidgetItem *pISocket = NULL;
	switch (pIItem->type()) {
	case QJACKCTL_SOCKETITEM:
        pISocket = static_cast<SocketTreeWidgetItem *> (pIItem);
		break;
	case QJACKCTL_PLUGITEM:
        pISocket = (static_cast<PlugTreeWidgetItem *> (pIItem))->socket();
		break;
	default:
		return false;
	}

	// Sockets must be of the same type...
	if (pOSocket->socketType() != pISocket->socketType())
		return false;

	// One-to-one disconnection...
	disconnectSockets(pOSocket, pISocket);

	// Making one list dirty will take care of the rest...
	m_pPatchbayView->setDirty(true);

	return true;
}


// Test if any plug is disconnectable.
bool Patchbay::canDisconnectAll ()
{
    QListIterator<SocketTreeWidgetItem *> osocket(m_pOSocketList->sockets());
	while (osocket.hasNext()) {
        SocketTreeWidgetItem *pOSocket = osocket.next();
		if (pOSocket->connects().count() > 0)
			return true;
	}

	return false;
}


// Disconnect all plugs.
bool Patchbay::disconnectAll ()
{
	if (QMessageBox::warning(m_pPatchbayView,
		tr("Warning") + " - " QJACKCTL_SUBTITLE1,
		tr("This will disconnect all sockets.\n\n"
		"Are you sure?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return false;
	}

    QListIterator<SocketTreeWidgetItem *> osocket(m_pOSocketList->sockets());
	while (osocket.hasNext()) {
        SocketTreeWidgetItem *pOSocket = osocket.next();
        QListIterator<SocketTreeWidgetItem *> isocket(pOSocket->connects());
		while (isocket.hasNext())
			disconnectSockets(pOSocket, isocket.next());
	}

	// Making one list dirty will take care of the rest...
	m_pPatchbayView->setDirty(true);

	return true;
}


// Expand all socket items.
void Patchbay::expandAll ()
{
	(m_pOSocketList->listView())->expandAll();
	(m_pISocketList->listView())->expandAll();
	(m_pPatchbayView->PatchworkView())->update();
}


// Complete contents rebuilder.
void Patchbay::refresh ()
{
	(m_pOSocketList->listView())->update();
	(m_pISocketList->listView())->update();
	(m_pPatchbayView->PatchworkView())->update();
}


// Complete contents clearer.
void Patchbay::clear ()
{
	// Clear socket lists.
	m_pOSocketList->clear();
	m_pISocketList->clear();

	// Reset dirty flag.
	m_pPatchbayView->setDirty(false);

	// May refresh everything.
	refresh();
}


// Patchbay client list accessors.
SocketList *Patchbay::OSocketList () const
{
	return m_pOSocketList;
}

SocketList *Patchbay::ISocketList () const
{
	return m_pISocketList;
}


void Patchbay::loadRackSockets ( SocketList *pSocketList,
    QList<qjackctlPatchbaySocket *>& socketlist )
{
	pSocketList->clear();
    SocketTreeWidgetItem *pSocketItem = NULL;

    QListIterator<qjackctlPatchbaySocket *> sockit(socketlist);
	while (sockit.hasNext()) {
        qjackctlPatchbaySocket *pSocket = sockit.next();
        pSocketItem = new SocketTreeWidgetItem(pSocketList, pSocket->name(),
			pSocket->clientName(), pSocket->type(), pSocketItem);
		if (pSocketItem) {
			pSocketItem->setExclusive(pSocket->isExclusive());
			pSocketItem->setForward(pSocket->forward());
			pSocketItem->updatePixmap();
            PlugTreeWidgetItem *pPlugItem = NULL;
			QStringListIterator iter(pSocket->pluglist());
			while (iter.hasNext()) {
                pPlugItem = new PlugTreeWidgetItem(
					pSocketItem, iter.next(), pPlugItem);
			}
		}
	}
}

void Patchbay::loadRack ( PatchbayRack *pPatchbayRack )
{
	(m_pOSocketList->listView())->setUpdatesEnabled(false);
	(m_pISocketList->listView())->setUpdatesEnabled(false);

	// Load ouput sockets.
	loadRackSockets(m_pOSocketList, pPatchbayRack->osocketlist());

	// Load input sockets.
	loadRackSockets(m_pISocketList, pPatchbayRack->isocketlist());

	// Now ready to load from cable model.
    QListIterator<qjackctlPatchbayCable *> iter(pPatchbayRack->cablelist());
	while (iter.hasNext()) {
        qjackctlPatchbayCable *pCable = iter.next();
		// Get proper sockets...
        qjackctlPatchbaySocket *pOSocket = pCable->outputSocket();
        qjackctlPatchbaySocket *pISocket = pCable->inputSocket();
		if (pOSocket && pISocket) {
            SocketTreeWidgetItem *pOSocketItem
				= m_pOSocketList->findSocket(pOSocket->name(), pOSocket->type());
            SocketTreeWidgetItem *pISocketItem
				= m_pISocketList->findSocket(pISocket->name(), pISocket->type());
			if (pOSocketItem && pISocketItem)
				connectSockets(pOSocketItem, pISocketItem);
		}
	}

	(m_pOSocketList->listView())->setUpdatesEnabled(true);
	(m_pISocketList->listView())->setUpdatesEnabled(true);

	(m_pOSocketList->listView())->update();
	(m_pISocketList->listView())->update();
	(m_pPatchbayView->PatchworkView())->update();

	// Reset dirty flag.
	m_pPatchbayView->setDirty(false);
}

void Patchbay::saveRackSockets ( SocketList *pSocketList,
    QList<qjackctlPatchbaySocket *>& socketlist )
{
	// Have QTreeWidget item order into account:
    SocketTreeWidget *pListView = pSocketList->listView();
	if (pListView == NULL)
		return;

	socketlist.clear();

	int iItemCount = pListView->topLevelItemCount();
	for (int iItem = 0; iItem < iItemCount; ++iItem) {
		QTreeWidgetItem *pItem = pListView->topLevelItem(iItem);
		if (pItem->type() != QJACKCTL_SOCKETITEM)
			continue;
        SocketTreeWidgetItem *pSocketItem
            = static_cast<SocketTreeWidgetItem *> (pItem);
		if (pSocketItem == NULL)
			continue;
        qjackctlPatchbaySocket *pSocket
            = new qjackctlPatchbaySocket(pSocketItem->socketName(),
				pSocketItem->clientName(), pSocketItem->socketType());
		if (pSocket) {
			pSocket->setExclusive(pSocketItem->isExclusive());
			pSocket->setForward(pSocketItem->forward());
            QListIterator<PlugTreeWidgetItem *> iter(pSocketItem->plugs());
			while (iter.hasNext())
				pSocket->pluglist().append((iter.next())->plugName());
			socketlist.append(pSocket);
		}
	}
}

void Patchbay::saveRack ( PatchbayRack *pPatchbayRack )
{
	// Save ouput sockets.
	saveRackSockets(m_pOSocketList, pPatchbayRack->osocketlist());

	// Save input sockets.
	saveRackSockets(m_pISocketList, pPatchbayRack->isocketlist());

	// Now ready to save into cable model.
	pPatchbayRack->cablelist().clear();

	// Start from output sockets...
    QListIterator<SocketTreeWidgetItem *> osocket(m_pOSocketList->sockets());
	while (osocket.hasNext()) {
        SocketTreeWidgetItem *pOSocketItem = osocket.next();
		// Then to input sockets...
        QListIterator<SocketTreeWidgetItem *> isocket(pOSocketItem->connects());
		while (isocket.hasNext()) {
            SocketTreeWidgetItem *pISocketItem = isocket.next();
			// Now find proper racked sockets...
            qjackctlPatchbaySocket *pOSocket = pPatchbayRack->findSocket(
				pPatchbayRack->osocketlist(), pOSocketItem->socketName());
            qjackctlPatchbaySocket *pISocket = pPatchbayRack->findSocket(
				pPatchbayRack->isocketlist(), pISocketItem->socketName());
			if (pOSocket && pISocket) {
				pPatchbayRack->addCable(
                    new qjackctlPatchbayCable(pOSocket, pISocket));
			}
		}
	}

	// Reset dirty flag.
	m_pPatchbayView->setDirty(false);
}


void Patchbay::connectionsSnapshot ()
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm == NULL)
		return;

    PatchbayRack rack;
	rack.connectJackSnapshot(pMainForm->jackClient());
    //rack.connectAlsaSnapshot(pMainForm->alsaSeq());
	loadRack(&rack);

	// Set dirty flag.
	m_pPatchbayView->setDirty(true);
}

