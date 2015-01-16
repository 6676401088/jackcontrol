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

#include <Qt>

// Own includes
#include "About.h"
#include "ConnectViewSplitter.h"

// Qt includes
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolTip>
#include <QPainter>
#include <QPolygon>
#include <QPainterPath>
#include <QTimer>
#include <QMenu>
#include <QMimeData>
#include <QDrag>
#include <QHelpEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

ConnectionsViewSplitter::ConnectionsViewSplitter ( QWidget *pParent )
	: QSplitter(Qt::Horizontal, pParent)
{
    m_pOListView     = new ClientListTreeWidget(this, true);
    m_pConnectorView = new JackPortConnectorWidget(this);
    m_pIListView     = new ClientListTreeWidget(this, false);

	m_pConnect = NULL;

	m_bBezierLines = false;
	m_iIconSize    = 0;

	connect(m_pOListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pOListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pOListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pOListView->header(), SIGNAL(sectionClicked(int)),
		m_pConnectorView, SLOT(contentsChanged()));

	connect(m_pIListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pIListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pIListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
		m_pConnectorView, SLOT(contentsChanged()));
	connect(m_pIListView->header(), SIGNAL(sectionClicked(int)),
		m_pConnectorView, SLOT(contentsChanged()));

	m_bDirty = false;
}

ConnectionsViewSplitter::~ConnectionsViewSplitter ()
{
}

void ConnectionsViewSplitter::setBinding ( ConnectionsModel *pConnect )
{
	m_pConnect = pConnect;
}

ConnectionsModel *ConnectionsViewSplitter::binding () const
{
	return m_pConnect;
}

JackClientList *ConnectionsViewSplitter::OClientList () const
{
	if (m_pConnect)
		return m_pConnect->OClientList();
	else
		return NULL;
}

JackClientList *ConnectionsViewSplitter::IClientList () const
{
	if (m_pConnect)
		return m_pConnect->OClientList();
	else
		return NULL;
}

void ConnectionsViewSplitter::setBezierLines ( bool bBezierLines )
{
	m_bBezierLines = bBezierLines;
}

bool ConnectionsViewSplitter::isBezierLines () const
{
	return m_bBezierLines;
}

void ConnectionsViewSplitter::setIconSize ( int iIconSize )
{
	// Update only if changed.
	if (iIconSize == m_iIconSize)
		return;

	// Go for it...
	m_iIconSize = iIconSize;

	// Update item sizes properly...
	int px = (16 << m_iIconSize);
	const QSize iconsize(px, px);
	m_pOListView->setIconSize(iconsize);
	m_pIListView->setIconSize(iconsize);

	// Call binding descendant implementation,
	// and do a complete content reset...
	if (m_pConnect)
		m_pConnect->updateContents(true);
}

int ConnectionsViewSplitter::iconSize () const
{
	return m_iIconSize;
}

void ConnectionsViewSplitter::setDirty ( bool bDirty )
{
	m_bDirty = bDirty;

	if (bDirty)
		emit contentsChanged();
}

bool ConnectionsViewSplitter::isDirty () const
{
	return m_bDirty;
}
