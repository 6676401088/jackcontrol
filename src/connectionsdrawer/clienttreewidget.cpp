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
#include "clienttreewidget.h"
#include "clienttreewidgetitem.h"

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

ClientTreeWidget::ClientTreeWidget(QWidget *parent)
    : QTreeWidget(parent) {

    _autoOpenTimer   = 0;
    _autoOpenTimeout = 0;

    _dragItem = NULL;
    _dragItem = NULL;

    _connectAliases = NULL;
    _renameEnabled = false;

    QHeaderView *headerView = QTreeWidget::header();
    headerView->setDefaultAlignment(Qt::AlignLeft);
    headerView->setSectionsMovable(false);
    headerView->setSectionsClickable(true);

    headerView->setSortIndicatorShown(true);
    headerView->setStretchLastSection(true);

    setRootIsDecorated(true);
    setUniformRowHeights(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setAutoScroll(true);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setExpandsOnDoubleClick(true);

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Expanding));
    setSortingEnabled(true);
    setColumnCount(1);
    setAlternatingRowColors(true);
    setAnimated(true);
    setTextElideMode(Qt::ElideMiddle);
    setMinimumWidth(120);

    sortItems(0, Qt::AscendingOrder);

    // Trap for help/tool-tips events.
    viewport()->installEventFilter(this);

    connect(QTreeWidget::itemDelegate(),
        SIGNAL(commitData(QWidget*)),
        SLOT(renamedSlot()));

    setAutoOpenTimeout(800);
}

ClientTreeWidget::~ClientTreeWidget() {
    setAutoOpenTimeout(0);
}

void ClientTreeWidget::setHeaderTitle(QString headerTitle) {
    headerItem()->setText(0, headerTitle);
    setToolTip(headerTitle);
}

void ClientTreeWidget::setAutoOpenTimeout(int autoOpenTimeout) {
    _autoOpenTimeout = autoOpenTimeout;

    if (_autoOpenTimer)
        delete _autoOpenTimer;
    _autoOpenTimer = NULL;

    if (_autoOpenTimeout > 0) {
        _autoOpenTimer = new QTimer(this);
        connect(_autoOpenTimer,
            SIGNAL(timeout()),
            SLOT(timeoutSlot()));
    }
}

int ClientTreeWidget::autoOpenTimeout() const {
    return _autoOpenTimeout;
}

void ClientTreeWidget::setAliases(ConnectAlias *connectAliases, bool renameEnabled) {
    Q_UNUSED(connectAliases);
    Q_UNUSED(renameEnabled);
//    _connectAliases = connectAliases;
//    _renameEnabled = renameEnabled;

//    // For each client item, if any.
//    int iItemCount = QTreeWidget::topLevelItemCount();
//    for (int iItem = 0; iItem < iItemCount; ++iItem) {
//        QTreeWidgetItem *pItem = QTreeWidget::topLevelItem(iItem);
//        if (pItem->type() != QJACKCTL_CLIENTITEM)
//            continue;
//        ClientTreeWidgetItem *pClientItem
//            = static_cast<ClientTreeWidgetItem *> (pItem);
//        if (pClientItem == NULL)
//            continue;
//        if (_connectAliases) {
//            pClientItem->setText(0,
//                _connectAliases->clientAlias(pClientItem->clientName()));
//        } else {
//            pClientItem->setText(0,
//                pClientItem->clientName());
//        }
//        if (_connectAliases && _renameEnabled) {
//            pClientItem->setFlags(
//                pClientItem->flags() |  Qt::ItemIsEditable);
//        } else {
//            pClientItem->setFlags(
//                pClientItem->flags() & ~Qt::ItemIsEditable);
//        }
//        // For each port item...
//        int iChildCount = pClientItem->childCount();
//        for (int iChild = 0; iChild < iChildCount; ++iChild) {
//            QTreeWidgetItem *pChildItem = pClientItem->child(iChild);
//            if (pChildItem->type() != QJACKCTL_PORTITEM)
//                continue;
//            PortTreeWidgetItem *pPortItem
//                = static_cast<PortTreeWidgetItem *> (pChildItem);
//            if (pPortItem == NULL)
//                continue;
//            if (_connectAliases) {
//                pPortItem->setText(0, _connectAliases->portAlias(
//                    pPortItem->clientName(), pPortItem->portName()));
//            } else {
//                pPortItem->setText(0,
//                    pPortItem->portName());
//            }
//            if (_connectAliases && _renameEnabled) {
//                pPortItem->setFlags(
//                    pPortItem->flags() |  Qt::ItemIsEditable);
//            } else {
//                pPortItem->setFlags(
//                    pPortItem->flags() & ~Qt::ItemIsEditable);
//            }
//        }
//    }
}

ConnectAlias *ClientTreeWidget::aliases() const {
    return _connectAliases;
}

bool ClientTreeWidget::renameEnabled() const {
    return _renameEnabled;
}

void ClientTreeWidget::startRenameSlot() {
    QTreeWidgetItem *pItem = QTreeWidget::currentItem();
    if(pItem) {
        QTreeWidget::editItem(pItem, 0);
    }
}

void ClientTreeWidget::renamedSlot() {
//    if (_connectAliases == NULL)
//        return;

//    QTreeWidgetItem *pItem = QTreeWidget::currentItem();
//    if (pItem == NULL)
//        return;

//    const QString& sText = pItem->text(0);
//    if (pItem->type() == QJACKCTL_CLIENTITEM) {
//        ClientTreeWidgetItem *pClientItem
//            = static_cast<ClientTreeWidgetItem *> (pItem);
//        _connectAliases->setClientAlias(
//            pClientItem->clientName(), sText);
//        if (sText.isEmpty())
//            pClientItem->setText(0, pClientItem->clientName());
//    } else {
//        PortTreeWidgetItem *pPortItem
//            = static_cast<PortTreeWidgetItem *> (pItem);
//        _connectAliases->setPortAlias(
//            pPortItem->clientName(), pPortItem->portName(), sText);
//        if (sText.isEmpty())
//            pPortItem->setText(0, pPortItem->portName());
//    }

//    emit contentsChanged();
}

void ClientTreeWidget::timeoutSlot() {
    if (_autoOpenTimer) {
        _autoOpenTimer->stop();
        if (_dropItem && _dropItem->type() == QJACKCTL_CLIENTITEM) {
            ClientTreeWidgetItem *pClientItem
                = static_cast<ClientTreeWidgetItem *> (_dropItem);
            if(pClientItem && !pClientItem->isExpanded()) {
                pClientItem->setExpanded(true);
            }
        }
    }
}

bool ClientTreeWidget::eventFilter(QObject *pObject, QEvent *pEvent) {
//    QWidget *pViewport = QTreeWidget::viewport();
//    if (static_cast<QWidget *> (pObject) == pViewport
//        && pEvent->type() == QEvent::ToolTip) {
//        QHelpEvent *pHelpEvent = static_cast<QHelpEvent *> (pEvent);
//        if (pHelpEvent) {
//            QTreeWidgetItem *pItem = QTreeWidget::itemAt(pHelpEvent->pos());
//            if (pItem && pItem->type() == QJACKCTL_CLIENTITEM) {
//                ClientTreeWidgetItem *pClientItem
//                    = static_cast<ClientTreeWidgetItem *> (pItem);
//                if (pClientItem) {
//                    QToolTip::showText(pHelpEvent->globalPos(),
//                        pClientItem->clientName(), pViewport);
//                    return true;
//                }
//            }
//            else
//            if (pItem && pItem->type() == QJACKCTL_PORTITEM) {
//                PortTreeWidgetItem *pPortItem
//                    = static_cast<PortTreeWidgetItem *> (pItem);
//                if (pPortItem) {
//                    QToolTip::showText(pHelpEvent->globalPos(),
//                        pPortItem->portName(), pViewport);
//                    return true;
//                }
//            }
//        }
//    }

    // Not handled here.
    return QTreeWidget::eventFilter(pObject, pEvent);
}

QTreeWidgetItem *ClientTreeWidget::dragDropItem(const QPoint& pos) {
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

void ClientTreeWidget::dragEnterEvent(QDragEnterEvent *pDragEnterEvent) {
    if(pDragEnterEvent->source() != this &&
        pDragEnterEvent->mimeData()->hasText() &&
        dragDropItem(pDragEnterEvent->pos())) {
        pDragEnterEvent->accept();
    } else {
        pDragEnterEvent->ignore();
    }
}


void ClientTreeWidget::dragMoveEvent(QDragMoveEvent *pDragMoveEvent) {
    if (pDragMoveEvent->source() != this &&
        pDragMoveEvent->mimeData()->hasText() &&
        dragDropItem(pDragMoveEvent->pos())) {
        pDragMoveEvent->accept();
    } else {
        pDragMoveEvent->ignore();
    }
}


void ClientTreeWidget::dragLeaveEvent(QDragLeaveEvent *) {
    _dropItem = 0;
    if (_autoOpenTimer)
        _autoOpenTimer->stop();
}


void ClientTreeWidget::dropEvent(QDropEvent *pDropEvent) {
    Q_UNUSED(pDropEvent);
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

void ClientTreeWidget::mousePressEvent(QMouseEvent *pMouseEvent) {
    QTreeWidget::mousePressEvent(pMouseEvent);

    if (pMouseEvent->button() == Qt::LeftButton) {
        _dragStartPosition   = pMouseEvent->pos();
        _dragItem = QTreeWidget::itemAt(_dragStartPosition);
    }
}


void ClientTreeWidget::mouseMoveEvent(QMouseEvent *pMouseEvent) {
    QTreeWidget::mouseMoveEvent(pMouseEvent);

    if ((pMouseEvent->buttons() & Qt::LeftButton) && _dragItem
        && ((pMouseEvent->pos() - _dragStartPosition).manhattanLength()
            >= QApplication::startDragDistance())) {
        // We'll start dragging something alright...
        QMimeData *pMimeData = new QMimeData();
        pMimeData->setText(_dragItem->text(0));
        QDrag *pDrag = new QDrag(this);
        pDrag->setMimeData(pMimeData);
        pDrag->setPixmap(_dragItem->icon(0).pixmap(16));
        pDrag->setHotSpot(QPoint(-4, -12));
        pDrag->start(Qt::LinkAction);
        // We've dragged and maybe dropped it by now...
        _dragItem = NULL;
    }
}

void ClientTreeWidget::contextMenuEvent(QContextMenuEvent *pContextMenuEvent) {
    Q_UNUSED(pContextMenuEvent);
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

QList<PortTreeWidgetItem *> ClientTreeWidget::ports() {
    QList<PortTreeWidgetItem *> items;
    int numberOfClients = topLevelItemCount();
    for(int clientIndex = 0; clientIndex < numberOfClients; clientIndex++) {
        ClientTreeWidgetItem *clientItem
            = dynamic_cast<ClientTreeWidgetItem*>(topLevelItem(clientIndex));
        if(clientItem) {
            int numberOfPorts = clientItem->childCount();
            for(int portIndex = 0; portIndex < numberOfPorts; portIndex++) {
                PortTreeWidgetItem *portItem
                    = dynamic_cast<PortTreeWidgetItem*>(clientItem->child(portIndex));
                if(portItem) {
                    items.append(portItem);
                }
            }
        }
    }
    return items;
}

void ClientTreeWidget::propagateWheelEvent(QWheelEvent *wheelEvent) {
    QTreeWidget::wheelEvent(wheelEvent);
}

