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
#include "jackservice.h"
#include "clienttreewidget.h"
#include "portconnectionswidget.h"
#include "jackconnectionsdrawer.h"
#include "jackclienttreewidgetitem.h"
#include "jackaudioporttreewidgetitem.h"
#include "jackmidiporttreewidgetitem.h"

// Qt includes
#include <QDebug>

JackConnectionsDrawer::JackConnectionsDrawer(QWidget *parent)
    : ConnectionsDrawer(parent) {
    connect(&JackService::instance().client(), SIGNAL(connectedToServer()), this, SLOT(connectedToServer()));
    connect(&JackService::instance().client(), SIGNAL(disconnectedFromServer()), this, SLOT(disconnectedFromServer()));

    connect(&JackService::instance().client(), SIGNAL(clientRegistered(QString)), this, SLOT(clientRegistered(QString)));
    connect(&JackService::instance().client(), SIGNAL(clientUnregistered(QString)), this, SLOT(clientUnregistered(QString)));

    connect(&JackService::instance().client(), SIGNAL(portRegistered(QtJack::Port)), this, SLOT(portRegistered(QtJack::Port)));
    connect(&JackService::instance().client(), SIGNAL(portUnregistered(QtJack::Port)), this, SLOT(portUnregistered(QtJack::Port)));

    connect(&JackService::instance().client(), SIGNAL(graphOrderHasChanged()), _portConnectionsWidget, SLOT(update()));

    connect(_sendTreeWidget, SIGNAL(droppedItem(PortTreeWidgetItem*,QString)),
            this, SLOT(connectDroppedItem(PortTreeWidgetItem*,QString)));
    connect(_returnTreeWidget, SIGNAL(droppedItem(PortTreeWidgetItem*,QString)),
            this, SLOT(connectDroppedItem(PortTreeWidgetItem*,QString)));
}

JackConnectionsDrawer::~JackConnectionsDrawer() {
}

void JackConnectionsDrawer::connectedToServer() {
    completeUpdate();
}

void JackConnectionsDrawer::disconnectedFromServer() {
    _sendTreeWidget->clear();
    _returnTreeWidget->clear();
}

void JackConnectionsDrawer::connectSelectedItems() {
    QList<QTreeWidgetItem*> sendItems = _sendTreeWidget->selectedItems();
    QList<QTreeWidgetItem*> returnItems = _returnTreeWidget->selectedItems();

    foreach(QTreeWidgetItem* sendItem, sendItems) {
        JackAudioPortTreeWidgetItem *jackAudioPortTreeWidgetSendItem
            = dynamic_cast<JackAudioPortTreeWidgetItem*>(sendItem);
        JackMidiPortTreeWidgetItem *jackMidiPortTreeWidgetSendItem
            = dynamic_cast<JackMidiPortTreeWidgetItem*>(sendItem);

        foreach(QTreeWidgetItem* returnItem, returnItems) {
            JackAudioPortTreeWidgetItem *jackAudioPortTreeWidgetReturnItem
                = dynamic_cast<JackAudioPortTreeWidgetItem*>(returnItem);
            JackMidiPortTreeWidgetItem *jackMidiPortTreeWidgetReturnItem
                = dynamic_cast<JackMidiPortTreeWidgetItem*>(returnItem);

            if(jackAudioPortTreeWidgetSendItem) {
                if(jackAudioPortTreeWidgetReturnItem) {
                    JackService::instance().client().connect(jackAudioPortTreeWidgetSendItem->audioPort(),
                                                             jackAudioPortTreeWidgetReturnItem->audioPort());
                }
            }

            if(jackMidiPortTreeWidgetSendItem) {
                if(jackMidiPortTreeWidgetReturnItem) {
                    JackService::instance().client().connect(jackMidiPortTreeWidgetSendItem->midiPort(),
                                                             jackMidiPortTreeWidgetReturnItem->midiPort());
                }
            }
        }
    }
}

void JackConnectionsDrawer::disconnectSelectedItems() {
    QList<QTreeWidgetItem*> sendItems = _sendTreeWidget->selectedItems();
    QList<QTreeWidgetItem*> returnItems = _returnTreeWidget->selectedItems();

    foreach(QTreeWidgetItem* sendItem, sendItems) {
        JackAudioPortTreeWidgetItem *jackAudioPortTreeWidgetSendItem
            = dynamic_cast<JackAudioPortTreeWidgetItem*>(sendItem);
        JackMidiPortTreeWidgetItem *jackMidiPortTreeWidgetSendItem
            = dynamic_cast<JackMidiPortTreeWidgetItem*>(sendItem);

        foreach(QTreeWidgetItem* returnItem, returnItems) {
            JackAudioPortTreeWidgetItem *jackAudioPortTreeWidgetReturnItem
                = dynamic_cast<JackAudioPortTreeWidgetItem*>(returnItem);
            JackMidiPortTreeWidgetItem *jackMidiPortTreeWidgetReturnItem
                = dynamic_cast<JackMidiPortTreeWidgetItem*>(returnItem);

            if(jackAudioPortTreeWidgetSendItem) {
                if(jackAudioPortTreeWidgetReturnItem) {
                    JackService::instance().client().disconnect(jackAudioPortTreeWidgetSendItem->audioPort(),
                                                                jackAudioPortTreeWidgetReturnItem->audioPort());
                }
            }

            if(jackMidiPortTreeWidgetSendItem) {
                if(jackMidiPortTreeWidgetReturnItem) {
                    JackService::instance().client().disconnect(jackMidiPortTreeWidgetSendItem->midiPort(),
                                                                jackMidiPortTreeWidgetReturnItem->midiPort());
                }
            }
        }
    }
}

void JackConnectionsDrawer::disconnectAll() {
    _sendTreeWidget->selectAll();
    _returnTreeWidget->selectAll();
    disconnectSelectedItems();
    _sendTreeWidget->clearSelection();
    _returnTreeWidget->clearSelection();
}

void JackConnectionsDrawer::clientRegistered(QString clientName) {
    qDebug() << "Client registered: " << clientName;
}

void JackConnectionsDrawer::clientUnregistered(QString clientName) {
    removeClient(_returnTreeWidget, clientName);
    removeClient(_sendTreeWidget, clientName);
}

void JackConnectionsDrawer::portRegistered(QtJack::Port port) {
    if(port.isAudioPort()) {
        QtJack::AudioPort audioPort = QtJack::AudioPort(port);
        if(audioPort.isValid()) {
            if(audioPort.isOutput()) {
                addAudioPort(_sendTreeWidget, audioPort);
            } else
            if(audioPort.isInput()) {
                addAudioPort(_returnTreeWidget, audioPort);
            }
        }
    } else
    if(port.isMidiPort()) {
        QtJack::MidiPort midiPort = QtJack::MidiPort(port);
        if(midiPort.isValid()) {
            if(midiPort.isOutput()) {
                addMidiPort(_sendTreeWidget, midiPort);
            } else
            if(midiPort.isInput()) {
                addMidiPort(_returnTreeWidget, midiPort);
            }
        }
    }
}

void JackConnectionsDrawer::portUnregistered(QtJack::Port port) {
    removePort(_sendTreeWidget, port);
    removePort(_returnTreeWidget, port);
}

void JackConnectionsDrawer::connectDroppedItem(PortTreeWidgetItem* portItem,
                                               QString itemIdentifier) {
    JackAudioPortTreeWidgetItem *audioPortTreeWidgetItem
        = dynamic_cast<JackAudioPortTreeWidgetItem*>(portItem);
    JackMidiPortTreeWidgetItem *midiPortTreeWidgetItem
        = dynamic_cast<JackMidiPortTreeWidgetItem*>(portItem);

    QtJack::Port port = JackService::instance().client().portByName(itemIdentifier);

    if(audioPortTreeWidgetItem && port.isAudioPort()) {
        if(port.isOutput()) {
            JackService::instance().client().connect(port, audioPortTreeWidgetItem->audioPort());
        } else {
            JackService::instance().client().connect(audioPortTreeWidgetItem->audioPort(), port);
        }
    }

    if(midiPortTreeWidgetItem && port.isMidiPort()) {
        if(port.isOutput()) {
            JackService::instance().client().connect(port, midiPortTreeWidgetItem->midiPort());
        } else {
            JackService::instance().client().connect(midiPortTreeWidgetItem->midiPort(), port);
        }
    }
}

void JackConnectionsDrawer::itemChanged(QTreeWidgetItem* treeWidgetItem, int column) {
    JackAudioPortTreeWidgetItem *audioPortTreeWidgetItem
        = dynamic_cast<JackAudioPortTreeWidgetItem*>(treeWidgetItem);
    JackMidiPortTreeWidgetItem *midiPortTreeWidgetItem
        = dynamic_cast<JackMidiPortTreeWidgetItem*>(treeWidgetItem);

    if(audioPortTreeWidgetItem) {
        if(!audioPortTreeWidgetItem->audioPort().rename(treeWidgetItem->text(column))) {
            treeWidgetItem->setText(column, audioPortTreeWidgetItem->audioPort().portName());
        }
    }

    if(midiPortTreeWidgetItem) {
        if(!midiPortTreeWidgetItem->midiPort().rename(treeWidgetItem->text(column))) {
            treeWidgetItem->setText(column, midiPortTreeWidgetItem->midiPort().portName());
        }
    }
}


void JackConnectionsDrawer::completeUpdate() {
    _returnTreeWidget->clear();
    _sendTreeWidget->clear();
    _portConnectionsWidget->update();

    QStringList clientList = JackService::instance().client().clientList();

    foreach(QString client, clientList) {
        JackClientTreeWidgetItem *outputClientItem = 0;
        if(JackService::instance().client().numberOfOutputPorts(client)) {
            outputClientItem = new JackClientTreeWidgetItem(client);
            _sendTreeWidget->addTopLevelItem(outputClientItem);
        }

        JackClientTreeWidgetItem *inputClientItem = 0;
        if(JackService::instance().client().numberOfInputPorts(client)) {
            inputClientItem = new JackClientTreeWidgetItem(client);
            _returnTreeWidget->addTopLevelItem(inputClientItem);
        }

        QList<QtJack::Port> ports = JackService::instance().client().portsForClient(client);
        foreach(QtJack::Port port, ports) {
            if(port.isAudioPort()) {
                QtJack::AudioPort audioPort = QtJack::AudioPort(port);
                if(audioPort.isInput() && inputClientItem) {
                    inputClientItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
                } else
                if(audioPort.isOutput() && outputClientItem) {
                    outputClientItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
                }
            } else
            if(port.isMidiPort()) {
                QtJack::MidiPort midiPort = QtJack::MidiPort(port);
                if(midiPort.isInput() && inputClientItem) {
                    inputClientItem->addChild(new JackMidiPortTreeWidgetItem(midiPort));
                } else
                if(midiPort.isOutput() && outputClientItem) {
                    outputClientItem->addChild(new JackMidiPortTreeWidgetItem(midiPort));
                }
            }
        }
    }

    expandAll();
}

void JackConnectionsDrawer::removeClient(ClientTreeWidget *clientTreeWidget,
                                         QString clientName) {
    int clientCount     = clientTreeWidget->topLevelItemCount();
    for(int clientIndex = 0; clientIndex < clientCount; clientIndex++) {
        // Get the client tree widget item
        QTreeWidgetItem *treeWidgetItem = clientTreeWidget->topLevelItem(clientIndex);
        JackClientTreeWidgetItem *jackClientTreeWidgetItem
            = dynamic_cast<JackClientTreeWidgetItem*>(treeWidgetItem);

        // Check if it is a client tree widget item
        if(jackClientTreeWidgetItem) {
            if(jackClientTreeWidgetItem->clientName() == clientName) {
                if(clientTreeWidget->takeTopLevelItem(clientIndex)) {
                    clientCount--;
                }
            }
        }
    }
}

void JackConnectionsDrawer::addAudioPort(ClientTreeWidget *clientTreeWidget,
                                         QtJack::AudioPort audioPort) {
    QString clientName  = audioPort.clientName();
    int clientCount     = clientTreeWidget->topLevelItemCount();
    bool clientFound    = false;

    for(int clientIndex = 0; clientIndex < clientCount; clientIndex++) {
        // Get the client tree widget item
        QTreeWidgetItem *treeWidgetItem = clientTreeWidget->topLevelItem(clientIndex);
        JackClientTreeWidgetItem *jackClientTreeWidgetItem
            = dynamic_cast<JackClientTreeWidgetItem*>(treeWidgetItem);

        // Check if it is a client tree widget item
        if(jackClientTreeWidgetItem) {
            if(jackClientTreeWidgetItem->clientName() == clientName) {
                clientFound = true;
                jackClientTreeWidgetItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
                return;
            }
        }
    }

    if(!clientFound) {
        JackClientTreeWidgetItem *inputItem = new JackClientTreeWidgetItem(clientName);
        clientTreeWidget->addTopLevelItem(inputItem);
        inputItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
        clientTreeWidget->expandItem(inputItem);
    }
}

void JackConnectionsDrawer::addMidiPort(ClientTreeWidget *clientTreeWidget,
                                        QtJack::MidiPort midiPort) {
    QString clientName  = midiPort.clientName();
    int clientCount     = clientTreeWidget->topLevelItemCount();
    bool clientFound    = false;

    for(int clientIndex = 0; clientIndex < clientCount; clientIndex++) {
        // Get the client tree widget item
        QTreeWidgetItem *treeWidgetItem = clientTreeWidget->topLevelItem(clientIndex);
        JackClientTreeWidgetItem *jackClientTreeWidgetItem
            = dynamic_cast<JackClientTreeWidgetItem*>(treeWidgetItem);

        // Check if it is a client tree widget item
        if(jackClientTreeWidgetItem) {
            if(jackClientTreeWidgetItem->clientName() == clientName) {
                clientFound = true;
                jackClientTreeWidgetItem->addChild(new JackMidiPortTreeWidgetItem(midiPort));
                return;
            }
        }
    }

    if(!clientFound) {
        JackClientTreeWidgetItem *inputItem = new JackClientTreeWidgetItem(clientName);
        clientTreeWidget->addTopLevelItem(inputItem);
        inputItem->addChild(new JackMidiPortTreeWidgetItem(midiPort));
        clientTreeWidget->expandItem(inputItem);
    }
}

void JackConnectionsDrawer::removePort(ClientTreeWidget *clientTreeWidget, QtJack::Port port) {
    int clientCount = clientTreeWidget->topLevelItemCount();
    for(int clientIndex = 0; clientIndex < clientCount; clientIndex++) {
        QTreeWidgetItem *clientTreeWidgetItem  = clientTreeWidget->topLevelItem(clientIndex);
        int portCount = clientTreeWidgetItem->childCount();
        for(int portIndex = 0; portIndex < portCount; portIndex++) {
            QTreeWidgetItem *portTreeWidgetItem = clientTreeWidgetItem->child(portIndex);
            JackAudioPortTreeWidgetItem *audioPortTreeWidgetItem
                = dynamic_cast<JackAudioPortTreeWidgetItem*>(portTreeWidgetItem);
            JackMidiPortTreeWidgetItem *midiPortTreeWidgetItem
                = dynamic_cast<JackMidiPortTreeWidgetItem*>(portTreeWidgetItem);
            if(audioPortTreeWidgetItem) {
                if(audioPortTreeWidgetItem->audioPort() == QtJack::AudioPort(port)) {
                    clientTreeWidgetItem->removeChild(audioPortTreeWidgetItem);
                    break;
                }
            }
            if(midiPortTreeWidgetItem) {
                if(midiPortTreeWidgetItem->midiPort() == QtJack::MidiPort(port)) {
                    clientTreeWidgetItem->removeChild(midiPortTreeWidgetItem);
                    break;
                }
            }
        }
    }
}


