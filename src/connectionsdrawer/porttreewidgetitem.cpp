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

PortTreeWidgetItem::PortTreeWidgetItem(QString portName)
    : QTreeWidgetItem() {
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

bool PortTreeWidgetItem::isConnectedTo(PortTreeWidgetItem *other) {
    Q_UNUSED(other);
    return false;
}

