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
#include "About.h"
#include "ConnectViewSplitter.h"

// Qt includes
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolTip>
#include <QPainter>
#include <QPolygon>
#include <QPainterPath>
#include <QTimer>
#include <QMenu>
#include <QMimeData>
#include <QDrag>
#include <QHelpEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

JackPortTreeWidgetItem::JackPortTreeWidgetItem ( JackClientTreeWidgetItem *pClient,
	const QString& sPortName )
	: QTreeWidgetItem(pClient, QJACKCTL_PORTITEM)
{
	m_pClient   = pClient;
	m_sPortName = sPortName;
	m_iPortMark = 0;
	m_bHilite   = false;

	m_pClient->ports().append(this);

	// Check aliasing...
	qjackctlConnectAlias *pAliases
		= ((pClient->clientList())->listView())->aliases();
	if (pAliases) {
		QTreeWidgetItem::setText(0,
			pAliases->portAlias(pClient->clientName(), sPortName));
		if (((pClient->clientList())->listView())->renameEnabled()) {
			QTreeWidgetItem::setFlags(QTreeWidgetItem::flags()
				| Qt::ItemIsEditable);
		}
	} else {
		QTreeWidgetItem::setText(0, sPortName);
	}
}

JackPortTreeWidgetItem::~JackPortTreeWidgetItem ()
{
	int iPort = m_pClient->ports().indexOf(this);
	if (iPort >= 0)
		m_pClient->ports().removeAt(iPort);

    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
	while (iter.hasNext())
		(iter.next())->removeConnect(this);

	m_connects.clear();
}

void JackPortTreeWidgetItem::setPortName ( const QString& sPortName )
{
	QTreeWidgetItem::setText(0, sPortName);

	m_sPortName = sPortName;
}

const QString& JackPortTreeWidgetItem::clientName () const
{
	return m_pClient->clientName();
}

const QString& JackPortTreeWidgetItem::portName () const
{
	return m_sPortName;
}

QString JackPortTreeWidgetItem::clientPortName () const
{
	return m_pClient->clientName() + ':' + m_sPortName;
}

JackClientTreeWidgetItem *JackPortTreeWidgetItem::client () const
{
	return m_pClient;
}

void JackPortTreeWidgetItem::markPort ( int iMark )
{
	setHilite(false);
	m_iPortMark = iMark;
	if (iMark > 0)
		m_connects.clear();
}

void JackPortTreeWidgetItem::markClientPort ( int iMark )
{
	markPort(iMark);

	m_pClient->markClient(iMark);
}

int JackPortTreeWidgetItem::portMark () const
{
	return m_iPortMark;
}

void JackPortTreeWidgetItem::addConnect ( JackPortTreeWidgetItem *pPort )
{
	m_connects.append(pPort);
}

void JackPortTreeWidgetItem::removeConnect ( JackPortTreeWidgetItem *pPort )
{
	pPort->setHilite(false);

	int iPort = m_connects.indexOf(pPort);
	if (iPort >= 0)
		m_connects.removeAt(iPort);
}

JackPortTreeWidgetItem *JackPortTreeWidgetItem::findConnect ( const QString& sClientPortName )
{
    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
	while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
		if (sClientPortName == pPort->clientPortName())
			return pPort;
	}

	return NULL;
}

JackPortTreeWidgetItem *JackPortTreeWidgetItem::findConnectPtr ( JackPortTreeWidgetItem *pPortPtr )
{
    QListIterator<JackPortTreeWidgetItem *> iter(m_connects);
	while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
		if (pPortPtr == pPort)
			return pPort;
	}

	return NULL;
}

const QList<JackPortTreeWidgetItem *>& JackPortTreeWidgetItem::connects () const
{
	return m_connects;
}

bool JackPortTreeWidgetItem::isHilite () const
{
	return m_bHilite;
}

void JackPortTreeWidgetItem::setHilite ( bool bHilite )
{
	// Update the port highlightning if changed...
	if ((m_bHilite && !bHilite) || (!m_bHilite && bHilite)) {
		m_bHilite = bHilite;
		// Propagate this to the parent...
		m_pClient->setHilite(bHilite);
	}

	// Set the new color.
	QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
	if (pTreeWidget == NULL)
		return;

	const QPalette& pal = pTreeWidget->palette();
	QTreeWidgetItem::setTextColor(0, m_bHilite
		? (pal.base().color().value() < 0x7f ? Qt::cyan : Qt::blue)
		: pal.text().color());
}

bool JackPortTreeWidgetItem::operator< ( const QTreeWidgetItem& other ) const
{
    return JackClientList::lessThan(*this, other);
}

JackClientTreeWidgetItem::JackClientTreeWidgetItem ( JackClientList *pClientList,
	const QString& sClientName )
	: QTreeWidgetItem(pClientList->listView(), QJACKCTL_CLIENTITEM)
{
	m_pClientList = pClientList;
	m_sClientName = sClientName;
	m_iClientMark = 0;
	m_iHilite     = 0;

	m_pClientList->clients().append(this);

	// Check aliasing...
	qjackctlConnectAlias *pAliases
		= (pClientList->listView())->aliases();
	if (pAliases) {
		QTreeWidgetItem::setText(0,
			pAliases->clientAlias(sClientName));
		if ((pClientList->listView())->renameEnabled()) {
			QTreeWidgetItem::setFlags(QTreeWidgetItem::flags()
				| Qt::ItemIsEditable);
		}
	} else {
		QTreeWidgetItem::setText(0, sClientName);
	}
}

JackClientTreeWidgetItem::~JackClientTreeWidgetItem ()
{
	qDeleteAll(m_ports);
	m_ports.clear();

	int iClient = m_pClientList->clients().indexOf(this);
	if (iClient >= 0)
		m_pClientList->clients().removeAt(iClient);
}

JackPortTreeWidgetItem *JackClientTreeWidgetItem::findPort (const QString& sPortName)
{
    QListIterator<JackPortTreeWidgetItem *> iter(m_ports);
	while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
		if (sPortName == pPort->portName())
			return pPort;
	}

	return NULL;
}


// Client list accessor.
JackClientList *JackClientTreeWidgetItem::clientList () const
{
	return m_pClientList;
}


// Port list accessor.
QList<JackPortTreeWidgetItem *>& JackClientTreeWidgetItem::ports ()
{
	return m_ports;
}

void JackClientTreeWidgetItem::setClientName ( const QString& sClientName )
{
	QTreeWidgetItem::setText(0, sClientName);

	m_sClientName = sClientName;
}

const QString& JackClientTreeWidgetItem::clientName () const
{
	return m_sClientName;
}

bool JackClientTreeWidgetItem::isReadable () const
{
	return m_pClientList->isReadable();
}

void JackClientTreeWidgetItem::markClient ( int iMark )
{
	setHilite(false);
	m_iClientMark = iMark;
}

void JackClientTreeWidgetItem::markClientPorts ( int iMark )
{
	markClient(iMark);

    QListIterator<JackPortTreeWidgetItem *> iter(m_ports);
	while (iter.hasNext())
		(iter.next())->markPort(iMark);
}

int JackClientTreeWidgetItem::cleanClientPorts ( int iMark )
{
	int iDirtyCount = 0;

    QMutableListIterator<JackPortTreeWidgetItem *> iter(m_ports);
	while (iter.hasNext()) {
        JackPortTreeWidgetItem *pPort = iter.next();
		if (pPort->portMark() == iMark) {
			iter.remove();
			delete pPort;
			iDirtyCount++;
		}
	}
	
	return iDirtyCount;
}

int JackClientTreeWidgetItem::clientMark () const
{
	return m_iClientMark;
}

bool JackClientTreeWidgetItem::isHilite () const
{
	return (m_iHilite > 0);
}

void JackClientTreeWidgetItem::setHilite ( bool bHilite )
{
	if (bHilite)
		m_iHilite++;
	else
	if (m_iHilite > 0)
		m_iHilite--;

	// Set the new color.
	QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
	if (pTreeWidget == NULL)
		return;

	const QPalette& pal = pTreeWidget->palette();
	QTreeWidgetItem::setTextColor(0, m_iHilite > 0
		? (pal.base().color().value() < 0x7f ? Qt::darkCyan : Qt::darkBlue)
		: pal.text().color());
}

void JackClientTreeWidgetItem::setOpen ( bool bOpen )
{
	QTreeWidgetItem::setExpanded(bOpen);
}


bool JackClientTreeWidgetItem::isOpen () const
{
	return QTreeWidgetItem::isExpanded();
}

bool JackClientTreeWidgetItem::operator< ( const QTreeWidgetItem& other ) const
{
    return JackClientList::lessThan(*this, other);
}

JackClientList::JackClientList (
    ClientListTreeWidget *pListView, bool bReadable )
{
	m_pListView = pListView;
	m_bReadable = bReadable;

	m_pHiliteItem = 0;
}

JackClientList::~JackClientList ()
{
	clear();
}

void JackClientList::clear ()
{
	qDeleteAll(m_clients);
	m_clients.clear();

	if (m_pListView)
		m_pListView->clear();
}

JackClientTreeWidgetItem *JackClientList::findClient (
	const QString& sClientName )
{
    QListIterator<JackClientTreeWidgetItem *> iter(m_clients);
	while (iter.hasNext()) {
        JackClientTreeWidgetItem *pClient = iter.next();
		if (sClientName == pClient->clientName())
			return pClient;
	}

	return NULL;
}

JackPortTreeWidgetItem *JackClientList::findClientPort (
	const QString& sClientPort )
{
    JackPortTreeWidgetItem *pPort = 0;
	int iColon = sClientPort.indexOf(':');
	if (iColon >= 0) {
        JackClientTreeWidgetItem *pClient = findClient(sClientPort.left(iColon));
		if (pClient) {
			pPort = pClient->findPort(
				sClientPort.right(sClientPort.length() - iColon - 1));
		}
	}
	return pPort;
}

QList<JackClientTreeWidgetItem *>& JackClientList::clients ()
{
	return m_clients;
}

ClientListTreeWidget *JackClientList::listView () const
{
	return m_pListView;
}

bool JackClientList::isReadable () const
{
	return m_bReadable;
}

void JackClientList::markClientPorts ( int iMark )
{
	m_pHiliteItem = 0;

    QListIterator<JackClientTreeWidgetItem *> iter(m_clients);
	while (iter.hasNext())
		(iter.next())->markClientPorts(iMark);
}

int JackClientList::cleanClientPorts ( int iMark )
{
	int iDirtyCount = 0;
	
    QMutableListIterator<JackClientTreeWidgetItem *> iter(m_clients);
	while (iter.hasNext()) {
        JackClientTreeWidgetItem *pClient = iter.next();
		if (pClient->clientMark() == iMark) {
			iter.remove();
			delete pClient;
			iDirtyCount++;
		} else {
			iDirtyCount += pClient->cleanClientPorts(iMark);
		}
	}

	return iDirtyCount;
}

void JackClientList::hiliteClientPorts ()
{
    JackClientTreeWidgetItem *pClient;
    JackPortTreeWidgetItem *pPort;

	QTreeWidgetItem *pItem = m_pListView->currentItem();

	// Dehilite the previous selected items.
	if (m_pHiliteItem && pItem != m_pHiliteItem) {
		if (m_pHiliteItem->type() == QJACKCTL_CLIENTITEM) {
            pClient = static_cast<JackClientTreeWidgetItem *> (m_pHiliteItem);
            QListIterator<JackPortTreeWidgetItem *> iter(pClient->ports());
			while (iter.hasNext()) {
				pPort = iter.next();
                QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
				while (it.hasNext())
					(it.next())->setHilite(false);
			}
		} else {
            pPort = static_cast<JackPortTreeWidgetItem *> (m_pHiliteItem);
            QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
			while (it.hasNext())
				(it.next())->setHilite(false);
		}
	}

	// Hilite the now current selected items.
	if (pItem) {
		if (pItem->type() == QJACKCTL_CLIENTITEM) {
            pClient = static_cast<JackClientTreeWidgetItem *> (pItem);
            QListIterator<JackPortTreeWidgetItem *> iter(pClient->ports());
			while (iter.hasNext()) {
				pPort = iter.next();
                QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
				while (it.hasNext())
					(it.next())->setHilite(true);
			}
		} else {
            pPort = static_cast<JackPortTreeWidgetItem *> (pItem);
            QListIterator<JackPortTreeWidgetItem *> it(pPort->connects());
			while (it.hasNext())
				(it.next())->setHilite(true);
		}
	}

	// Do remember this one, ever.
	m_pHiliteItem = pItem;
}

bool JackClientList::lessThan (
	const QTreeWidgetItem& i1, const QTreeWidgetItem& i2 )
{
	const QString& s1 = i1.text(0);
	const QString& s2 = i2.text(0);

	int ich1, ich2;

	int cch1 = s1.length();
	int cch2 = s2.length();

	for (ich1 = ich2 = 0; ich1 < cch1 && ich2 < cch2; ich1++, ich2++) {

		// Skip (white)spaces...
		while (s1.at(ich1).isSpace())
			ich1++;
		while (s2.at(ich2).isSpace())
			ich2++;

		// Normalize (to uppercase) the next characters...
		QChar ch1 = s1.at(ich1).toUpper();
		QChar ch2 = s2.at(ich2).toUpper();

		if (ch1.isDigit() && ch2.isDigit()) {
			// Find the whole length numbers...
			int iDigits1 = ich1++;
			while (ich1 < cch1 && s1.at(ich1).isDigit())
				ich1++;
			int iDigits2 = ich2++;
			while (ich2 < cch2 && s2.at(ich2).isDigit())
				ich2++;
			// Compare as natural decimal-numbers...
			int n1 = s1.mid(iDigits1, ich1 - iDigits1).toInt();
			int n2 = s2.mid(iDigits2, ich2 - iDigits2).toInt();
			if (n1 != n2)
				return (n1 < n2);
			// Never go out of bounds...
			if (ich1 >= cch1 || ich1 >= cch2)
				break;
			// Go on with this next char...
			ch1 = s1.at(ich1).toUpper();
			ch2 = s2.at(ich2).toUpper();
		}

		// Compare this char...
		if (ch1 != ch2)
			return (ch1 < ch2);
	}

	// Probable exact match.
	return false;
}

void JackClientList::refresh ()
{
	QHeaderView *pHeader = m_pListView->header();
	m_pListView->sortItems(
		pHeader->sortIndicatorSection(),
		pHeader->sortIndicatorOrder());
}

ClientListTreeWidget::ClientListTreeWidget (
    ConnectionsViewSplitter *pConnectView, bool bReadable )
	: QTreeWidget(pConnectView)
{
	m_pConnectView = pConnectView;

	m_pAutoOpenTimer   = 0;
	m_iAutoOpenTimeout = 0;

	m_pDragItem = NULL;
	m_pDragItem = NULL;

	m_pAliases = NULL;
	m_bRenameEnabled = false;

	QHeaderView *pHeader = QTreeWidget::header();
	pHeader->setDefaultAlignment(Qt::AlignLeft);
//	pHeader->setDefaultSectionSize(120);
#if QT_VERSION >= 0x050000
//	pHeader->setSectionResizeMode(QHeaderView::Custom);
	pHeader->setSectionsMovable(false);
	pHeader->setSectionsClickable(true);
#else
//	pHeader->setResizeMode(QHeaderView::Custom);
	pHeader->setMovable(false);
	pHeader->setClickable(true);
#endif
	pHeader->setSortIndicatorShown(true);
	pHeader->setStretchLastSection(true);

	QTreeWidget::setRootIsDecorated(true);
	QTreeWidget::setUniformRowHeights(true);
//	QTreeWidget::setDragEnabled(true);
	QTreeWidget::setAcceptDrops(true);
	QTreeWidget::setDropIndicatorShown(true);
	QTreeWidget::setAutoScroll(true);
	QTreeWidget::setSelectionMode(QAbstractItemView::SingleSelection);
	QTreeWidget::setSizePolicy(
		QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QTreeWidget::setSortingEnabled(true);
	QTreeWidget::setMinimumWidth(120);
	QTreeWidget::setColumnCount(1);

	QString sText;
	if (bReadable)
		sText = tr("Readable Clients / Output Ports");
	else
		sText = tr("Writable Clients / Input Ports");
	QTreeWidget::headerItem()->setText(0, sText);
	QTreeWidget::sortItems(0, Qt::AscendingOrder);
	QTreeWidget::setToolTip(sText);

	// Trap for help/tool-tips events.
	QTreeWidget::viewport()->installEventFilter(this);

	QObject::connect(QTreeWidget::itemDelegate(),
		SIGNAL(commitData(QWidget*)),
		SLOT(renamedSlot()));

	setAutoOpenTimeout(800);
}

ClientListTreeWidget::~ClientListTreeWidget ()
{
	setAutoOpenTimeout(0);
}

ConnectionsModel *ClientListTreeWidget::binding() const
{
	return m_pConnectView->binding();
}

void ClientListTreeWidget::setAutoOpenTimeout ( int iAutoOpenTimeout )
{
	m_iAutoOpenTimeout = iAutoOpenTimeout;

	if (m_pAutoOpenTimer)
		delete m_pAutoOpenTimer;
	m_pAutoOpenTimer = NULL;

	if (m_iAutoOpenTimeout > 0) {
		m_pAutoOpenTimer = new QTimer(this);
		QObject::connect(m_pAutoOpenTimer,
			SIGNAL(timeout()),
			SLOT(timeoutSlot()));
	}
}

int ClientListTreeWidget::autoOpenTimeout () const
{
	return m_iAutoOpenTimeout;
}

void ClientListTreeWidget::setAliases ( qjackctlConnectAlias *pAliases, bool bRenameEnabled )
{
	m_pAliases = pAliases;
	m_bRenameEnabled = bRenameEnabled;

	// For each client item, if any.
	int iItemCount = QTreeWidget::topLevelItemCount();
	for (int iItem = 0; iItem < iItemCount; ++iItem) {
		QTreeWidgetItem *pItem = QTreeWidget::topLevelItem(iItem);
		if (pItem->type() != QJACKCTL_CLIENTITEM)
			continue;
        JackClientTreeWidgetItem *pClientItem
            = static_cast<JackClientTreeWidgetItem *> (pItem);
		if (pClientItem == NULL)
			continue;
		if (m_pAliases) {
			pClientItem->setText(0,
				m_pAliases->clientAlias(pClientItem->clientName()));
		} else {
			pClientItem->setText(0,
				pClientItem->clientName());
		}
		if (m_pAliases && m_bRenameEnabled) {
			pClientItem->setFlags(
				pClientItem->flags() |  Qt::ItemIsEditable);
		} else {
			pClientItem->setFlags(
				pClientItem->flags() & ~Qt::ItemIsEditable);
		}
		// For each port item...
		int iChildCount = pClientItem->childCount();
		for (int iChild = 0; iChild < iChildCount; ++iChild) {
			QTreeWidgetItem *pChildItem = pClientItem->child(iChild);
			if (pChildItem->type() != QJACKCTL_PORTITEM)
				continue;
            JackPortTreeWidgetItem *pPortItem
                = static_cast<JackPortTreeWidgetItem *> (pChildItem);
			if (pPortItem == NULL)
				continue;
			if (m_pAliases) {
				pPortItem->setText(0, m_pAliases->portAlias(
					pPortItem->clientName(), pPortItem->portName()));
			} else {
				pPortItem->setText(0,
					pPortItem->portName());
			}
			if (m_pAliases && m_bRenameEnabled) {
				pPortItem->setFlags(
					pPortItem->flags() |  Qt::ItemIsEditable);
			} else {
				pPortItem->setFlags(
					pPortItem->flags() & ~Qt::ItemIsEditable);
			}
		}
	}
}

qjackctlConnectAlias *ClientListTreeWidget::aliases () const
{
	return m_pAliases;
}

bool ClientListTreeWidget::renameEnabled () const
{
	return m_bRenameEnabled;
}

void ClientListTreeWidget::startRenameSlot ()
{
	QTreeWidgetItem *pItem = QTreeWidget::currentItem();
	if (pItem)
		QTreeWidget::editItem(pItem, 0);
}

void ClientListTreeWidget::renamedSlot ()
{
	if (m_pAliases == NULL)
		return;

	QTreeWidgetItem *pItem = QTreeWidget::currentItem();
	if (pItem == NULL)
		return;

	const QString& sText = pItem->text(0);
	if (pItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pClientItem
            = static_cast<JackClientTreeWidgetItem *> (pItem);
		m_pAliases->setClientAlias(
			pClientItem->clientName(), sText);
		if (sText.isEmpty())
			pClientItem->setText(0, pClientItem->clientName());
	} else {
        JackPortTreeWidgetItem *pPortItem
            = static_cast<JackPortTreeWidgetItem *> (pItem);
		m_pAliases->setPortAlias(
			pPortItem->clientName(), pPortItem->portName(), sText);
		if (sText.isEmpty())
			pPortItem->setText(0, pPortItem->portName());
	}

	m_pConnectView->setDirty(true);
}

void ClientListTreeWidget::timeoutSlot ()
{
	if (m_pAutoOpenTimer) {
		m_pAutoOpenTimer->stop();
		if (m_pDropItem && m_pDropItem->type() == QJACKCTL_CLIENTITEM) {
            JackClientTreeWidgetItem *pClientItem
                = static_cast<JackClientTreeWidgetItem *> (m_pDropItem);
			if (pClientItem && !pClientItem->isOpen())
				pClientItem->setOpen(true);
		}
	}
}

bool ClientListTreeWidget::eventFilter ( QObject *pObject, QEvent *pEvent )
{
	QWidget *pViewport = QTreeWidget::viewport();
	if (static_cast<QWidget *> (pObject) == pViewport
		&& pEvent->type() == QEvent::ToolTip) {
		QHelpEvent *pHelpEvent = static_cast<QHelpEvent *> (pEvent);
		if (pHelpEvent) {
			QTreeWidgetItem *pItem = QTreeWidget::itemAt(pHelpEvent->pos());
			if (pItem && pItem->type() == QJACKCTL_CLIENTITEM) {
                JackClientTreeWidgetItem *pClientItem
                    = static_cast<JackClientTreeWidgetItem *> (pItem);
				if (pClientItem) {
					QToolTip::showText(pHelpEvent->globalPos(),
						pClientItem->clientName(), pViewport);
					return true;
				}
			}
			else
			if (pItem && pItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pPortItem
                    = static_cast<JackPortTreeWidgetItem *> (pItem);
				if (pPortItem) {
					QToolTip::showText(pHelpEvent->globalPos(),
						pPortItem->portName(), pViewport);
					return true;
				}
			}
		}
	}

	// Not handled here.
	return QTreeWidget::eventFilter(pObject, pEvent);
}

QTreeWidgetItem *ClientListTreeWidget::dragDropItem ( const QPoint& pos )
{
	QTreeWidgetItem *pItem = QTreeWidget::itemAt(pos);
	if (pItem) {
		if (m_pDropItem != pItem) {
			QTreeWidget::setCurrentItem(pItem);
			m_pDropItem = pItem;
			if (m_pAutoOpenTimer)
				m_pAutoOpenTimer->start(m_iAutoOpenTimeout);
            ConnectionsModel *pConnect = m_pConnectView->binding();
			if ((pItem->flags() & Qt::ItemIsDropEnabled) == 0
				|| pConnect == NULL || !pConnect->canConnectSelected())
				pItem = NULL;
		}
	} else {
		m_pDropItem = NULL;
		if (m_pAutoOpenTimer)
			m_pAutoOpenTimer->stop();
	}

	return pItem;
}

void ClientListTreeWidget::dragEnterEvent ( QDragEnterEvent *pDragEnterEvent )
{
	if (pDragEnterEvent->source() != this &&
		pDragEnterEvent->mimeData()->hasText() &&
		dragDropItem(pDragEnterEvent->pos())) {
		pDragEnterEvent->accept();
	} else {
		pDragEnterEvent->ignore();
	}
}


void ClientListTreeWidget::dragMoveEvent ( QDragMoveEvent *pDragMoveEvent )
{
	if (pDragMoveEvent->source() != this &&
		pDragMoveEvent->mimeData()->hasText() &&
		dragDropItem(pDragMoveEvent->pos())) {
		pDragMoveEvent->accept();
	} else {
		pDragMoveEvent->ignore();
	}
}


void ClientListTreeWidget::dragLeaveEvent ( QDragLeaveEvent * )
{
	m_pDropItem = 0;
	if (m_pAutoOpenTimer)
		m_pAutoOpenTimer->stop();
}


void ClientListTreeWidget::dropEvent( QDropEvent *pDropEvent )
{
	if (pDropEvent->source() != this &&
		pDropEvent->mimeData()->hasText() &&
		dragDropItem(pDropEvent->pos())) {
		const QString sText = pDropEvent->mimeData()->text();
        ConnectionsModel *pConnect = m_pConnectView->binding();
		if (!sText.isEmpty() && pConnect)
			pConnect->connectSelected();
	}

	dragLeaveEvent(0);
}

void ClientListTreeWidget::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	QTreeWidget::mousePressEvent(pMouseEvent);

	if (pMouseEvent->button() == Qt::LeftButton) {
		m_posDrag   = pMouseEvent->pos();
		m_pDragItem = QTreeWidget::itemAt(m_posDrag);
	}
}


void ClientListTreeWidget::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	QTreeWidget::mouseMoveEvent(pMouseEvent);

	if ((pMouseEvent->buttons() & Qt::LeftButton) && m_pDragItem
		&& ((pMouseEvent->pos() - m_posDrag).manhattanLength()
			>= QApplication::startDragDistance())) {
		// We'll start dragging something alright...
		QMimeData *pMimeData = new QMimeData();
		pMimeData->setText(m_pDragItem->text(0));
		QDrag *pDrag = new QDrag(this);
		pDrag->setMimeData(pMimeData);
		pDrag->setPixmap(m_pDragItem->icon(0).pixmap(16));
		pDrag->setHotSpot(QPoint(-4, -12));
		pDrag->start(Qt::LinkAction);
		// We've dragged and maybe dropped it by now...
		m_pDragItem = NULL;
	}
}

void ClientListTreeWidget::contextMenuEvent ( QContextMenuEvent *pContextMenuEvent )
{
    ConnectionsModel *pConnect = m_pConnectView->binding();
	if (pConnect == 0)
		return;

	QMenu menu(this);
	QAction *pAction;

	pAction = menu.addAction(QIcon(":/images/connect1.png"),
		tr("&Connect"), pConnect, SLOT(connectSelected()),
		tr("Alt+C", "Connect"));
	pAction->setEnabled(pConnect->canConnectSelected());
	pAction = menu.addAction(QIcon(":/images/disconnect1.png"),
		tr("&Disconnect"), pConnect, SLOT(disconnectSelected()),
		tr("Alt+D", "Disconnect"));
	pAction->setEnabled(pConnect->canDisconnectSelected());
	pAction = menu.addAction(QIcon(":/images/disconnectall1.png"),
		tr("Disconnect &All"), pConnect, SLOT(disconnectAll()),
		tr("Alt+A", "Disconect All"));
	pAction->setEnabled(pConnect->canDisconnectAll());
	if (m_bRenameEnabled) {
		menu.addSeparator();
		pAction = menu.addAction(QIcon(":/images/edit1.png"),
			tr("Re&name"), this, SLOT(startRenameSlot()),
			tr("Alt+N", "Rename"));
		QTreeWidgetItem *pItem = QTreeWidget::currentItem();
		pAction->setEnabled(pItem && (pItem->flags() & Qt::ItemIsEditable));
	}
	menu.addSeparator();
	pAction = menu.addAction(QIcon(":/images/refresh1.png"),
		tr("&Refresh"), pConnect, SLOT(refresh()),
		tr("Alt+R", "Refresh"));

	menu.exec(pContextMenuEvent->globalPos());
}


JackPortConnectorWidget::JackPortConnectorWidget (
    ConnectionsViewSplitter *pConnectView )
	: QWidget(pConnectView)
{
	m_pConnectView = pConnectView;

	QWidget::setMinimumWidth(20);
//  QWidget::setMaximumWidth(120);
	QWidget::setSizePolicy(
		QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

JackPortConnectorWidget::~JackPortConnectorWidget ()
{
}

int JackPortConnectorWidget::itemY ( QTreeWidgetItem *pItem ) const
{
	QRect rect;
	QTreeWidget *pList = pItem->treeWidget();
	QTreeWidgetItem *pParent = pItem->parent();
    JackClientTreeWidgetItem *pClientItem = NULL;
	if (pParent && pParent->type() == QJACKCTL_CLIENTITEM)
        pClientItem = static_cast<JackClientTreeWidgetItem *> (pParent);
	if (pClientItem && !pClientItem->isOpen()) {
		rect = pList->visualItemRect(pClientItem);
	} else {
		rect = pList->visualItemRect(pItem);
	}
	return rect.top() + rect.height() / 2;
}

void JackPortConnectorWidget::drawConnectionLine ( QPainter *pPainter,
	int x1, int y1, int x2, int y2, int h1, int h2 )
{
	// Account for list view headers.
	y1 += h1;
	y2 += h2;

	// Invisible output ports don't get a connecting dot.
	if (y1 > h1)
		pPainter->drawLine(x1, y1, x1 + 4, y1);

	// How do we'll draw it?
	if (m_pConnectView->isBezierLines()) {
		// Setup control points
		QPolygon spline(4);
		int cp = int(float(x2 - x1 - 8) * 0.4f);
		spline.putPoints(0, 4,
			x1 + 4, y1, x1 + 4 + cp, y1, 
			x2 - 4 - cp, y2, x2 - 4, y2);
		// The connection line, it self.
		QPainterPath path;
		path.moveTo(spline.at(0));
		path.cubicTo(spline.at(1), spline.at(2), spline.at(3));
		pPainter->strokePath(path, pPainter->pen());
	}
	else pPainter->drawLine(x1 + 4, y1, x2 - 4, y2);

	// Invisible input ports don't get a connecting dot.
	if (y2 > h2)
		pPainter->drawLine(x2 - 4, y2, x2, y2);
}

void JackPortConnectorWidget::paintEvent ( QPaintEvent * )
{
	if (m_pConnectView == NULL)
		return;
	if (m_pConnectView->OListView() == NULL ||
		m_pConnectView->IListView() == NULL)
		return;

    ClientListTreeWidget *pOListView = m_pConnectView->OListView();
    ClientListTreeWidget *pIListView = m_pConnectView->IListView();

	int yc = QWidget::pos().y();
	int yo = pOListView->pos().y();
	int yi = pIListView->pos().y();

	QPainter painter(this);
	int x1, y1, h1;
	int x2, y2, h2;
	int i, rgb[3] = { 0x33, 0x66, 0x99 };

	// Inline adaptive to draker background themes...
	if (QWidget::palette().window().color().value() < 0x7f)
		for (i = 0; i < 3; ++i) rgb[i] += 0x33;

	// Initialize color changer.
	i = 0;
	// Almost constants.
	x1 = 0;
	x2 = QWidget::width();
	h1 = (pOListView->header())->sizeHint().height();
	h2 = (pIListView->header())->sizeHint().height();
	// For each output client item...
	int iItemCount = pOListView->topLevelItemCount();
	for (int iItem = 0; iItem < iItemCount; ++iItem) {
		QTreeWidgetItem *pItem = pOListView->topLevelItem(iItem);
		if (pItem->type() != QJACKCTL_CLIENTITEM)
			continue;
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pItem);
		if (pOClient == NULL)
			continue;
		// Set new connector color.
		++i;
		painter.setPen(QColor(rgb[i % 3], rgb[(i / 3) % 3], rgb[(i / 9) % 3]));
		// For each port item
		int iChildCount = pOClient->childCount();
		for (int iChild = 0; iChild < iChildCount; ++iChild) {
			QTreeWidgetItem *pChild = pOClient->child(iChild);
			if (pChild->type() != QJACKCTL_PORTITEM)
				continue;
            JackPortTreeWidgetItem *pOPort
                = static_cast<JackPortTreeWidgetItem *> (pChild);
			if (pOPort) {
				// Get starting connector arrow coordinates.
				y1 = itemY(pOPort) + (yo - yc);
				// Get port connections...
                QListIterator<JackPortTreeWidgetItem *> iter(pOPort->connects());
				while (iter.hasNext()) {
                    JackPortTreeWidgetItem *pIPort = iter.next();
					// Obviously, should be a connection
					// from pOPort to pIPort items:
					y2 = itemY(pIPort) + (yi - yc);
					drawConnectionLine(&painter, x1, y1, x2, y2, h1, h2);
				}
			}
		}
	}
}

void JackPortConnectorWidget::contextMenuEvent (
	QContextMenuEvent *pContextMenuEvent )
{
    ConnectionsModel *pConnect = m_pConnectView->binding();
	if (pConnect == 0)
		return;

	QMenu menu(this);
	QAction *pAction;

	pAction = menu.addAction(QIcon(":/images/connect1.png"),
		tr("&Connect"), pConnect, SLOT(connectSelected()),
		tr("Alt+C", "Connect"));
	pAction->setEnabled(pConnect->canConnectSelected());
	pAction = menu.addAction(QIcon(":/images/disconnect1.png"),
		tr("&Disconnect"), pConnect, SLOT(disconnectSelected()),
		tr("Alt+D", "Disconnect"));
	pAction->setEnabled(pConnect->canDisconnectSelected());
	pAction = menu.addAction(QIcon(":/images/disconnectall1.png"),
		tr("Disconnect &All"), pConnect, SLOT(disconnectAll()),
		tr("Alt+A", "Disconect All"));
	pAction->setEnabled(pConnect->canDisconnectAll());

	menu.addSeparator();
	pAction = menu.addAction(QIcon(":/images/refresh1.png"),
		tr("&Refresh"), pConnect, SLOT(refresh()),
		tr("Alt+R", "Refresh"));

	menu.exec(pContextMenuEvent->globalPos());
}

void JackPortConnectorWidget::contentsChanged ()
{
	QWidget::update();
}

ConnectionsViewSplitter::ConnectionsViewSplitter ( QWidget *pParent )
	: QSplitter(Qt::Horizontal, pParent)
{
    m_pOListView     = new ClientListTreeWidget(this, true);
    m_pConnectorView = new JackPortConnectorWidget(this);
    m_pIListView     = new ClientListTreeWidget(this, false);

	m_pConnect = NULL;

	m_bBezierLines = false;
	m_iIconSize    = 0;

	QObject::connect(m_pOListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pOListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pOListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pOListView->header(), SIGNAL(sectionClicked(int)),
		m_pConnectorView, SLOT(contentsChanged()));

	QObject::connect(m_pIListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pIListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pIListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
		m_pConnectorView, SLOT(contentsChanged()));
	QObject::connect(m_pIListView->header(), SIGNAL(sectionClicked(int)),
		m_pConnectorView, SLOT(contentsChanged()));

	m_bDirty = false;
}

ConnectionsViewSplitter::~ConnectionsViewSplitter ()
{
}

void ConnectionsViewSplitter::setBinding ( ConnectionsModel *pConnect )
{
	m_pConnect = pConnect;
}

ConnectionsModel *ConnectionsViewSplitter::binding () const
{
	return m_pConnect;
}

JackClientList *ConnectionsViewSplitter::OClientList () const
{
	if (m_pConnect)
		return m_pConnect->OClientList();
	else
		return NULL;
}

JackClientList *ConnectionsViewSplitter::IClientList () const
{
	if (m_pConnect)
		return m_pConnect->OClientList();
	else
		return NULL;
}

void ConnectionsViewSplitter::setBezierLines ( bool bBezierLines )
{
	m_bBezierLines = bBezierLines;
}

bool ConnectionsViewSplitter::isBezierLines () const
{
	return m_bBezierLines;
}

void ConnectionsViewSplitter::setIconSize ( int iIconSize )
{
	// Update only if changed.
	if (iIconSize == m_iIconSize)
		return;

	// Go for it...
	m_iIconSize = iIconSize;

	// Update item sizes properly...
	int px = (16 << m_iIconSize);
	const QSize iconsize(px, px);
	m_pOListView->setIconSize(iconsize);
	m_pIListView->setIconSize(iconsize);

	// Call binding descendant implementation,
	// and do a complete content reset...
	if (m_pConnect)
		m_pConnect->updateContents(true);
}

int ConnectionsViewSplitter::iconSize () const
{
	return m_iIconSize;
}

void ConnectionsViewSplitter::setDirty ( bool bDirty )
{
	m_bDirty = bDirty;

	if (bDirty)
		emit contentsChanged();
}

bool ConnectionsViewSplitter::isDirty () const
{
	return m_bDirty;
}

ConnectionsModel::ConnectionsModel ( ConnectionsViewSplitter *pConnectView )
{
	m_pConnectView = pConnectView;

	m_pOClientList = NULL;
	m_pIClientList = NULL;

	m_iMutex = 0;

	m_pConnectView->setBinding(this);
}

ConnectionsModel::~ConnectionsModel ()
{
	// Force end of works here.
	m_iMutex++;

	m_pConnectView->setBinding(NULL);

	if (m_pOClientList)
		delete m_pOClientList;
	if (m_pIClientList)
		delete m_pIClientList;

	m_pOClientList = NULL;
	m_pIClientList = NULL;

	m_pConnectView->ConnectorView()->update();
}

ConnectionsViewSplitter *ConnectionsModel::connectView () const
{
	return m_pConnectView;
}

void ConnectionsModel::setOClientList ( JackClientList *pOClientList )
{
	m_pOClientList = pOClientList;
}

void ConnectionsModel::setIClientList ( JackClientList *pIClientList )
{
	m_pIClientList = pIClientList;
}

bool ConnectionsModel::connectPortsEx (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
	if (pOPort->findConnectPtr(pIPort) != NULL)
		return false;
	
	emit connecting(pOPort, pIPort);

	if (!connectPorts(pOPort, pIPort))
		return false;

	pOPort->addConnect(pIPort);
	pIPort->addConnect(pOPort);
	return true;
}

bool ConnectionsModel::disconnectPortsEx (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
	if (pOPort->findConnectPtr(pIPort) == NULL)
		return false;

	emit disconnecting(pOPort, pIPort);

	if (!disconnectPorts(pOPort, pIPort))
		return false;

	pOPort->removeConnect(pIPort);
	pIPort->removeConnect(pOPort);
	return true;
}

bool ConnectionsModel::canConnectSelected ()
{
	bool bResult = false;

	if (startMutex()) {
		bResult = canConnectSelectedEx();
		endMutex();
	}

	return bResult;
}

bool ConnectionsModel::canConnectSelectedEx ()
{
	// Take this opportunity to highlight any current selections.
	m_pOClientList->hiliteClientPorts();
	m_pIClientList->hiliteClientPorts();

	// Now with our predicate work...
	QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

	if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
				if (pOPort->findConnectPtr(pIPort) == NULL)
					return true;
			}
		} else {
			// Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
			while (oport.hasNext()
				&& pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
				if (pOPort->findConnectPtr(pIPort) == NULL)
					return true;
				pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
			}
		}
	} else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
				if (pOPort->findConnectPtr(pIPort) == NULL)
					return true;
			}
		} else {
			// One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
			return (pOPort->findConnectPtr(pIPort) == NULL);
		}
	}

	return false;
}

bool ConnectionsModel::connectSelected ()
{
	bool bResult = false;

	if (startMutex()) {
		bResult = connectSelectedEx();
		endMutex();
	}

	m_pConnectView->ConnectorView()->update();

	if (bResult)
		emit connectChanged();

	return bResult;
}

bool ConnectionsModel::connectSelectedEx ()
{
	QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

	if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
				connectPortsEx(pOPort, pIPort);
			}
		} else {
			// Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
			while (oport.hasNext()
				&& pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
				connectPortsEx(pOPort, pIPort);
				pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
			}
		}
	} else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
				connectPortsEx(pOPort, pIPort);
			}
		} else {
			// One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
			connectPortsEx(pOPort, pIPort);
		}
	}

	return true;
}

bool ConnectionsModel::canDisconnectSelected ()
{
	bool bResult = false;

	if (startMutex()) {
		bResult = canDisconnectSelectedEx();
		endMutex();
	}

	return bResult;
}

bool ConnectionsModel::canDisconnectSelectedEx ()
{
	QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
	if (!pOItem)
		return false;

	QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
	if (!pIItem)
		return false;

	if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
				if (pOPort->findConnectPtr(pIPort) != NULL)
					return true;
			}
		} else {
			// Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
			while (oport.hasNext()
				&& pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
				if (pOPort->findConnectPtr(pIPort) != NULL)
					return true;
				pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
			}
		}
	} else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
				if (pOPort->findConnectPtr(pIPort) != NULL)
					return true;
			}
		} else {
			// One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
			return (pOPort->findConnectPtr(pIPort) != NULL);
		}
	}

	return false;
}

bool ConnectionsModel::disconnectSelected ()
{
	bool bResult = false;

	if (startMutex()) {
		bResult = disconnectSelectedEx();
		endMutex();
	}

	m_pConnectView->ConnectorView()->update();

	if (bResult)
		emit connectChanged();

	return bResult;
}

bool ConnectionsModel::disconnectSelectedEx ()
{
	QTreeWidgetItem *pOItem = (m_pOClientList->listView())->currentItem();
	if (pOItem == NULL)
		return false;

	QTreeWidgetItem *pIItem = (m_pIClientList->listView())->currentItem();
	if (pIItem == NULL)
		return false;

	if (pOItem->type() == QJACKCTL_CLIENTITEM) {
        JackClientTreeWidgetItem *pOClient
            = static_cast<JackClientTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// Each-to-each connections...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (oport.hasNext() && iport.hasNext()) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort = iport.next();
				disconnectPortsEx(pOPort, pIPort);
			}
		} else {
			// Many(all)-to-one/many connection...
            QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
			while (oport.hasNext()
				&& pIItem && pIItem->type() == QJACKCTL_PORTITEM) {
                JackPortTreeWidgetItem *pOPort = oport.next();
                JackPortTreeWidgetItem *pIPort
                    = static_cast<JackPortTreeWidgetItem *> (pIItem);
				disconnectPortsEx(pOPort, pIPort);
				pIItem = (m_pIClientList->listView())->itemBelow(pIItem);
			}
		}
	} else {
        JackPortTreeWidgetItem *pOPort
            = static_cast<JackPortTreeWidgetItem *> (pOItem);
		if (pIItem->type() == QJACKCTL_CLIENTITEM) {
			// One-to-many(all) connection...
            JackClientTreeWidgetItem *pIClient
                = static_cast<JackClientTreeWidgetItem *> (pIItem);
            QListIterator<JackPortTreeWidgetItem *> iport(pIClient->ports());
			while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
				disconnectPortsEx(pOPort, pIPort);
			}
		} else {
			// One-to-one connection...
            JackPortTreeWidgetItem *pIPort
                = static_cast<JackPortTreeWidgetItem *> (pIItem);
			disconnectPortsEx(pOPort, pIPort);
		}
	}

	return true;
}

bool ConnectionsModel::canDisconnectAll ()
{
	bool bResult = false;

	if (startMutex()) {
		bResult = canDisconnectAllEx();
		endMutex();
	}

	return bResult;
}

bool ConnectionsModel::canDisconnectAllEx ()
{
    QListIterator<JackClientTreeWidgetItem *> iter(m_pOClientList->clients());
	while (iter.hasNext()) {
        JackClientTreeWidgetItem *pOClient = iter.next();
        QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
		while (oport.hasNext()) {
            JackPortTreeWidgetItem *pOPort = oport.next();
			if (pOPort->connects().count() > 0)
				return true;
		}
	}
	return false;
}

bool ConnectionsModel::disconnectAll ()
{
	if (QMessageBox::warning(m_pConnectView,
		tr("Warning") + " - " QJACKCTL_SUBTITLE1,
		tr("This will suspend sound processing\n"
		"from all client applications.\n\nAre you sure?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return false;
	}

	bool bResult = false;

	if (startMutex()) {
		bResult = disconnectAllEx();
		endMutex();
	}

	m_pConnectView->ConnectorView()->update();

	if (bResult)
		emit connectChanged();

	return bResult;
}

bool ConnectionsModel::disconnectAllEx ()
{
    QListIterator<JackClientTreeWidgetItem *> iter(m_pOClientList->clients());
	while (iter.hasNext()) {
        JackClientTreeWidgetItem *pOClient = iter.next();
        QListIterator<JackPortTreeWidgetItem *> oport(pOClient->ports());
		while (oport.hasNext()) {
            JackPortTreeWidgetItem *pOPort = oport.next();
            QListIterator<JackPortTreeWidgetItem *> iport(pOPort->connects());
			while (iport.hasNext()) {
                JackPortTreeWidgetItem *pIPort = iport.next();
				disconnectPortsEx(pOPort, pIPort);
			}
		}
	}
	return true;
}

void ConnectionsModel::expandAll ()
{
	(m_pOClientList->listView())->expandAll();
	(m_pIClientList->listView())->expandAll();
	(m_pConnectView->ConnectorView())->update();
}

void ConnectionsModel::updateContents ( bool bClear )
{
	int iDirtyCount = 0;

	if (startMutex()) {
		// Do we do a complete rebuild?
		if (bClear) {
			m_pOClientList->clear();
			m_pIClientList->clear();
			updateIconPixmaps();
		}
		// Add (newer) client:ports and respective connections...
		if (m_pOClientList->updateClientPorts() > 0) {
			m_pOClientList->refresh();
			iDirtyCount++;
		}
		if (m_pIClientList->updateClientPorts() > 0) {
			m_pIClientList->refresh();
			iDirtyCount++;
		}			
		updateConnections();
		endMutex();
	}

	(m_pConnectView->ConnectorView())->update();

	if (!bClear && iDirtyCount > 0)
		emit connectChanged();
}

void ConnectionsModel::refresh ()
{
	updateContents(false);
}

bool ConnectionsModel::startMutex ()
{
	bool bMutex = (m_iMutex == 0);
	if (bMutex)
		m_iMutex++;
	return bMutex;
}

void ConnectionsModel::endMutex ()
{
	if (m_iMutex > 0)
		m_iMutex--;
}

JackClientList *ConnectionsModel::OClientList () const
{
	return m_pOClientList;
}

JackClientList *ConnectionsModel::IClientList () const
{
	return m_pIClientList;
}

QPixmap *ConnectionsModel::createIconPixmap ( const QString& sIconName )
{
	QString sName = sIconName;
	int     iSize = m_pConnectView->iconSize() * 32;

	if (iSize > 0)
		sName += QString("_%1x%2").arg(iSize).arg(iSize);

	return new QPixmap(":/images/" + sName + ".png");
}
