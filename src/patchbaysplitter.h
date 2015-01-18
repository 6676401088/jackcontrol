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
#include "sockettreewidget.h"
#include "patchworkwidget.h"
#include "patchbay.h"

// Qt includes
#include <QSplitter>

class PatchbaySplitter : public QSplitter {
    Q_OBJECT
public:
    PatchbaySplitter(QWidget *pParent = 0);
    ~PatchbaySplitter();

    // Widget accesors.
    SocketTreeWidget *OListView()     { return m_pOListView; }
    SocketTreeWidget *IListView()     { return m_pIListView; }
    PatchworkWidget  *PatchworkView() { return m_pPatchworkView; }

    // Patchbay object binding methods.
    void setBinding(Patchbay *pPatchbay);
    Patchbay *binding() const;

    // Socket list accessors.
    SocketList *OSocketList() const;
    SocketList *ISocketList() const;

    // Patchwork line style accessors.
    void setBezierLines(bool bBezierLines);
    bool isBezierLines() const;

    // Patchbay dirty flag accessors.
    void setDirty (bool bDirty);
    bool dirty() const;

signals:

    // Contents change signal.
    void contentsChanged();

public slots:

    // Common context menu slots.
    void contextMenu(const QPoint& pos, SocketList *pSocketList);
    void activateForwardMenu(QAction *);

private:

    // Child controls.
    SocketTreeWidget *m_pOListView;
    SocketTreeWidget *m_pIListView;
    PatchworkWidget  *m_pPatchworkView;

    // The main binding object.
    Patchbay *m_pPatchbay;

    // How we'll draw patchwork lines.
    bool m_bBezierLines;

    // The obnoxious dirty flag.
    bool m_bDirty;
};
