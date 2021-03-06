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
#include "jackaudioporttreewidgetitem.h"

// Qt includes
#include <QIcon>

JackAudioPortTreeWidgetItem::JackAudioPortTreeWidgetItem(QtJack::AudioPort audioPort)
    : PortTreeWidgetItem(audioPort.portName()) {
    _audioPort = audioPort;

    if(_audioPort.isOutput()) {
        if(_audioPort.isPhysical()) {
            setIcon(0, QIcon("://images/mic.svg"));
        } else {
            setIcon(0, QIcon("://images/audio_in.svg"));
        }
    } else {
        if(_audioPort.isInput()) {
            if(_audioPort.isPhysical()) {
                setIcon(0, QIcon("://images/loudspeaker.svg"));
            } else {
                setIcon(0, QIcon("://images/audio_out.svg"));
            }
        }
    }

    setFlags(flags() | Qt::ItemIsEditable);
}

bool JackAudioPortTreeWidgetItem::isConnectedTo(PortTreeWidgetItem *other) {
    // Null ports cannot be connected.
    if(!other) {
        return false;
    }

    // Try to cast to an audio port.
    JackAudioPortTreeWidgetItem *audioPortItem
        = dynamic_cast<JackAudioPortTreeWidgetItem*>(other);

    if(!audioPortItem) {
        // This not an audio port, therefore it cannot be connected.
        return false;
    }

    return _audioPort.isConnectedTo(audioPortItem->_audioPort);
}

QtJack::AudioPort JackAudioPortTreeWidgetItem::audioPort() {
    return _audioPort;
}

QString JackAudioPortTreeWidgetItem::dragIdentifier() {
    return _audioPort.fullName();
}

