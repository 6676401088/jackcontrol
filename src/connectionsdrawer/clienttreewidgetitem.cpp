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
#include "clienttreewidgetitem.h"
#include "clienttreewidget.h"

#define QJACKCTL_CLIENTITEM    1001

ClientTreeWidgetItem::ClientTreeWidgetItem(QString clientName)
    : QTreeWidgetItem(QJACKCTL_CLIENTITEM) {
    _clientName = clientName;
    _clientMark = 0;
    _hilight    = 0;

    setText(0, clientName);
}

ClientTreeWidgetItem::~ClientTreeWidgetItem () {
}

void ClientTreeWidgetItem::setClientName(QString clientName) {
    QTreeWidgetItem::setText(0, clientName);

    _clientName = clientName;
}

QString ClientTreeWidgetItem::clientName() const {
    return _clientName;
}

void ClientTreeWidgetItem::markClient(int mark) {
    setHighlighted(false);
    _clientMark = mark;
}

int ClientTreeWidgetItem::clientMark () const {
    return _clientMark;
}

bool ClientTreeWidgetItem::isHighlighted () const {
    return (_hilight > 0);
}

void ClientTreeWidgetItem::setHighlighted(bool highlighted) {
    if (highlighted)
        _hilight++;
    else
    if (_hilight > 0)
        _hilight--;

    // Set the new color.
    QTreeWidget *pTreeWidget = QTreeWidgetItem::treeWidget();
    if (pTreeWidget == NULL)
        return;

    const QPalette& pal = pTreeWidget->palette();
    QTreeWidgetItem::setTextColor(0, _hilight > 0
        ? (pal.base().color().value() < 0x7f ? Qt::darkCyan : Qt::darkBlue)
        : pal.text().color());
}

void ClientTreeWidgetItem::setOpen ( bool bOpen ) {
    QTreeWidgetItem::setExpanded(bOpen);
}

bool ClientTreeWidgetItem::isOpen () const {
    return QTreeWidgetItem::isExpanded();
}

