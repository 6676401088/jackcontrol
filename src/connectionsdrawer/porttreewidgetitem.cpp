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

// Own includes
#include "porttreewidgetitem.h"
#include "clienttreewidgetitem.h"
#include "connectalias.h"
#include "clienttreewidget.h"

PortTreeWidgetItem::PortTreeWidgetItem(QString portName)
    : QTreeWidgetItem(QJACKCTL_PORTITEM) {
    _portMark               = 0;
    _highlight              = false;
    setPortName(portName);
}

PortTreeWidgetItem::~PortTreeWidgetItem() {
}

void PortTreeWidgetItem::setPortName(QString portName) {
    setText(0, portName);
}

const QString& PortTreeWidgetItem::portName() const {
    return text(0);
}

void PortTreeWidgetItem::markPort(int mark) {
    setHighlighted(false);
    _portMark = mark;
}

int PortTreeWidgetItem::portMark() const {
    return _portMark;
}

bool PortTreeWidgetItem::highlighted() const {
    return _highlight;
}

void PortTreeWidgetItem::setHighlighted (bool highlighted ) {
    // Update the port highlightning if changed...
    if ((_highlight && !highlighted) || (!_highlight && highlighted)) {
        _highlight = highlighted;
        // Propagate this to the parent...
        _clientTreeWidgetItem->setHighlighted(highlighted);
    }

    // Set the new color.
    QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
    if (pTreeWidget == NULL)
        return;

    const QPalette& pal = pTreeWidget->palette();
    QTreeWidgetItem::setTextColor(0, _highlight
        ? (pal.base().color().value() < 0x7f ? Qt::cyan : Qt::blue)
        : pal.text().color());
}

bool PortTreeWidgetItem::isConnectedTo(PortTreeWidgetItem *other) {
    Q_UNUSED(other);
    return false;
}

