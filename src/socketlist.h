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
class SocketTreeWidget;
#include "sockettreewidgetitem.h"

// Qt includes
#include <QObject>

// Pixmap-set indexes.
#define QJACKCTL_XPM_AUDIO_SOCKET   0
#define QJACKCTL_XPM_AUDIO_SOCKET_X 1
#define QJACKCTL_XPM_AUDIO_CLIENT   2
#define QJACKCTL_XPM_AUDIO_PLUG     3
#define QJACKCTL_XPM_MIDI_SOCKET    4
#define QJACKCTL_XPM_MIDI_SOCKET_X  5
#define QJACKCTL_XPM_MIDI_CLIENT    6
#define QJACKCTL_XPM_MIDI_PLUG      7
#define QJACKCTL_XPM_PIXMAPS        8

class SocketList : public QObject
{
    Q_OBJECT
public:
    SocketList(SocketTreeWidget *pListView, bool bReadable);
    ~SocketList();

    // Socket list primitive methods.
    void addSocket(SocketTreeWidgetItem *pSocket);
    void removeSocket(SocketTreeWidgetItem *pSocket);

    // Socket finder.
    SocketTreeWidgetItem *findSocket(const QString& sSocketName, int iSocketType);

    // List view accessor.
    SocketTreeWidget *listView() const;

    // Socket flags accessor.
    bool isReadable() const;

    // Socket aesthetics accessors.
    const QString& socketCaption() const;

    // Socket list cleaner.
    void clear();

    // Socket list accessor.
    QList<SocketTreeWidgetItem *>& sockets();

    // Find the current selected socket item in list.
    SocketTreeWidgetItem *selectedSocketItem() const;

    // Retrieve a context pixmap.
    const QPixmap& pixmap(int iPixmap) const;

public slots:
    // Socket item interactivity methods.
    bool addSocketItem();
    bool removeSocketItem();
    bool editSocketItem();
    bool copySocketItem();
    bool exclusiveSocketItem();
    bool moveUpSocketItem();
    bool moveDownSocketItem();

private:
    // Merge two pixmaps with union of respective masks.
    QPixmap *createPixmapMerge(const QPixmap& xpmDst, const QPixmap& xpmSrc);

    // Instance variables.
    SocketTreeWidget *m_pListView;
    bool m_bReadable;
    QString  m_sSocketCaption;

    QPixmap *m_apPixmaps[QJACKCTL_XPM_PIXMAPS];

    QList<SocketTreeWidgetItem *> m_sockets;
};
