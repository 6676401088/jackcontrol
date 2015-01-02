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

#include <QTreeWidget>
#include <QSplitter>

// Our external patchbay models.
#include "PatchbayRack.h"

// QListViewItem::rtti return values.
#define QJACKCTL_SOCKETITEM 2001
#define QJACKCTL_PLUGITEM   2002

// Forward declarations.
class PlugTreeWidgetItem;
class SocketTreeWidgetItem;
class SocketList;
class SocketTreeWidget;
class PatchworkWidget;
class PatchbaySplitter;
class Patchbay;

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


class PlugTreeWidgetItem : public QTreeWidgetItem
{
public:

	// Constructor.
    PlugTreeWidgetItem(SocketTreeWidgetItem *pSocket,
        const QString& sPlugName, PlugTreeWidgetItem *pPlugAfter);
	// Default destructor.
    ~PlugTreeWidgetItem();

	// Instance accessors.
	const QString& socketName() const;
	const QString& plugName() const;

	// Patchbay socket item accessor.
    SocketTreeWidgetItem *socket() const;

private:

	// Instance variables.
    SocketTreeWidgetItem *m_pSocket;
	QString m_sPlugName;
};


class SocketTreeWidgetItem : public QTreeWidgetItem
{
public:

	// Constructor.
    SocketTreeWidgetItem(SocketList *pSocketList,
		const QString& sSocketName, const QString& sClientName,
        int iSocketType,SocketTreeWidgetItem *pSocketAfter);

	// Default destructor.
    ~SocketTreeWidgetItem();

	// Instance accessors.
	const QString& socketName() const;
	const QString& clientName() const;
	int socketType() const;
	bool isExclusive() const;
	const QString& forward() const;

	void setSocketName (const QString& sSocketName);
	void setClientName (const QString& sClientName);
	void setSocketType (int iSocketType);
	void setExclusive  (bool bExclusive);
	void setForward    (const QString& sSocketForward);

	// Socket flags accessor.
	bool isReadable() const;

	// Connected plug list primitives.
    void addConnect(SocketTreeWidgetItem *pSocket);
    void removeConnect(SocketTreeWidgetItem *pSocket);

	// Connected plug  finders.
    SocketTreeWidgetItem *findConnectPtr(SocketTreeWidgetItem *pSocketPtr);

	// Connection list accessor.
    const QList<SocketTreeWidgetItem *>& connects() const;

	// Plug list primitive methods.
    void addPlug(PlugTreeWidgetItem *pPlug);
    void removePlug(PlugTreeWidgetItem *pPlug);

	// Plug finder.
    PlugTreeWidgetItem *findPlug(const QString& sPlugName);

	// Plug list accessor.
    QList<PlugTreeWidgetItem *>& plugs();

	// Plug list cleaner.
	void clear();

	// Retrieve a context pixmap.
	const QPixmap& pixmap(int iPixmap) const;

	// Update pixmap to its proper context.
	void updatePixmap();

	// Client item openness status.
	void setOpen(bool bOpen);
	bool isOpen() const;

private:

	// Instance variables.
    SocketList *m_pSocketList;
	QString m_sSocketName;
	QString m_sClientName;
	int m_iSocketType;
	bool m_bExclusive;
	QString m_sSocketForward;

	// Plug (port) list.
    QList<PlugTreeWidgetItem *> m_plugs;

	// Connection cache list.
    QList<SocketTreeWidgetItem *> m_connects;
};

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


class SocketTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
    SocketTreeWidget(PatchbaySplitter *pPatchbayView, bool bReadable);
    ~SocketTreeWidget();

	// Patchbay dirty flag accessors.
	void setDirty (bool bDirty);
	bool dirty() const;
	
	// Auto-open timer methods.
	void setAutoOpenTimeout(int iAutoOpenTimeout);
	int autoOpenTimeout() const;

protected slots:

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
    PatchbaySplitter *m_pPatchbayView;

	bool m_bReadable;

	// Auto-open timer.
	int    m_iAutoOpenTimeout;
	QTimer *m_pAutoOpenTimer;

	// Items we'll eventually drop something.
	QTreeWidgetItem *m_pDragItem;
	QTreeWidgetItem *m_pDropItem;
	// The point from where drag started.
	QPoint m_posDrag;
};

class PatchworkWidget : public QWidget
{
	Q_OBJECT

public:
    PatchworkWidget(PatchbaySplitter *pPatchbayView);
    ~PatchworkWidget();

public slots:

	// Useful slots (should this be protected?).
	void contentsChanged();

protected:

	// Draw visible port connection relation arrows.
	void paintEvent(QPaintEvent *);

	// Context menu request event handler.
	void contextMenuEvent(QContextMenuEvent *);

private:

	// Legal socket item position helper.
	int itemY(QTreeWidgetItem *pItem) const;

	// Drawing methods.
	void drawForwardLine(QPainter *pPainter,
		int x, int dx, int y1, int y2, int h);
	void drawConnectionLine(QPainter *pPainter,
		int x1, int y1, int x2, int y2, int h1, int h2);

	// Local instance variables.
    PatchbaySplitter *m_pPatchbayView;
};

class PatchbaySplitter : public QSplitter
{
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

class Patchbay : public QObject
{
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
