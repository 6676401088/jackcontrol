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

#pragma once

// Own includes
#include "porttreewidgetitem.h"
#include "connectionsdrawer.h"

// QtJack includes
#include "audioport.h"
#include "midiport.h"

class JackConnectionsDrawer : public ConnectionsDrawer {
    Q_OBJECT
public:
    JackConnectionsDrawer(QWidget *parent = 0);
    ~JackConnectionsDrawer();

    void connectSelectedItems();
    void disconnectSelectedItems();
    void disconnectAll();

protected slots:
    void connectedToServer();
    void disconnectedFromServer();

    void clientRegistered(QString clientName);
    void clientUnregistered(QString clientName);

    void portRegistered(QtJack::Port port);
    void portUnregistered(QtJack::Port port);

    void connectDroppedItem(PortTreeWidgetItem* portItem, QString itemIdentifier);

    void itemChanged(QTreeWidgetItem* treeWidgetItem, int column);

protected:
    void completeUpdate();

    void removeClient(ClientTreeWidget *clientTreeWidget, QString clientName);

    void addAudioPort(ClientTreeWidget *clientTreeWidget, QtJack::AudioPort audioPort);
    void addMidiPort(ClientTreeWidget *clientTreeWidget, QtJack::MidiPort midiPort);
    void removePort(ClientTreeWidget *clientTreeWidget, QtJack::Port port);
};

