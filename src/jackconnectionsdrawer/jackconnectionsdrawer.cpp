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
#include "jackconnectionsdrawer.h"
#include "jackservice.h"
#include "clienttreewidget.h"
#include "portconnectionswidget.h"
#include "jackclienttreewidgetitem.h"
#include "jackaudioporttreewidgetitem.h"

#include <QDebug>

JackConnectionsDrawer::JackConnectionsDrawer(QWidget *parent)
    : ConnectionsDrawer(parent) {
    connect(&JackService::instance().client(), SIGNAL(connectedToServer()), this, SLOT(connectedToServer()));
    connect(&JackService::instance().client(), SIGNAL(disconnectedFromServer()), this, SLOT(disconnectedFromServer()));
    connect(&JackService::instance().client(), SIGNAL(portRegistered(QJack::Port)), this, SLOT(portRegistered(QJack::Port)));
}

JackConnectionsDrawer::~JackConnectionsDrawer() {
}

void JackConnectionsDrawer::connectedToServer() {
    JackService::instance().client().activate();
    completeUpdate();
}

void JackConnectionsDrawer::disconnectedFromServer() {
}

void JackConnectionsDrawer::portRegistered(QJack::Port port) {
    qDebug() << "test";
    if(port.isAudioPort()) {
        QJack::AudioPort audioPort = QJack::AudioPort(port);
        if(audioPort.isValid()) {
            if(audioPort.isOutput()) {
                addAudioPort(_outputTreeWidget, audioPort);
            } else
            if(audioPort.isInput()) {
                addAudioPort(_inputTreeWidget, audioPort);
            }
        }
    } else
    if(port.isMidiPort()) {
        // ..
    }
}

void JackConnectionsDrawer::addAudioPort(ClientTreeWidget *clientTreeWidget,
                                         QJack::AudioPort audioPort) {
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
            }
        }
    }

    if(!clientFound) {
        JackClientTreeWidgetItem *inputItem = new JackClientTreeWidgetItem(clientName);
        clientTreeWidget->addTopLevelItem(inputItem);
        inputItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
    }
}

void JackConnectionsDrawer::completeUpdate() {
    _inputTreeWidget->clear();
    _outputTreeWidget->clear();
    _portConnectionsWidget->update();

    // Get a full list of all connected clients.
    QStringList clientList = JackService::instance().client().clientList();

    // Walk each client
    foreach(QString client, clientList) {
        // Add client items
        JackClientTreeWidgetItem *outputItem = new JackClientTreeWidgetItem(client);
        _outputTreeWidget->addTopLevelItem(outputItem);

        JackClientTreeWidgetItem *inputItem = new JackClientTreeWidgetItem(client);
        _inputTreeWidget->addTopLevelItem(inputItem);

        //
        QList<QJack::Port> ports = JackService::instance().client().portsForClient(client);
        foreach(QJack::Port port, ports) {
            if(port.isAudioPort()) {
                QJack::AudioPort audioPort = QJack::AudioPort(port);
                if(audioPort.isInput()) {
                    inputItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
                } else
                if(audioPort.isOutput()) {
                    outputItem->addChild(new JackAudioPortTreeWidgetItem(audioPort));
                }
            } else
            if(port.isMidiPort()) {

            }
        }
    }
}
