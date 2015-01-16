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
#include <QPainter>
#include <QHeaderView>
#include <QContextMenuEvent>

// Own includes
#include "patchworkwidget.h"
#include "patchbaysplitter.h"

PatchworkWidget::PatchworkWidget (
    PatchbaySplitter *pPatchbayView )
    : QWidget(pPatchbayView) {
    m_pPatchbayView = pPatchbayView;

    QWidget::setMinimumWidth(20);
//	QWidget::setMaximumWidth(120);
    QWidget::setSizePolicy(
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

PatchworkWidget::~PatchworkWidget () {
}

int PatchworkWidget::itemY(QTreeWidgetItem *pItem) const {
    QRect rect;
    QTreeWidget *pList = pItem->treeWidget();
    QTreeWidgetItem *pParent = pItem->parent();
    SocketTreeWidgetItem *pSocketItem = NULL;
    if (pParent && pParent->type() == QJACKCTL_SOCKETITEM)
        pSocketItem = static_cast<SocketTreeWidgetItem *> (pParent);
    if (pSocketItem && !pSocketItem->isOpen()) {
        rect = pList->visualItemRect(pParent);
    } else {
        rect = pList->visualItemRect(pItem);
    }
    return rect.top() + rect.height() / 2;
}


// Draw visible socket connection relation lines
void PatchworkWidget::drawConnectionLine ( QPainter *pPainter,
    int x1, int y1, int x2, int y2, int h1, int h2 ) {
    // Account for list view headers.
    y1 += h1;
    y2 += h2;

    // Invisible output plugs don't get a connecting dot.
    if (y1 > h1)
        pPainter->drawLine(x1, y1, x1 + 4, y1);

    // How do we'll draw it?
    if (m_pPatchbayView->isBezierLines()) {
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
    }   // Old style...
    else pPainter->drawLine(x1 + 4, y1, x2 - 4, y2);

    // Invisible input plugs don't get a connecting dot.
    if (y2 > h2)
        pPainter->drawLine(x2 - 4, y2, x2, y2);
}


// Draw socket forwrading line (for input sockets / right pane only)
void PatchworkWidget::drawForwardLine ( QPainter *pPainter,
    int x, int dx, int y1, int y2, int h )
{
    // Account for list view headers.
    y1 += h;
    y2 += h;
    dx += 4;

    // Draw it...
    if (y1 < y2) {
        pPainter->drawLine(x - dx, y1 + 4, x, y1);
        pPainter->drawLine(x - dx, y1 + 4, x - dx, y2 - 4);
        pPainter->drawLine(x - dx, y2 - 4, x, y2);
        // Down arrow...
        pPainter->drawLine(x - dx, y2 - 8, x - dx - 2, y2 - 12);
        pPainter->drawLine(x - dx, y2 - 8, x - dx + 2, y2 - 12);
    } else {
        pPainter->drawLine(x - dx, y1 - 4, x, y1);
        pPainter->drawLine(x - dx, y1 - 4, x - dx, y2 + 4);
        pPainter->drawLine(x - dx, y2 + 4, x, y2);
        // Up arrow...
        pPainter->drawLine(x - dx, y2 + 8, x - dx - 2, y2 + 12);
        pPainter->drawLine(x - dx, y2 + 8, x - dx + 2, y2 + 12);
    }
}


// Draw visible socket connection relation arrows.
void PatchworkWidget::paintEvent ( QPaintEvent * )
{
    if (m_pPatchbayView->OSocketList() == NULL ||
        m_pPatchbayView->ISocketList() == NULL)
        return;

    QPainter painter(this);
    int x1, y1, h1;
    int x2, y2, h2;
    int i, rgb[3] = { 0x99, 0x66, 0x33 };

    // Inline adaptive to darker background themes...
    if (QWidget::palette().window().color().value() < 0x7f)
        for (i = 0; i < 3; ++i) rgb[i] += 0x33;

    // Initialize color changer.
    i = 0;
    // Almost constants.
    x1 = 0;
    x2 = width();
    h1 = ((m_pPatchbayView->OListView())->header())->sizeHint().height();
    h2 = ((m_pPatchbayView->IListView())->header())->sizeHint().height();
    // For each client item...
    SocketTreeWidgetItem *pOSocket, *pISocket;
    QListIterator<SocketTreeWidgetItem *> osocket(
        (m_pPatchbayView->OSocketList())->sockets());
    while (osocket.hasNext()) {
        pOSocket = osocket.next();
        // Set new connector color.
        ++i;
        painter.setPen(QColor(rgb[i % 3], rgb[(i / 3) % 3], rgb[(i / 9) % 3]));
        // Get starting connector arrow coordinates.
        y1 = itemY(pOSocket);
        // Get input socket connections...
        QListIterator<SocketTreeWidgetItem *> isocket(pOSocket->connects());
        while (isocket.hasNext()) {
            pISocket = isocket.next();
            // Obviously, there is a connection from pOPlug to pIPlug items:
            y2 = itemY(pISocket);
            drawConnectionLine(&painter, x1, y1, x2, y2, h1, h2);
        }
    }

    // Look for forwarded inputs...
    QList<SocketTreeWidgetItem *> iforwards;
    // Make a local copy of just the forwarding socket list, if any...
    QListIterator<SocketTreeWidgetItem *> isocket(
        (m_pPatchbayView->ISocketList())->sockets());
    while (isocket.hasNext()) {
        pISocket = isocket.next();
        // Check if its forwarded...
        if (pISocket->forward().isEmpty())
            continue;
        iforwards.append(pISocket);
    }
    // (Re)initialize color changer.
    i = 0;
    // Now traverse those for proper connection drawing...
    int dx = 0;
    QListIterator<SocketTreeWidgetItem *> iter(iforwards);
    while (iter.hasNext()) {
        pISocket = iter.next();
        SocketTreeWidgetItem *pISocketForward
            = m_pPatchbayView->ISocketList()->findSocket(
                pISocket->forward(), pISocket->socketType());
        if (pISocketForward == NULL)
            continue;
        // Set new connector color.
        ++i;
        painter.setPen(QColor(rgb[i % 3], rgb[(i / 3) % 3], rgb[(i / 9) % 3]));
        // Get starting connector arrow coordinates.
        y1 = itemY(pISocketForward);
        y2 = itemY(pISocket);
        drawForwardLine(&painter, x2, dx, y1, y2, h2);
        dx += 2;
    }
}


// Context menu request event handler.
void PatchworkWidget::contextMenuEvent (
    QContextMenuEvent *pContextMenuEvent )
{
    m_pPatchbayView->contextMenu(pContextMenuEvent->globalPos(), NULL);
}

void PatchworkWidget::contentsChanged ()
{
    QWidget::update();
}
