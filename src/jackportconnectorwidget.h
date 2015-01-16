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
class ConnectionsViewSplitter;

// Qt includes
#include <QWidget>
#include <QTreeWidgetItem>

class JackPortConnectorWidget : public QWidget {
    Q_OBJECT
public:
    JackPortConnectorWidget(ConnectionsViewSplitter *pConnectView);
    ~JackPortConnectorWidget();

public slots:

    // Useful slots (should this be protected?).
    void contentsChanged();

protected:

    // Draw visible port connection relation arrows.
    void paintEvent(QPaintEvent *);

    // Context menu request event handler.
    virtual void contextMenuEvent(QContextMenuEvent *);

private:

    // Legal client/port item position helper.
    int itemY(QTreeWidgetItem *pItem) const;

    // Drawing methods.
    void drawConnectionLine(QPainter *pPainter,
        int x1, int y1, int x2, int y2, int h1, int h2);

    // Local instance variables.
    ConnectionsViewSplitter *m_pConnectView;
};
