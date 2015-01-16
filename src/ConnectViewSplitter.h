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
//#include "ConnectAlias.h"
#include "clientlisttreewidget.h"
#include "jackportconnectorwidget.h"
#include "connectionsmodel.h"
#include "jackclientlist.h"

// Qt includes
#include <QSplitter>

class ConnectionsViewSplitter : public QSplitter
{
	Q_OBJECT
public:
    ConnectionsViewSplitter(QWidget *pParent = 0);
    ~ConnectionsViewSplitter();

	// Widget accesors.
    ClientListTreeWidget *OListView() const
		{ return m_pOListView; }
    ClientListTreeWidget *IListView() const
		{ return m_pIListView; }
    JackPortConnectorWidget  *ConnectorView() const
		{ return m_pConnectorView; }

	// Connections object binding methods.
    void setBinding(ConnectionsModel *pConnect);
    ConnectionsModel *binding() const;

	// Client list accessors.
    JackClientList *OClientList() const;
    JackClientList *IClientList() const;

	// Connector line style accessors.
	void setBezierLines(bool bBezierLines);
	bool isBezierLines() const;

	// Common icon size pixmap accessors.
	void setIconSize (int iIconSize);
    int iconSize () const;

	// Dirty flag accessors.
	void setDirty (bool bDirty);
	bool isDirty() const;

signals:

	// Contents change signal.
	void contentsChanged();

private:

	// Child controls.
    ClientListTreeWidget *m_pOListView;
    ClientListTreeWidget *m_pIListView;
    JackPortConnectorWidget  *m_pConnectorView;

	// The main binding object.
    ConnectionsModel *m_pConnect;

	// How we'll draw connector lines.
	bool m_bBezierLines;

	// How large will be those icons.
	// 0 = 16x16 (default), 1 = 32x32, 2 = 64x64.
	int m_iIconSize;

	// The obnoxious dirty flag.
	bool m_bDirty;
};


