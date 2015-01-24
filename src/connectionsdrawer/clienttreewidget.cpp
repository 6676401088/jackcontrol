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
    setIconSize(QSize(24, 24));

    sortItems(0, Qt::AscendingOrder);
}

ClientTreeWidget::~ClientTreeWidget() {
}

void ClientTreeWidget::setHeaderTitle(QString headerTitle) {
    headerItem()->setText(0, headerTitle);
    setToolTip(headerTitle);
}

void ClientTreeWidget::dragEnterEvent(QDragEnterEvent *dragEnterEvent) {
}

void ClientTreeWidget::dragMoveEvent(QDragMoveEvent *dragMoveEvent) {
}

void ClientTreeWidget::dragLeaveEvent(QDragLeaveEvent *dragLeaveEvent) {
}

void ClientTreeWidget::dropEvent(QDropEvent *dropEvent) {
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

