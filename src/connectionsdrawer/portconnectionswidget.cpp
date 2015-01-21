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
#include "portconnectionswidget.h"
#include "clienttreewidgetitem.h"
#include "porttreewidgetitem.h"

// Qt includes
#include <QPainter>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

PortConnectionsWidget::PortConnectionsWidget(
    ClientTreeWidget *outputClientListTreeWidget,
    ClientTreeWidget *inputClientListTreeWidget,
    QWidget *parent)
    : QWidget(parent) {
    _outputClientListTreeWidget = outputClientListTreeWidget;
    _inputClientListTreeWidget  = inputClientListTreeWidget;
    _drawingBezierLines         = true;

    setMinimumWidth(200);
    setMaximumWidth(200);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Expanding));
}

PortConnectionsWidget::~PortConnectionsWidget() {
}

int PortConnectionsWidget::itemY(QTreeWidgetItem *pItem) const {
    QRect rect;
    QTreeWidget *pList = pItem->treeWidget();
    QTreeWidgetItem *pParent = pItem->parent();
    ClientTreeWidgetItem *pClientItem = NULL;
    if (pParent && pParent->type() == QJACKCTL_CLIENTITEM)
        pClientItem = static_cast<ClientTreeWidgetItem *> (pParent);
    if (pClientItem && !pClientItem->isOpen()) {
        rect = pList->visualItemRect(pClientItem);
    } else {
        rect = pList->visualItemRect(pItem);
    }
    return rect.top() + rect.height() / 2;
}

void PortConnectionsWidget::setDrawingBezierLines(bool drawingBezierLines) {
    _drawingBezierLines = drawingBezierLines;
}

bool PortConnectionsWidget::isDrawingBezierLines() const {
    return _drawingBezierLines;
}

void PortConnectionsWidget::drawConnectionLine(
    QPainter *pPainter,
    int x1, int y1,
    int x2, int y2,
    int h1, int h2 ) {
    // Account for list view headers.
    y1 += h1;
    y2 += h2;

    // Invisible output ports don't get a connecting dot.
    if (y1 > h1)
        pPainter->drawLine(x1, y1, x1 + 4, y1);

    // How do we'll draw it?
    if (isDrawingBezierLines()) {
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

void PortConnectionsWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    // Window offsets and measures
    int widgetPositionY     = pos().y();
    int outputPositionY     = _outputClientListTreeWidget->pos().y();
    int inputPositionY      = _inputClientListTreeWidget->pos().y();
    int outputHeaderOffset  = _outputClientListTreeWidget->header()->sizeHint().height();
    int inputHeaderOffset   = _inputClientListTreeWidget->header()->sizeHint().height();

    // Connection line measures
    int left                = 0;
    int right               = width();
    int startY, endY;

    QList<PortTreeWidgetItem*> outputPorts  = _outputClientListTreeWidget->ports();
    QList<PortTreeWidgetItem*> inputPorts   = _inputClientListTreeWidget->ports();

    foreach(PortTreeWidgetItem *outputPort, outputPorts) {
        foreach(PortTreeWidgetItem *inputPort, inputPorts) {
            if(outputPort->isConnectedTo(inputPort)) {
                startY  = itemY(outputPort) + outputPositionY - widgetPositionY;
                endY    = itemY(inputPort) + inputPositionY - widgetPositionY;
                drawConnectionLine(&painter,
                                   left, startY,
                                   right, endY,
                                   outputHeaderOffset, inputHeaderOffset);
            }
        }
    }
}

void PortConnectionsWidget::contextMenuEvent (
    QContextMenuEvent *pContextMenuEvent )
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

//    menu.addSeparator();
//    pAction = menu.addAction(QIcon(":/images/refresh1.png"),
//        tr("&Refresh"), pConnect, SLOT(refresh()),
//        tr("Alt+R", "Refresh"));

//    menu.exec(pContextMenuEvent->globalPos());
}

void PortConnectionsWidget::contentsChanged() {
    QWidget::update();
}
