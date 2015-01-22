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

// uic includes
#include "ui_SocketDialog.h"

// Forward declarations.
class Patchbay;
class qjackctlPatchbaySocket;
class SocketList;

class QButtonGroup;
class QPixmap;

class SocketDialog : public QDialog
{
	Q_OBJECT
public:
    SocketDialog(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
    ~SocketDialog();

	void setSocketCaption(const QString& sSocketCaption);
    void setSocketList(SocketList *pSocketList);
	void setSocketNew(bool bSocketNew);

	void setPixmaps(QPixmap **ppPixmaps);

	void setConnectCount(int iConnectCount);

    void load(qjackctlPatchbaySocket *pSocket);
    void save(qjackctlPatchbaySocket *pSocket);

public slots:

	void changed();

	void addPlug();
	void editPlug();
	void removePlug();
	void moveUpPlug();
	void moveDownPlug();
	void selectedPlug();

	void activateAddPlugMenu(QAction *);

	void customContextMenu(const QPoint&);

	void socketTypeChanged();
	void clientNameChanged();

	void stabilizeForm();

protected slots:
	void accept();
	void reject();

protected:

	void updateJackClients(int iSocketType);
	void updateAlsaClients(int iSocketType);

	void updateJackPlugs(int iSocketType);
	void updateAlsaPlugs(int iSocketType);

	bool validateForm();

private:
	Ui::qjackctlSocketForm m_ui;

    SocketList          *m_pSocketList;
	bool                m_bSocketNew;
	QPixmap           **m_ppPixmaps;
	int                 m_iDirtyCount;

	QButtonGroup       *m_pSocketTypeButtonGroup;
};
