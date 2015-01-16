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

// Qt includes
#include <QObject>
#include <QScrollBar>
#include <QMenu>

// Own includes
#include "patchbaysplitter.h"

PatchbaySplitter::PatchbaySplitter ( QWidget *pParent )
    : QSplitter(Qt::Horizontal, pParent) {
    m_pOListView = new SocketTreeWidget(this, true);
    m_pPatchworkView = new PatchworkWidget(this);
    m_pIListView = new SocketTreeWidget(this, false);

    m_pPatchbay = NULL;

    m_bBezierLines = false;

    connect(m_pOListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        m_pPatchworkView, SLOT(contentsChanged()));
    connect(m_pOListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        m_pPatchworkView, SLOT(contentsChanged()));
    connect(m_pOListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
        m_pPatchworkView, SLOT(contentsChanged()));
//	connect(m_pOListView->header(), SIGNAL(sectionClicked(int)),
//		m_pPatchworkView, SLOT(contentsChanged()));

    connect(m_pIListView, SIGNAL(itemExpanded(QTreeWidgetItem *)),
        m_pPatchworkView, SLOT(contentsChanged()));
    connect(m_pIListView, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
        m_pPatchworkView, SLOT(contentsChanged()));
    connect(m_pIListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
        m_pPatchworkView, SLOT(contentsChanged()));
//	connect(m_pIListView->header(), SIGNAL(sectionClicked(int)),
//		m_pPatchworkView, SLOT(contentsChanged()));

    m_bDirty = false;
}

PatchbaySplitter::~PatchbaySplitter() {
}

void PatchbaySplitter::contextMenu(const QPoint& pos, SocketList *pSocketList) {
    Patchbay *pPatchbay = binding();
    if (pPatchbay == NULL)
        return;

    QMenu menu(this);
    QAction *pAction;

    if (pSocketList) {
        SocketTreeWidgetItem *pSocketItem = pSocketList->selectedSocketItem();
        bool bEnabled = (pSocketItem != NULL);
        pAction = menu.addAction(QIcon(":/images/add1.png"),
            tr("Add..."), pSocketList, SLOT(addSocketItem()));
        pAction = menu.addAction(QIcon(":/images/edit1.png"),
            tr("Edit..."), pSocketList, SLOT(editSocketItem()));
        pAction->setEnabled(bEnabled);
        pAction = menu.addAction(QIcon(":/images/copy1.png"),
            tr("Copy..."), pSocketList, SLOT(copySocketItem()));
        pAction->setEnabled(bEnabled);
        pAction = menu.addAction(QIcon(":/images/remove1.png"),
            tr("Remove"), pSocketList, SLOT(removeSocketItem()));
        pAction->setEnabled(bEnabled);
        menu.addSeparator();
        pAction = menu.addAction(
            tr("Exclusive"), pSocketList, SLOT(exclusiveSocketItem()));
        pAction->setCheckable(true);
        pAction->setChecked(bEnabled && pSocketItem->isExclusive());
        pAction->setEnabled(bEnabled && (pSocketItem->connects().count() < 2));
        // Construct the forwarding menu,
        // overriding the last one, if any...
        QMenu *pForwardMenu = menu.addMenu(tr("Forward"));
        // Assume sockets iteration follows item index order (0,1,2...)
        // and remember that we only do this for input sockets...
        int iIndex = 0;
        if (pSocketItem && pSocketList == ISocketList()) {
            QListIterator<SocketTreeWidgetItem *> isocket(ISocketList()->sockets());
            while (isocket.hasNext()) {
                SocketTreeWidgetItem *pISocket = isocket.next();
                // Must be of same type of target one...
                int iSocketType = pISocket->socketType();
                if (iSocketType != pSocketItem->socketType())
                    continue;
                const QString& sSocketName = pISocket->socketName();
                if (pSocketItem->socketName() == sSocketName)
                    continue;
                int iPixmap = 0;
                switch (iSocketType) {
                case QJACKCTL_SOCKETTYPE_JACK_AUDIO:
                    iPixmap = (pISocket->isExclusive()
                        ? QJACKCTL_XPM_AUDIO_SOCKET_X
                        : QJACKCTL_XPM_AUDIO_SOCKET);
                    break;
                case QJACKCTL_SOCKETTYPE_JACK_MIDI:
                case QJACKCTL_SOCKETTYPE_ALSA_MIDI:
                    iPixmap = (pISocket->isExclusive()
                        ? QJACKCTL_XPM_MIDI_SOCKET_X
                        : QJACKCTL_XPM_MIDI_SOCKET);
                    break;
                }
                pAction = pForwardMenu->addAction(
                    QIcon(ISocketList()->pixmap(iPixmap)), sSocketName);
                pAction->setChecked(pSocketItem->forward() == sSocketName);
                pAction->setData(iIndex);
                iIndex++;
            }
            // Null forward always present,
            // and has invalid index parameter (-1)...
            if (iIndex > 0)
                pForwardMenu->addSeparator();
            pAction = pForwardMenu->addAction(tr("(None)"));
            pAction->setCheckable(true);
            pAction->setChecked(pSocketItem->forward().isEmpty());
            pAction->setData(-1);
            // We have something here...
            connect(pForwardMenu,
                SIGNAL(triggered(QAction*)),
                SLOT(activateForwardMenu(QAction*)));
        }
        pForwardMenu->setEnabled(iIndex > 0);
        menu.addSeparator();
        int iItem = (pSocketList->listView())->indexOfTopLevelItem(pSocketItem);
        int iItemCount = (pSocketList->listView())->topLevelItemCount();
        pAction = menu.addAction(QIcon(":/images/up1.png"),
            tr("Move Up"), pSocketList, SLOT(moveUpSocketItem()));
        pAction->setEnabled(bEnabled && iItem > 0);
        pAction = menu.addAction(QIcon(":/images/down1.png"),
            tr("Move Down"), pSocketList, SLOT(moveDownSocketItem()));
        pAction->setEnabled(bEnabled && iItem < iItemCount - 1);
        menu.addSeparator();
    }

    pAction = menu.addAction(QIcon(":/images/connect1.png"),
        tr("&Connect"), pPatchbay, SLOT(connectSelected()),
        tr("Alt+C", "Connect"));
    pAction->setEnabled(pPatchbay->canConnectSelected());
    pAction = menu.addAction(QIcon(":/images/disconnect1.png"),
        tr("&Disconnect"), pPatchbay, SLOT(disconnectSelected()),
        tr("Alt+D", "Disconnect"));
    pAction->setEnabled(pPatchbay->canDisconnectSelected());
    pAction = menu.addAction(QIcon(":/images/disconnectall1.png"),
        tr("Disconnect &All"), pPatchbay, SLOT(disconnectAll()),
        tr("Alt+A", "Disconect All"));
    pAction->setEnabled(pPatchbay->canDisconnectAll());

    menu.addSeparator();
    pAction = menu.addAction(QIcon(":/images/refresh1.png"),
        tr("&Refresh"), pPatchbay, SLOT(refresh()),
        tr("Alt+R", "Refresh"));

    menu.exec(pos);
}

void PatchbaySplitter::activateForwardMenu(QAction *pAction) {
    int iIndex = pAction->data().toInt();

    // Get currently input socket (assume its nicely selected)
    SocketTreeWidgetItem *pSocketItem = ISocketList()->selectedSocketItem();
    if (pSocketItem) {
        // Check first for forward from nil...
        if (iIndex < 0) {
            pSocketItem->setForward(QString::null);
            setDirty(true);
            return;
        }
        // Hopefully, its a real socket about to be forwraded...
        QListIterator<SocketTreeWidgetItem *> isocket(ISocketList()->sockets());
        while (isocket.hasNext()) {
            SocketTreeWidgetItem *pISocket = isocket.next();
            // Must be of same type of target one...
            if (pISocket->socketType() != pSocketItem->socketType())
                continue;
            const QString& sSocketName = pISocket->socketName();
            if (pSocketItem->socketName() == sSocketName)
                continue;
            if (iIndex == 0) {
                pSocketItem->setForward(sSocketName);
                setDirty(true);
                break;
            }
            iIndex--;
        }
    }
}

void PatchbaySplitter::setBinding(Patchbay *pPatchbay) {
    m_pPatchbay = pPatchbay;
}

Patchbay *PatchbaySplitter::binding() const {
    return m_pPatchbay;
}

SocketList *PatchbaySplitter::OSocketList () const
{
    if (m_pPatchbay)
        return m_pPatchbay->OSocketList();
    else
        return NULL;
}

SocketList *PatchbaySplitter::ISocketList () const
{
    if (m_pPatchbay)
        return m_pPatchbay->ISocketList();
    else
        return NULL;
}


// Patchwork line style accessors.
void PatchbaySplitter::setBezierLines ( bool bBezierLines )
{
    m_bBezierLines = bBezierLines;
}

bool PatchbaySplitter::isBezierLines () const
{
    return m_bBezierLines;
}

void PatchbaySplitter::setDirty ( bool bDirty )
{
    m_bDirty = bDirty;
    if (bDirty)
        emit contentsChanged();
}

bool PatchbaySplitter::dirty () const
{
    return m_bDirty;
}
