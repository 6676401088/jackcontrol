/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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
class PatchbaySplitter;
#include "socketlist.h"

// Qt includes
#include <QTreeWidget>
#include <QSplitter>

// Our external patchbay models.
#include "PatchbayRack.h"

// QListViewItem::rtti return values.
#define QJACKCTL_SOCKETITEM 2001
#define QJACKCTL_PLUGITEM   2002

class Patchbay : public QObject {
	Q_OBJECT
public:
    Patchbay(PatchbaySplitter *pPatchbayView);
    ~Patchbay();

	// Explicit connection tests.
	bool canConnectSelected();
	bool canDisconnectSelected();
	bool canDisconnectAll();

	// Socket list accessors.
    SocketList *OSocketList() const;
    SocketList *ISocketList() const;

	// External rack transfer methods.
	void loadRack(PatchbayRack *pPatchbayRack);
	void saveRack(PatchbayRack *pPatchbayRack);

public slots:

	// Complete contents refreshner.
	void refresh();

	// Explicit connection slots.
	bool connectSelected();
	bool disconnectSelected();
	bool disconnectAll();

	// Expand all socket items.
	void expandAll();

	// Complete patchbay clearer.
	void clear();

	// Do actual and complete connections snapshot.
	void connectionsSnapshot();

private:

	// Internal rack transfer methods.
    void loadRackSockets (SocketList *pSocketList,
		QList<qjackctlPatchbaySocket *>& socketlist);
    void saveRackSockets (SocketList *pSocketList,
		QList<qjackctlPatchbaySocket *>& socketlist);

	// Connect/Disconnection primitives.
    void connectSockets(SocketTreeWidgetItem *pOSocket,
        SocketTreeWidgetItem *pISocket);
    void disconnectSockets(SocketTreeWidgetItem *pOSocket,
        SocketTreeWidgetItem *pISocket);

	// Instance variables.
    PatchbaySplitter *m_pPatchbayView;
    SocketList *m_pOSocketList;
    SocketList *m_pISocketList;
};
