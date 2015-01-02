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

#include "ConnectAlias.h"

#include <QTreeWidget>
#include <QSplitter>


// QListViewItem::rtti return values.
#define QJACKCTL_CLIENTITEM    1001
#define QJACKCTL_PORTITEM      1002

// Forward declarations.
class JackPortTreeWidgetItem;
class JackClientTreeWidgetItem;
class JackClientList;
class ClientListTreeWidget;
class JackPortConnectorWidget;
class ConnectionsViewSplitter;
class ConnectionsModel;

class JackPortTreeWidgetItem : public QTreeWidgetItem
{
public:
    JackPortTreeWidgetItem(JackClientTreeWidgetItem *pClient, const QString& sPortName);
    ~JackPortTreeWidgetItem();

	// Instance accessors.
	void setPortName(const QString& sPortName);
	const QString& clientName() const;
	const QString& portName() const;

	// Complete client:port name helper.
	QString clientPortName() const;

	// Connections client item accessor.
    JackClientTreeWidgetItem *client() const;

	// Client port cleanup marker.
	void markPort(int iMark);
	void markClientPort(int iMark);

	int portMark() const;

	// Connected port list primitives.
    void addConnect(JackPortTreeWidgetItem *pPort);
    void removeConnect(JackPortTreeWidgetItem *pPort);

	// Connected port finders.
    JackPortTreeWidgetItem *findConnect(const QString& sClientPortName);
    JackPortTreeWidgetItem *findConnectPtr(JackPortTreeWidgetItem *pPortPtr);

	// Connection list accessor.
    const QList<JackPortTreeWidgetItem *>& connects() const;

	// Connectiopn highlight methods.
	bool isHilite() const;
	void setHilite (bool bHilite);

	// Proxy sort override method.
	// - Natural decimal sorting comparator.
	bool operator< (const QTreeWidgetItem& other) const;

private:

	// Instance variables.
    JackClientTreeWidgetItem *m_pClient;

	QString m_sPortName;
	int     m_iPortMark;
	bool    m_bHilite;

	// Connection cache list.
    QList<JackPortTreeWidgetItem *> m_connects;
};

class JackClientTreeWidgetItem : public QTreeWidgetItem
{
public:
    JackClientTreeWidgetItem(JackClientList *pClientList,
        const QString& sClientName);
    ~JackClientTreeWidgetItem();

	// Port list primitive methods.
    void addPort(JackPortTreeWidgetItem *pPort);
    void removePort(JackPortTreeWidgetItem *pPort);

	// Port finder.
    JackPortTreeWidgetItem *findPort(const QString& sPortName);

	// Instance accessors.
	void setClientName(const QString& sClientName);
	const QString& clientName() const;

	// Readable flag accessor.
	bool isReadable() const;

	// Client list accessor.
    JackClientList *clientList() const;

	// Port list accessor.
    QList<JackPortTreeWidgetItem *>& ports();

	// Client port cleanup marker.
	void markClient(int iMark);
	void markClientPorts(int iMark);
	int cleanClientPorts(int iMark);
	int clientMark() const;

	// Connectiopn highlight methods.
	bool isHilite() const;
	void setHilite (bool bHilite);

	// Client item openness status.
	void setOpen(bool bOpen);
	bool isOpen() const;

	// Proxy sort override method.
	// - Natural decimal sorting comparator.
	bool operator< (const QTreeWidgetItem& other) const;

private:

	// Instance variables.
    JackClientList *m_pClientList;

	QString m_sClientName;
	int     m_iClientMark;
	int     m_iHilite;

    QList<JackPortTreeWidgetItem *> m_ports;
};

class JackClientList : public QObject
{
public:
    JackClientList(ClientListTreeWidget *pListView, bool bReadable);
    ~JackClientList();

	// Do proper contents cleanup.
	void clear();

	// Client list primitive methods.
    void addClient(JackClientTreeWidgetItem *pClient);
    void removeClient(JackClientTreeWidgetItem *pClient);

	// Client finder.
    JackClientTreeWidgetItem *findClient(const QString& sClientName);
	// Client:port finder.
    JackPortTreeWidgetItem *findClientPort(const QString& sClientPort);

	// List view accessor.
    ClientListTreeWidget *listView() const;

	// Readable flag accessor.
	bool isReadable() const;

	// Client list accessor.
    QList<JackClientTreeWidgetItem *>& clients();

	// Client ports cleanup marker.
	void markClientPorts(int iMark);
	int cleanClientPorts(int iMark);

	// Client:port refreshner (return newest item count).
	virtual int updateClientPorts() = 0;

	// Client:port hilite update stabilization.
    void hiliteClientPorts ();

	// Do proper contents refresh/update.
	void refresh();

	// Natural decimal sorting comparator.
	static bool lessThan(const QTreeWidgetItem& i1, const QTreeWidgetItem& i2);

private:

	// Instance variables.
    ClientListTreeWidget *m_pListView;
	bool m_bReadable;

    QList<JackClientTreeWidgetItem *> m_clients;

	QTreeWidgetItem *m_pHiliteItem;
};

class ClientListTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
    ClientListTreeWidget(ConnectionsViewSplitter *pConnectView, bool bReadable);
    ~ClientListTreeWidget();

	// Auto-open timer methods.
	void setAutoOpenTimeout(int iAutoOpenTimeout);
	int autoOpenTimeout() const;

	// Aliasing support methods.
	void setAliases(qjackctlConnectAlias *pAliases, bool bRenameEnabled);
	qjackctlConnectAlias *aliases() const;
	bool renameEnabled() const;

	// Binding indirect accessor.
    ConnectionsModel *binding() const;

protected slots:

	// In-place aliasing slots.
	void startRenameSlot();
	void renamedSlot();
	// Auto-open timeout slot.
	void timeoutSlot();

protected:

	// Trap for help/tool-tip events.
	bool eventFilter(QObject *pObject, QEvent *pEvent);

	// Drag-n-drop stuff.
	QTreeWidgetItem *dragDropItem(const QPoint& pos);

	// Drag-n-drop stuff -- reimplemented virtual methods.
	void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
	void dragMoveEvent(QDragMoveEvent *pDragMoveEvent);
	void dragLeaveEvent(QDragLeaveEvent *);
	void dropEvent(QDropEvent *pDropEvent);

	// Handle mouse events for drag-and-drop stuff.
	void mousePressEvent(QMouseEvent *pMouseEvent);
	void mouseMoveEvent(QMouseEvent *pMouseEvent);

	// Context menu request event handler.
	void contextMenuEvent(QContextMenuEvent *);

private:

	// Bindings.
    ConnectionsViewSplitter *m_pConnectView;

	// Auto-open timer.
	int     m_iAutoOpenTimeout;
	QTimer *m_pAutoOpenTimer;

	// Items we'll eventually drop something.
	QTreeWidgetItem *m_pDragItem;
	QTreeWidgetItem *m_pDropItem;
	// The point from where drag started.
	QPoint m_posDrag;

	// Aliasing support.
	qjackctlConnectAlias *m_pAliases;
	bool m_bRenameEnabled;
};


class JackPortConnectorWidget : public QWidget
{
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


//----------------------------------------------------------------------------
// qjackctlConnect -- Connections model integrated object.

class ConnectionsModel : public QObject
{
	Q_OBJECT

public:
    ConnectionsModel(ConnectionsViewSplitter *pConnectView);
    ~ConnectionsModel();

	// Explicit connection tests.
	bool canConnectSelected();
	bool canDisconnectSelected();
	bool canDisconnectAll();

	// Client list accessors.
    JackClientList *OClientList() const;
    JackClientList *IClientList() const;

public slots:

	// Incremental contents refreshner; check dirty status.
	void refresh();

	// Explicit connection slots.
	bool connectSelected();
	bool disconnectSelected();
	bool disconnectAll();

	// Expand all client ports.
	void expandAll();

	// Complete/incremental contents rebuilder; check dirty status if incremental.
	void updateContents(bool bClear);

signals:

	// Connection change signal.
	void connectChanged();

	// Pre-notification of (dis)connection.
    void connecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);
    void disconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);

protected:

	// Connect/Disconnection primitives.
	virtual bool connectPorts(
        JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort) = 0;
	virtual bool disconnectPorts(
        JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort) = 0;

	// Update port connection references.
	virtual void updateConnections() = 0;

	// These must be accessed by the descendant constructor.
    ConnectionsViewSplitter *connectView() const;
    void setOClientList(JackClientList *pOClientList);
    void setIClientList(JackClientList *pIClientList);

	// Common pixmap factory helper-method.
	QPixmap *createIconPixmap (const QString& sIconName);

	// Update icon size implementation.
	virtual void updateIconPixmaps() = 0;

private:

	// Dunno. But this may avoid some conflicts.
	bool startMutex();
	void endMutex();

	// Connection methods (unguarded).
	bool canConnectSelectedEx();
	bool canDisconnectSelectedEx();
	bool canDisconnectAllEx();
	bool connectSelectedEx();
	bool disconnectSelectedEx();
	bool disconnectAllEx();

	// Connect/Disconnection local primitives.
    bool connectPortsEx(JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort);
    bool disconnectPortsEx(JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort);

	// Instance variables.
    ConnectionsViewSplitter *m_pConnectView;
	// These must be created on the descendant constructor.
    JackClientList *m_pOClientList;
    JackClientList *m_pIClientList;
	int m_iMutex;
};
