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
#include "jackportconnectorwidget.h"
#include "ConnectViewSplitter.h"

// Qt includes
#include <QPainter>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

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
