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
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>

// Own includes
#include "socketlist.h"
#include "sockettreewidget.h"
#include "Patchbay.h"
#include "SocketDialog.h"

SocketList::SocketList (
    SocketTreeWidget *pListView, bool bReadable ) {
    QPixmap pmXSocket1(":/images/xsocket1.png");

    m_pListView = pListView;
    m_bReadable = bReadable;

    if (bReadable) {
        m_sSocketCaption = tr("Output");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET]   = new QPixmap(":/images/asocketo.png");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET_X] = createPixmapMerge(*m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET], pmXSocket1);
        m_apPixmaps[QJACKCTL_XPM_AUDIO_CLIENT]   = new QPixmap(":/images/acliento.png");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_PLUG]     = new QPixmap(":/images/aportlno.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET]    = new QPixmap(":/images/msocketo.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET_X]  = createPixmapMerge(*m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET], pmXSocket1);
        m_apPixmaps[QJACKCTL_XPM_MIDI_CLIENT]    = new QPixmap(":/images/mcliento.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_PLUG]      = new QPixmap(":/images/mporto.png");
    } else {
        m_sSocketCaption = tr("Input");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET]   = new QPixmap(":/images/asocketi.png");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET_X] = createPixmapMerge(*m_apPixmaps[QJACKCTL_XPM_AUDIO_SOCKET], pmXSocket1);
        m_apPixmaps[QJACKCTL_XPM_AUDIO_CLIENT]   = new QPixmap(":/images/aclienti.png");
        m_apPixmaps[QJACKCTL_XPM_AUDIO_PLUG]     = new QPixmap(":/images/aportlni.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET]    = new QPixmap(":/images/msocketi.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET_X]  = createPixmapMerge(*m_apPixmaps[QJACKCTL_XPM_MIDI_SOCKET], pmXSocket1);
        m_apPixmaps[QJACKCTL_XPM_MIDI_CLIENT]    = new QPixmap(":/images/mclienti.png");
        m_apPixmaps[QJACKCTL_XPM_MIDI_PLUG]      = new QPixmap(":/images/mporti.png");
    }

    if (!m_sSocketCaption.isEmpty())
        m_sSocketCaption += ' ';
    m_sSocketCaption += tr("Socket");
}

// Default destructor.
SocketList::~SocketList ()
{
    clear();

    for (int iPixmap = 0; iPixmap < QJACKCTL_XPM_PIXMAPS; iPixmap++)
        delete m_apPixmaps[iPixmap];
}


// Client finder.
SocketTreeWidgetItem *SocketList::findSocket (
    const QString& sSocketName, int iSocketType )
{
    QListIterator<SocketTreeWidgetItem *> iter(m_sockets);
    while (iter.hasNext()) {
        SocketTreeWidgetItem *pSocket = iter.next();
        if (sSocketName == pSocket->socketName() &&
            iSocketType == pSocket->socketType())
            return pSocket;
    }

    return NULL;
}


// Socket list accessor.
QList<SocketTreeWidgetItem *>& SocketList::sockets ()
{
    return m_sockets;
}


// List view accessor.
SocketTreeWidget *SocketList::listView () const
{
    return m_pListView;
}


// Socket flags accessor.
bool SocketList::isReadable () const
{
    return m_bReadable;
}


// Socket caption titler.
const QString& SocketList::socketCaption () const
{
    return m_sSocketCaption;
}


// Socket list cleaner.
void SocketList::clear ()
{
    qDeleteAll(m_sockets);
    m_sockets.clear();
}


// Socket list pixmap peeker.
const QPixmap& SocketList::pixmap ( int iPixmap ) const
{
    return *m_apPixmaps[iPixmap];
}


// Merge two pixmaps with union of respective masks.
QPixmap *SocketList::createPixmapMerge (
    const QPixmap& xpmDst, const QPixmap& xpmSrc )
{
    QPixmap *pXpmMerge = new QPixmap(xpmDst);
    if (pXpmMerge) {
        QBitmap bmMask = xpmDst.mask();
        QPainter(&bmMask).drawPixmap(0, 0, xpmSrc.mask());
        pXpmMerge->setMask(bmMask);
        QPainter(pXpmMerge).drawPixmap(0, 0, xpmSrc);
    }
    return pXpmMerge;
}


// Return currently selected socket item.
SocketTreeWidgetItem *SocketList::selectedSocketItem () const
{
    SocketTreeWidgetItem *pSocketItem = NULL;

    QTreeWidgetItem *pItem = m_pListView->currentItem();
    if (pItem) {
        if (pItem->type() == QJACKCTL_PLUGITEM) {
            pSocketItem = static_cast<SocketTreeWidgetItem *> (pItem->parent());
        } else {
            pSocketItem = static_cast<SocketTreeWidgetItem *> (pItem);
        }
    }

    return pSocketItem;
}


// Add a new socket item, using interactive form.
bool SocketList::addSocketItem ()
{
    bool bResult = false;

    SocketDialog socketForm(m_pListView);
    socketForm.setWindowTitle(tr("<New> - %1").arg(m_sSocketCaption));
    socketForm.setSocketCaption(m_sSocketCaption);
    socketForm.setPixmaps(m_apPixmaps);
    socketForm.setSocketList(this);
    socketForm.setSocketNew(true);
    qjackctlPatchbaySocket socket(m_sSocketCaption
        + ' ' + QString::number(m_sockets.count() + 1),
        QString::null, QJACKCTL_SOCKETTYPE_JACK_AUDIO);
    socketForm.load(&socket);
    if (socketForm.exec()) {
        socketForm.save(&socket);
        SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    //  m_pListView->setUpdatesEnabled(false);
        if (pSocketItem)
            pSocketItem->setSelected(false);
        pSocketItem = new SocketTreeWidgetItem(this, socket.name(),
            socket.clientName(), socket.type(), pSocketItem);
        if (pSocketItem) {
            pSocketItem->setExclusive(socket.isExclusive());
            pSocketItem->setForward(socket.forward());
            PlugTreeWidgetItem *pPlugItem = NULL;
            QStringListIterator iter(socket.pluglist());
            while (iter.hasNext()) {
                pPlugItem = new PlugTreeWidgetItem(
                    pSocketItem, iter.next(), pPlugItem);
            }
            bResult = true;
        }
        pSocketItem->setSelected(true);
        m_pListView->setCurrentItem(pSocketItem);
    //  m_pListView->setUpdatesEnabled(true);
    //  m_pListView->update();
        m_pListView->setDirty(true);
    }

    return bResult;
}


// Remove (delete) currently selected socket item.
bool SocketList::removeSocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        if (QMessageBox::warning(m_pListView,
            tr("Warning") + " - " QJACKCTL_SUBTITLE1,
            tr("%1 about to be removed:\n\n"
            "\"%2\"\n\nAre you sure?")
            .arg(m_sSocketCaption)
            .arg(pSocketItem->socketName()),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            delete pSocketItem;
            bResult = true;
            m_pListView->setDirty(true);
        }
    }

    return bResult;
}

bool SocketList::editSocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        SocketDialog socketForm(m_pListView);
        socketForm.setWindowTitle(pSocketItem->socketName()
            + " - " + m_sSocketCaption);
        socketForm.setSocketCaption(m_sSocketCaption);
        socketForm.setPixmaps(m_apPixmaps);
        socketForm.setSocketList(this);
        socketForm.setSocketNew(false);
        qjackctlPatchbaySocket socket(pSocketItem->socketName(),
            pSocketItem->clientName(), pSocketItem->socketType());
        socket.setExclusive(pSocketItem->isExclusive());
        socket.setForward(pSocketItem->forward());
        QListIterator<PlugTreeWidgetItem *> iter(pSocketItem->plugs());
        while (iter.hasNext())
            socket.pluglist().append((iter.next())->plugName());
        socketForm.load(&socket);
        socketForm.setConnectCount(pSocketItem->connects().count());
        if (socketForm.exec()) {
            socketForm.save(&socket);
        //  m_pListView->setUpdatesEnabled(false);
            pSocketItem->clear();
            pSocketItem->setText(0, socket.name());
            pSocketItem->setSocketName(socket.name());
            pSocketItem->setClientName(socket.clientName());
            pSocketItem->setSocketType(socket.type());
            pSocketItem->setExclusive(socket.isExclusive());
            pSocketItem->setForward(socket.forward());
            pSocketItem->updatePixmap();
            PlugTreeWidgetItem *pPlugItem = NULL;
            QStringListIterator iter(socket.pluglist());
            while (iter.hasNext()) {
                pPlugItem = new PlugTreeWidgetItem(
                    pSocketItem, iter.next(), pPlugItem);
            }
            pSocketItem->setSelected(true);
            m_pListView->setCurrentItem(pSocketItem);
        //  m_pListView->setUpdatesEnabled(true);
        //  m_pListView->triggerUpdate();
            m_pListView->setDirty(true);
            bResult = true;
        }
    }

    return bResult;
}

bool SocketList::copySocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        SocketDialog socketForm(m_pListView);
        // Find a new distinguishable socket name, please.
        QString sSocketName;
        QString sSkel = pSocketItem->socketName();
        sSkel.remove(QRegExp("[0-9]+$")).append("%1");
        int iSocketType = pSocketItem->socketType();
        int iSocketNo = 1;
        do { sSocketName = sSkel.arg(++iSocketNo); }
        while (findSocket(sSocketName, iSocketType));
        // Show up as a new socket...
        socketForm.setWindowTitle(tr("%1 <Copy> - %2")
            .arg(pSocketItem->socketName()).arg(m_sSocketCaption));
        socketForm.setSocketCaption(m_sSocketCaption);
        socketForm.setPixmaps(m_apPixmaps);
        socketForm.setSocketList(this);
        socketForm.setSocketNew(true);
        qjackctlPatchbaySocket socket(sSocketName,
            pSocketItem->clientName(), iSocketType);
        if (pSocketItem->isExclusive())
            socket.setExclusive(true);
        QListIterator<PlugTreeWidgetItem *> iter(pSocketItem->plugs());
        while (iter.hasNext())
            socket.pluglist().append((iter.next())->plugName());
        socketForm.load(&socket);
        if (socketForm.exec()) {
            socketForm.save(&socket);
            pSocketItem = new SocketTreeWidgetItem(this, socket.name(),
                socket.clientName(), socket.type(), pSocketItem);
            if (pSocketItem) {
                pSocketItem->setExclusive(socket.isExclusive());
                pSocketItem->setForward(socket.forward());
                PlugTreeWidgetItem *pPlugItem = NULL;
                QStringListIterator iter(socket.pluglist());
                while (iter.hasNext()) {
                    pPlugItem = new PlugTreeWidgetItem(
                        pSocketItem, iter.next(), pPlugItem);
                }
                bResult = true;
            }
            pSocketItem->setSelected(true);
            m_pListView->setCurrentItem(pSocketItem);
        //  m_pListView->setUpdatesEnabled(true);
        //  m_pListView->triggerUpdate();
            m_pListView->setDirty(true);
        }
    }

    return bResult;
}


// Toggle exclusive currently selected socket item.
bool SocketList::exclusiveSocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        pSocketItem->setExclusive(!pSocketItem->isExclusive());
        pSocketItem->updatePixmap();
        bResult = true;
        m_pListView->setDirty(true);
    }

    return bResult;
}


// Move current selected socket item up one position.
bool SocketList::moveUpSocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        int iItem = m_pListView->indexOfTopLevelItem(pSocketItem);
        if (iItem > 0) {
            QTreeWidgetItem *pItem = m_pListView->takeTopLevelItem(iItem);
            if (pItem) {
                m_pListView->insertTopLevelItem(iItem - 1, pItem);
                pSocketItem->setSelected(true);
                m_pListView->setCurrentItem(pSocketItem);
            //  m_pListView->setUpdatesEnabled(true);
            //  m_pListView->update();
                m_pListView->setDirty(true);
                bResult = true;
            }
        }
    }

    return bResult;
}


// Move current selected socket item down one position.
bool SocketList::moveDownSocketItem ()
{
    bool bResult = false;

    SocketTreeWidgetItem *pSocketItem = selectedSocketItem();
    if (pSocketItem) {
        int iItem = m_pListView->indexOfTopLevelItem(pSocketItem);
        int iItemCount = m_pListView->topLevelItemCount();
        if (iItem < iItemCount - 1) {
            QTreeWidgetItem *pItem = m_pListView->takeTopLevelItem(iItem);
            if (pItem) {
                m_pListView->insertTopLevelItem(iItem + 1, pItem);
                pSocketItem->setSelected(true);
                m_pListView->setCurrentItem(pSocketItem);
            //  m_pListView->setUpdatesEnabled(true);
            //  m_pListView->update();
                m_pListView->setDirty(true);
                bResult = true;
            }
        }
    }

    return bResult;
}
