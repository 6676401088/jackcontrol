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

// Qt includes
#include <QHeaderView>
#include <QTimer>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QMimeData>
#include <QApplication>
#include <QDrag>

// Own includes
#include "sockettreewidget.h"
#include "patchbaysplitter.h"

SocketTreeWidget::SocketTreeWidget (
    PatchbaySplitter *pPatchbayView, bool bReadable )
    : QTreeWidget(pPatchbayView) {
    m_pPatchbayView = pPatchbayView;
    m_bReadable     = bReadable;

    m_pAutoOpenTimer   = 0;
    m_iAutoOpenTimeout = 0;

    m_pDragItem = NULL;
    m_pDropItem = NULL;

    QHeaderView *pHeader = QTreeWidget::header();
//	pHeader->setDefaultAlignment(Qt::AlignLeft);
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
//	pHeader->setSortIndicatorShown(true);
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
    QTreeWidget::setSortingEnabled(false);
    QTreeWidget::setMinimumWidth(120);
    QTreeWidget::setColumnCount(1);

    // Trap for help/tool-tips events.
    QTreeWidget::viewport()->installEventFilter(this);

    QString sText;
    if (m_bReadable)
        sText = tr("Output Sockets / Plugs");
    else
        sText = tr("Input Sockets / Plugs");
    QTreeWidget::headerItem()->setText(0, sText);
    QTreeWidget::setToolTip(sText);

    setAutoOpenTimeout(800);
}

SocketTreeWidget::~SocketTreeWidget() {
    setAutoOpenTimeout(0);
}

void SocketTreeWidget::setDirty(bool bDirty) {
    m_pPatchbayView->setDirty(bDirty);
}

bool SocketTreeWidget::dirty() const {
    return m_pPatchbayView->dirty();
}

void SocketTreeWidget::setAutoOpenTimeout(int iAutoOpenTimeout) {
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

int SocketTreeWidget::autoOpenTimeout() const {
    return m_iAutoOpenTimeout;
}

void SocketTreeWidget::timeoutSlot() {
    if (m_pAutoOpenTimer) {
        m_pAutoOpenTimer->stop();
        if (m_pDropItem && m_pDropItem->type() == QJACKCTL_SOCKETITEM) {
            SocketTreeWidgetItem *pSocketItem
                = static_cast<SocketTreeWidgetItem *> (m_pDropItem);
            if (pSocketItem && !pSocketItem->isOpen())
                pSocketItem->setOpen(true);
        }
    }
}

bool SocketTreeWidget::eventFilter(QObject *pObject, QEvent *pEvent) {
    QWidget *pViewport = QTreeWidget::viewport();
    if (static_cast<QWidget *> (pObject) == pViewport
        && pEvent->type() == QEvent::ToolTip) {
        QHelpEvent *pHelpEvent = static_cast<QHelpEvent *> (pEvent);
        if (pHelpEvent) {
            QTreeWidgetItem *pItem = QTreeWidget::itemAt(pHelpEvent->pos());
            if (pItem && pItem->type() == QJACKCTL_SOCKETITEM) {
                SocketTreeWidgetItem *pSocketItem
                    = static_cast<SocketTreeWidgetItem *> (pItem);
                if (pSocketItem) {
                    QToolTip::showText(pHelpEvent->globalPos(),
                        pSocketItem->clientName(), pViewport);
                    return true;
                }
            }
            else
            if (pItem && pItem->type() == QJACKCTL_PLUGITEM) {
                PlugTreeWidgetItem *pPlugItem
                    = static_cast<PlugTreeWidgetItem *> (pItem);
                if (pPlugItem) {
                    QToolTip::showText(pHelpEvent->globalPos(),
                        pPlugItem->plugName(), pViewport);
                    return true;
                }
            }
        }
    }

    // Not handled here.
    return QTreeWidget::eventFilter(pObject, pEvent);
}

QTreeWidgetItem *SocketTreeWidget::dragDropItem(const QPoint& pos) {
    QTreeWidgetItem *pItem = QTreeWidget::itemAt(pos);
    if (pItem) {
        if (m_pDropItem != pItem) {
            QTreeWidget::setCurrentItem(pItem);
            m_pDropItem = pItem;
            if (m_pAutoOpenTimer)
                m_pAutoOpenTimer->start(m_iAutoOpenTimeout);
            Patchbay *pPatchbay = m_pPatchbayView->binding();
            if ((pItem->flags() & Qt::ItemIsDropEnabled) == 0
                || pPatchbay == NULL || !pPatchbay->canConnectSelected())
                pItem = NULL;
        }
    } else {
        m_pDropItem = NULL;
        if (m_pAutoOpenTimer)
            m_pAutoOpenTimer->stop();
    }

    return pItem;
}

void SocketTreeWidget::dragEnterEvent(QDragEnterEvent *pDragEnterEvent) {
    if (pDragEnterEvent->source() != this &&
        pDragEnterEvent->mimeData()->hasText() &&
        dragDropItem(pDragEnterEvent->pos())) {
        pDragEnterEvent->accept();
    } else {
        pDragEnterEvent->ignore();
    }
}

void SocketTreeWidget::dragMoveEvent(QDragMoveEvent *pDragMoveEvent) {
    if (pDragMoveEvent->source() != this &&
        pDragMoveEvent->mimeData()->hasText() &&
        dragDropItem(pDragMoveEvent->pos())) {
        pDragMoveEvent->accept();
    } else {
        pDragMoveEvent->ignore();
    }
}

void SocketTreeWidget::dragLeaveEvent(QDragLeaveEvent *) {
    m_pDropItem = 0;
    if (m_pAutoOpenTimer)
        m_pAutoOpenTimer->stop();
}

void SocketTreeWidget::dropEvent ( QDropEvent *pDropEvent )
{
    if (pDropEvent->source() != this &&
        pDropEvent->mimeData()->hasText() &&
        dragDropItem(pDropEvent->pos())) {
        const QString sText = pDropEvent->mimeData()->text();
        Patchbay *pPatchbay = m_pPatchbayView->binding();
        if (!sText.isEmpty() && pPatchbay)
            pPatchbay->connectSelected();
    }

    dragLeaveEvent(NULL);
}


// Handle mouse events for drag-and-drop stuff.
void SocketTreeWidget::mousePressEvent ( QMouseEvent *pMouseEvent )
{
    QTreeWidget::mousePressEvent(pMouseEvent);

    if (pMouseEvent->button() == Qt::LeftButton) {
        m_posDrag   = pMouseEvent->pos();
        m_pDragItem = QTreeWidget::itemAt(m_posDrag);
    }
}


void SocketTreeWidget::mouseMoveEvent(QMouseEvent *pMouseEvent) {
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

void SocketTreeWidget::contextMenuEvent(QContextMenuEvent *pContextMenuEvent) {
    m_pPatchbayView->contextMenu(
        pContextMenuEvent->globalPos(),
        (m_bReadable
            ? m_pPatchbayView->OSocketList()
            : m_pPatchbayView->ISocketList())
    );
}
