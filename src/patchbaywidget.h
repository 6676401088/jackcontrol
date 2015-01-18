/****************************************************************************
   Copyright (C) 2003-2011, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include "patchbaysplitter.h"
class Patchbay;
class Settings;

// uic includes
#include "ui_patchbaywidget.h"

class PatchbayWidget : public QWidget {
	Q_OBJECT
public:

	// Constructor.
    PatchbayWidget(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
    ~PatchbayWidget();

    void setup(Settings *pSetup);

    PatchbaySplitter *patchbaySplitter() const;

	bool queryClose();

	const QString& patchbayPath() const;

	void newPatchbayFile(bool bSnapshot);
	bool loadPatchbayFile(const QString& sFileName);
	bool savePatchbayFile(const QString& sFileName);

	void loadPatchbayRack(PatchbayRack *pRack);

	void setRecentPatchbays(const QStringList& patchbays);
	void updateRecentPatchbays();

public slots:

	void newPatchbay();
	void loadPatchbay();
	void savePatchbay();
	void selectPatchbay(int iPatchbay);
	void toggleActivePatchbay();

	void addOSocket();
	void removeOSocket();
	void editOSocket();
	void copyOSocket();
	void moveUpOSocket();
	void moveDownOSocket();

	void addISocket();
	void removeISocket();
	void editISocket();
	void copyISocket();
	void moveUpISocket();
	void moveDownISocket();

	void connectSelected();
	void disconnectSelected();
	void disconnectAll();

	void expandAll();

	void contentsChanged();

	void refreshForm();
	void stabilizeForm();

protected:

	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void closeEvent(QCloseEvent *);

	void keyPressEvent(QKeyEvent *);

private:
    Ui::PatchbayWidget m_ui;
    Settings    *m_pSetup;

	int               m_iUntitled;
    Patchbay *m_pPatchbay;
	QString           m_sPatchbayPath;
	QString           m_sPatchbayName;
	QStringList       m_recentPatchbays;
	bool              m_bActivePatchbay;

	int               m_iUpdate;
};

