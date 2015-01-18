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

#include <Qt>

// Own includes
#include "clientlisttreewidget.h"
#include "connectionssplitter.h"

// Qt includes
#include <QHeaderView>
#include <QTimer>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QMenu>
#include <QAction>

ClientListTreeWidget::ClientListTreeWidget (
    ConnectionsSplitter *pConnectView, bool bReadable )
    : QTreeWidget(pConnectView) {
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

    connect(QTreeWidget::itemDelegate(),
        SIGNAL(commitData(QWidget*)),
        SLOT(renamedSlot()));

    setAutoOpenTimeout(800);
}

ClientListTreeWidget::~ClientListTreeWidget ()
{
    setAutoOpenTimeout(0);
}

void ClientListTreeWidget::setAutoOpenTimeout ( int iAutoOpenTimeout )
{
    m_iAutoOpenTimeout = iAutoOpenTimeout;

    if (m_pAutoOpenTimer)
        delete m_pAutoOpenTimer;
    m_pAutoOpenTimer = NULL;

    if (m_iAutoOpenTimeout > 0) {
        m_pAutoOpenTimer = new QTimer(this);
        connect(m_pAutoOpenTimer,
            SIGNAL(timeout()),
            SLOT(timeoutSlot()));
    }
}

int ClientListTreeWidget::autoOpenTimeout () const
{
    return m_iAutoOpenTimeout;
}

void ClientListTreeWidget::setAliases ( ConnectAlias *pAliases, bool bRenameEnabled )
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

ConnectAlias *ClientListTreeWidget::aliases () const
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

    // TODO: Use Qt signals here.
    //m_pConnectView->setDirty(true);
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
//    if (pItem) {
//        if (m_pDropItem != pItem) {
//            QTreeWidget::setCurrentItem(pItem);
//            m_pDropItem = pItem;
//            if (m_pAutoOpenTimer)
//                m_pAutoOpenTimer->start(m_iAutoOpenTimeout);
//            ConnectionsModel *pConnect = m_pConnectView->binding();
//            if ((pItem->flags() & Qt::ItemIsDropEnabled) == 0
//                || pConnect == NULL || !pConnect->canConnectSelected())
//                pItem = NULL;
//        }
//    } else {
//        m_pDropItem = NULL;
//        if (m_pAutoOpenTimer)
//            m_pAutoOpenTimer->stop();
//    }

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
//    if (pDropEvent->source() != this &&
//        pDropEvent->mimeData()->hasText() &&
//        dragDropItem(pDropEvent->pos())) {
//        const QString sText = pDropEvent->mimeData()->text();
//        ConnectionsModel *pConnect = m_pConnectView->binding();
//        if (!sText.isEmpty() && pConnect)
//            pConnect->connectSelected();
//    }

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
//    ConnectionsModel *pConnect = m_pConnectView->binding();
//    if (pConnect == 0)
//        return;

//    QMenu menu(this);
//    QAction *pAction;

//    pAction = menu.addAction(QIcon(":/images/connect1.png"),
//        tr("&Connect"), pConnect, SLOT(connectSelected()),
//        tr("Alt+C", "Connect"));
//    pAction->setEnabled(pConnect->canConnectSelected());
//    pAction = menu.addAction(QIcon(":/images/disconnect1.png"),
//        tr("&Disconnect"), pConnect, SLOT(disconnectSelected()),
//        tr("Alt+D", "Disconnect"));
//    pAction->setEnabled(pConnect->canDisconnectSelected());
//    pAction = menu.addAction(QIcon(":/images/disconnectall1.png"),
//        tr("Disconnect &All"), pConnect, SLOT(disconnectAll()),
//        tr("Alt+A", "Disconect All"));
//    pAction->setEnabled(pConnect->canDisconnectAll());
//    if (m_bRenameEnabled) {
//        menu.addSeparator();
//        pAction = menu.addAction(QIcon(":/images/edit1.png"),
//            tr("Re&name"), this, SLOT(startRenameSlot()),
//            tr("Alt+N", "Rename"));
//        QTreeWidgetItem *pItem = QTreeWidget::currentItem();
//        pAction->setEnabled(pItem && (pItem->flags() & Qt::ItemIsEditable));
//    }
//    menu.addSeparator();
//    pAction = menu.addAction(QIcon(":/images/refresh1.png"),
//        tr("&Refresh"), pConnect, SLOT(refresh()),
//        tr("Alt+R", "Refresh"));

//    menu.exec(pContextMenuEvent->globalPos());
}
