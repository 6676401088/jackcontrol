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

#include <Qt>

// Own includes
#include "About.h"
#include "patchbaywidget.h"
#include "PatchbayFile.h"
#include "settings.h"
#include "MainWidget.h"

// Qt includes
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QPixmap>
#include <QShowEvent>
#include <QHideEvent>

PatchbayWidget::PatchbayWidget (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QWidget(pParent, wflags)
{
	// Setup UI struct...
	m_ui.setupUi(this);

	m_pSetup = NULL;

	// Create the patchbay view object.
    m_pPatchbay = new Patchbay(m_ui.patchbaySplitter);
	m_iUntitled = 0;

	m_bActivePatchbay = false;

	m_iUpdate = 0;

	// UI connections...

	connect(m_ui.NewPatchbayPushButton,
		SIGNAL(clicked()),
		SLOT(newPatchbay()));
	connect(m_ui.LoadPatchbayPushButton,
		SIGNAL(clicked()),
		SLOT(loadPatchbay()));
	connect(m_ui.SavePatchbayPushButton,
		SIGNAL(clicked()),
		SLOT(savePatchbay()));
	connect(m_ui.PatchbayComboBox,
		SIGNAL(activated(int)),
		SLOT(selectPatchbay(int)));
	connect(m_ui.ActivatePatchbayPushButton,
		SIGNAL(clicked()),
		SLOT(toggleActivePatchbay()));

	connect(m_ui.OSocketAddPushButton,
		SIGNAL(clicked()),
		SLOT(addOSocket()));
	connect(m_ui.OSocketEditPushButton,
		SIGNAL(clicked()),
		SLOT(editOSocket()));
	connect(m_ui.OSocketCopyPushButton,
		SIGNAL(clicked()),
		SLOT(copyOSocket()));
	connect(m_ui.OSocketRemovePushButton,
		SIGNAL(clicked()),
		SLOT(removeOSocket()));
	connect(m_ui.OSocketMoveUpPushButton,
		SIGNAL(clicked()),
		SLOT(moveUpOSocket()));
	connect(m_ui.OSocketMoveDownPushButton,
		SIGNAL(clicked()),
		SLOT(moveDownOSocket()));

	connect(m_ui.ISocketAddPushButton,
		SIGNAL(clicked()),
		SLOT(addISocket()));
	connect(m_ui.ISocketEditPushButton,
		SIGNAL(clicked()),
		SLOT(editISocket()));
	connect(m_ui.ISocketCopyPushButton,
		SIGNAL(clicked()),
		SLOT(copyISocket()));
	connect(m_ui.ISocketRemovePushButton,
		SIGNAL(clicked()),
		SLOT(removeISocket()));
	connect(m_ui.ISocketMoveUpPushButton,
		SIGNAL(clicked()),
		SLOT(moveUpISocket()));
	connect(m_ui.ISocketMoveDownPushButton,
		SIGNAL(clicked()),
		SLOT(moveDownISocket()));

	connect(m_ui.ConnectPushButton,
		SIGNAL(clicked()),
		SLOT(connectSelected()));
	connect(m_ui.DisconnectPushButton,
		SIGNAL(clicked()),
		SLOT(disconnectSelected()));
	connect(m_ui.DisconnectAllPushButton,
		SIGNAL(clicked()),
		SLOT(disconnectAll()));

	connect(m_ui.ExpandAllPushButton,
		SIGNAL(clicked()),
		SLOT(expandAll()));

	connect(m_ui.RefreshPushButton,
		SIGNAL(clicked()),
		SLOT(refreshForm()));

	// Connect it to some UI feedback slot.
    connect(m_ui.patchbaySplitter->OListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(stabilizeForm()));
    connect(m_ui.patchbaySplitter->IListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(stabilizeForm()));

	// Dirty patchbay dispatcher (stabilization deferral).
    connect(m_ui.patchbaySplitter,
		SIGNAL(contentsChanged()),
		SLOT(contentsChanged()));

	newPatchbayFile(false);
	stabilizeForm();
}

PatchbayWidget::~PatchbayWidget ()
{
	// May delete the patchbay view object.
	delete m_pPatchbay;
}

void PatchbayWidget::showEvent ( QShowEvent *pShowEvent )
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();

	stabilizeForm();

	QWidget::showEvent(pShowEvent);
}

void PatchbayWidget::hideEvent ( QHideEvent *pHideEvent )
{
	QWidget::hideEvent(pHideEvent);

    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();
}

void PatchbayWidget::closeEvent ( QCloseEvent * /*pCloseEvent*/ )
{
	QWidget::hide();

    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();
}

void PatchbayWidget::setup ( Setup *pSetup )
{
	m_pSetup = pSetup;

	// Load main splitter sizes...
	if (m_pSetup) {
		QList<int> sizes;
		sizes.append(180);
		sizes.append(60);
		sizes.append(180);
        m_pSetup->loadSplitterSizes(m_ui.patchbaySplitter, sizes);
	}
}

PatchbaySplitter *PatchbayWidget::patchbaySplitter () const
{
    return m_ui.patchbaySplitter;
}


// Window close event handlers.
bool PatchbayWidget::queryClose ()
{
	bool bQueryClose = true;

    if (m_ui.patchbaySplitter->dirty()) {
		switch (QMessageBox::warning(this,
			tr("Warning") + " - " QJACKCTL_SUBTITLE1,
			tr("The patchbay definition has been changed:\n\n"
			"\"%1\"\n\nDo you want to save the changes?")
			.arg(m_sPatchbayName),
			QMessageBox::Save |
			QMessageBox::Discard |
			QMessageBox::Cancel)) {
		case QMessageBox::Save:
			savePatchbay();
			// Fall thru....
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bQueryClose = false;
		}
	}

	// Save main splitter sizes...
	if (m_pSetup && bQueryClose)
        m_pSetup->saveSplitterSizes(m_ui.patchbaySplitter);

	return bQueryClose;
}


// Contents change deferrer slot...
void PatchbayWidget::contentsChanged ()
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->refreshPatchbay();
}


// Refresh complete form.
void PatchbayWidget::refreshForm ()
{
	m_pPatchbay->refresh();
	stabilizeForm();
}


// A helper stabilization slot.
void PatchbayWidget::stabilizeForm ( void )
{
    m_ui.SavePatchbayPushButton->setEnabled(m_ui.patchbaySplitter->dirty());
	m_ui.ActivatePatchbayPushButton->setEnabled(
		QFileInfo(m_sPatchbayPath).exists());

    MainWidget *pMainForm = MainWidget::getInstance();
	m_bActivePatchbay = (pMainForm
		&& pMainForm->isActivePatchbay(m_sPatchbayPath));
	m_ui.ActivatePatchbayPushButton->setChecked(m_bActivePatchbay);

    if (m_ui.patchbaySplitter->dirty()) {
		m_ui.PatchbayComboBox->setItemText(
			m_ui.PatchbayComboBox->currentIndex(),
			tr("%1 [modified]").arg(m_sPatchbayName));
	}

	// Take care that IT might be destroyed already...
    if (m_ui.patchbaySplitter->binding() == NULL)
		return;

	bool bExpandAll = false;

    SocketList *pSocketList;
    SocketTreeWidgetItem *pSocketItem;
	int iItemCount, iItem;

	pSocketList = m_pPatchbay->OSocketList();
	pSocketItem = pSocketList->selectedSocketItem();
	iItemCount  = (pSocketList->listView())->topLevelItemCount();
	bExpandAll  = bExpandAll || (iItemCount > 0);
	if (pSocketItem) {
		iItem = (pSocketList->listView())->indexOfTopLevelItem(pSocketItem);
		m_ui.OSocketEditPushButton->setEnabled(true);
		m_ui.OSocketCopyPushButton->setEnabled(true);
		m_ui.OSocketRemovePushButton->setEnabled(true);
		m_ui.OSocketMoveUpPushButton->setEnabled(iItem > 0);
		m_ui.OSocketMoveDownPushButton->setEnabled(iItem < iItemCount - 1);
	} else {
		m_ui.OSocketEditPushButton->setEnabled(false);
		m_ui.OSocketCopyPushButton->setEnabled(false);
		m_ui.OSocketRemovePushButton->setEnabled(false);
		m_ui.OSocketMoveUpPushButton->setEnabled(false);
		m_ui.OSocketMoveDownPushButton->setEnabled(false);
	}

	pSocketList = m_pPatchbay->ISocketList();
	pSocketItem = pSocketList->selectedSocketItem();
	iItemCount  = (pSocketList->listView())->topLevelItemCount();
	bExpandAll  = bExpandAll || (iItemCount > 0);
	if (pSocketItem) {
		iItem = (pSocketList->listView())->indexOfTopLevelItem(pSocketItem);
		m_ui.ISocketEditPushButton->setEnabled(true);
		m_ui.ISocketCopyPushButton->setEnabled(true);
		m_ui.ISocketRemovePushButton->setEnabled(true);
		m_ui.ISocketMoveUpPushButton->setEnabled(iItem > 0);
		m_ui.ISocketMoveDownPushButton->setEnabled(iItem < iItemCount - 1);
	} else {
		m_ui.ISocketEditPushButton->setEnabled(false);
		m_ui.ISocketCopyPushButton->setEnabled(false);
		m_ui.ISocketRemovePushButton->setEnabled(false);
		m_ui.ISocketMoveUpPushButton->setEnabled(false);
		m_ui.ISocketMoveDownPushButton->setEnabled(false);
	}

	m_ui.ConnectPushButton->setEnabled(
		m_pPatchbay->canConnectSelected());
	m_ui.DisconnectPushButton->setEnabled(
		m_pPatchbay->canDisconnectSelected());
	m_ui.DisconnectAllPushButton->setEnabled(
		m_pPatchbay->canDisconnectAll());
	m_ui.ExpandAllPushButton->setEnabled(bExpandAll);
}


// Patchbay path accessor.
const QString& PatchbayWidget::patchbayPath () const
{
	return m_sPatchbayPath;
}


// Reset patchbay definition from scratch.
void PatchbayWidget::newPatchbayFile ( bool bSnapshot )
{
	m_pPatchbay->clear();
	m_sPatchbayPath.clear();
	m_sPatchbayName = tr("Untitled%1").arg(m_iUntitled++);
	if (bSnapshot)
		m_pPatchbay->connectionsSnapshot();
//	updateRecentPatchbays();
}


// Load patchbay definitions from specific file path.
bool PatchbayWidget::loadPatchbayFile ( const QString& sFileName )
{
	// Check if we're going to discard safely the current one...
	if (!queryClose())
		return false;

	// We'll have a temporary rack...
    PatchbayRack rack;
	// Step 1: load from file...
	if (!qjackctlPatchbayFile::load(&rack, sFileName)) {
		QMessageBox::critical(this,
			tr("Error") + " - " QJACKCTL_SUBTITLE1,
			tr("Could not load patchbay definition file: \n\n\"%1\"")
			.arg(sFileName),
			QMessageBox::Cancel);
		// Reset/disable further trials.
		m_sPatchbayPath.clear();
		return false;
	}
	// Step 2: load from rack...
	m_pPatchbay->loadRack(&rack);

	// Step 3: stabilize form...
	m_sPatchbayPath = sFileName;
	m_sPatchbayName = QFileInfo(sFileName).completeBaseName();
//	updateRecentPatchbays();

	return true;
}


// Save current patchbay definition to specific file path.
bool PatchbayWidget::savePatchbayFile ( const QString& sFileName )
{
	// We'll have a temporary rack...
    PatchbayRack rack;
	// Step 1: save to rack...
	m_pPatchbay->saveRack(&rack);
	// Step 2: save to file...
	if (!qjackctlPatchbayFile::save(&rack, sFileName)) {
		QMessageBox::critical(this,
			tr("Error") + " - " QJACKCTL_SUBTITLE1,
			tr("Could not save patchbay definition file: \n\n\"%1\"")
			.arg(sFileName),
			QMessageBox::Cancel);
		return false;
	}
	// Step 3: stabilize form...
	m_sPatchbayPath = sFileName;
	m_sPatchbayName = QFileInfo(sFileName).completeBaseName();
//	updateRecentPatchbays();

	// Step 4: notify main form if applicable ...
    MainWidget *pMainForm = MainWidget::getInstance();
	m_bActivePatchbay = (pMainForm
		&& pMainForm->isActivePatchbay(m_sPatchbayPath));
	if (m_bActivePatchbay)
		pMainForm->updateActivePatchbay();

	return true;
}


// Dirty-(re)load patchbay definitions from know rack.
void PatchbayWidget::loadPatchbayRack ( PatchbayRack *pRack )
{
	// Step 1: load from rack...
	m_pPatchbay->loadRack(pRack);

	// Override dirty flag.
    m_ui.patchbaySplitter->setDirty(true);

	// Done.
	stabilizeForm();
}


// Create a new patchbay definition from scratch.
void PatchbayWidget::newPatchbay ()
{
	// Assume a snapshot from scratch...
	bool bSnapshot = false;

	// Ask user what he/she wants to do...
    MainWidget *pMainForm = MainWidget::getInstance();
    if (pMainForm && (pMainForm->jackClient() /*|| pMainForm->alsaSeq()*/)) {
		switch (QMessageBox::information(this,
			tr("New Patchbay definition") + " - " QJACKCTL_SUBTITLE1,
			tr("Create patchbay definition as a snapshot\n"
			"of all actual client connections?"),
			QMessageBox::Yes |
			QMessageBox::No |
			QMessageBox::Cancel)) {
		case QMessageBox::Yes:
			bSnapshot = true;
			break;
		case QMessageBox::No:
			bSnapshot = false;
			break;
		default:	// Cancel.
			return;
		}
	}

	// Check if we can discard safely the current one...
	if (!queryClose())
		return;

	// Reset patchbay editor.
	newPatchbayFile(bSnapshot);
	updateRecentPatchbays();
	stabilizeForm();
}


// Load patchbay definitions from file.
void PatchbayWidget::loadPatchbay ()
{
	QString sFileName = QFileDialog::getOpenFileName(
		this, tr("Load Patchbay Definition"),			// Parent & Caption.
		m_sPatchbayPath,								// Start here.
		tr("Patchbay Definition files") + " (*.xml)"	// Filter (XML files)
	);

	if (sFileName.isEmpty())
		return;

	// Load it right away.
	if (loadPatchbayFile(sFileName))
		updateRecentPatchbays();

	stabilizeForm();
}


// Save current patchbay definition to file.
void PatchbayWidget::savePatchbay ()
{
	QString sFileName = QFileDialog::getSaveFileName(
		this, tr("Save Patchbay Definition"),			// Parent & Caption.
		m_sPatchbayPath,								// Start here.
		tr("Patchbay Definition files") + " (*.xml)"	// Filter (XML files)
	);

	if (sFileName.isEmpty())
		return;

	// Enforce .xml extension...
	if (QFileInfo(sFileName).suffix().isEmpty())
		sFileName += ".xml";

	// Save it right away.
	if (savePatchbayFile(sFileName))
		updateRecentPatchbays();

	stabilizeForm();
}


// A new patchbay has been selected
void PatchbayWidget::selectPatchbay ( int iPatchbay )
{

	// Remember and avoid reloading the previous (first) selected one.
	if (iPatchbay > 0) {
		// Take care whether the first is untitled, and
		// thus not present in the recenet ptachbays list
		if (m_sPatchbayPath.isEmpty())
			iPatchbay--;
		if (iPatchbay >= 0 && iPatchbay < m_recentPatchbays.count()) {
			// If we cannot load the new one, backout...
			loadPatchbayFile(m_recentPatchbays[iPatchbay]);
			updateRecentPatchbays();
		}
	}

	stabilizeForm();
}


// Set current active patchbay definition file.
void PatchbayWidget::toggleActivePatchbay ()
{
	// Check if we're going to discard safely the current one...
	if (!queryClose())
		return;

	// Activate it...
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm) {
		pMainForm->setActivePatchbay(
			m_bActivePatchbay ? QString::null : m_sPatchbayPath);
	}

	// Need to force/refresh the patchbay list...
	updateRecentPatchbays();
	stabilizeForm();
}


// Set/initialize the MRU patchbay list.
void PatchbayWidget::setRecentPatchbays ( const QStringList& patchbays )
{
	m_recentPatchbays = patchbays;
}


// Update patchbay MRU variables and widgets.
void PatchbayWidget::updateRecentPatchbays ()
{
	// TRye not to be reeentrant.
	if (m_iUpdate > 0)
		return;

	m_iUpdate++;

	// Update the visible combobox...
	const QIcon icon(":/images/patchbay1.png");
	m_ui.PatchbayComboBox->clear();

	if (m_sPatchbayPath.isEmpty()) {
		// Display a probable untitled patchbay...
		m_ui.PatchbayComboBox->addItem(icon, m_sPatchbayName);
	} else  {
		// Remove from list if already there (avoid duplicates)...
		int iIndex = m_recentPatchbays.indexOf(m_sPatchbayPath);
		if (iIndex >= 0)
			m_recentPatchbays.removeAt(iIndex);
		// Put it to front...
		m_recentPatchbays.push_front(m_sPatchbayPath);
	}

	// Time to keep the list under limits.
	while (m_recentPatchbays.count() > 8)
		m_recentPatchbays.pop_back();

	// Update the main setup list...
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->setRecentPatchbays(m_recentPatchbays);

	QStringListIterator iter(m_recentPatchbays);
	while (iter.hasNext()) {
		const QString& sPatchbayPath = iter.next();
		QString sText = QFileInfo(sPatchbayPath).completeBaseName();
		if (pMainForm && pMainForm->isActivePatchbay(sPatchbayPath))
			sText += " [" + tr("active") + "]";
		m_ui.PatchbayComboBox->addItem(icon, sText);
	}

	// Sure this one must be currently selected.
	m_ui.PatchbayComboBox->setCurrentIndex(0);
//	stabilizeForm();

	m_iUpdate--;
}


// Output socket list push button handlers gallore...

void PatchbayWidget::addOSocket ()
{
	(m_pPatchbay->OSocketList())->addSocketItem();
}


void PatchbayWidget::removeOSocket ()
{
	(m_pPatchbay->OSocketList())->removeSocketItem();
}


void PatchbayWidget::editOSocket ()
{
	(m_pPatchbay->OSocketList())->editSocketItem();
}


void PatchbayWidget::copyOSocket ()
{
	(m_pPatchbay->OSocketList())->copySocketItem();
}


void PatchbayWidget::moveUpOSocket ()
{
	(m_pPatchbay->OSocketList())->moveUpSocketItem();
}


void PatchbayWidget::moveDownOSocket ()
{
	(m_pPatchbay->OSocketList())->moveDownSocketItem();
}


// Input socket list push button handlers gallore...

void PatchbayWidget::addISocket ()
{
	(m_pPatchbay->ISocketList())->addSocketItem();
}


void PatchbayWidget::removeISocket ()
{
	(m_pPatchbay->ISocketList())->removeSocketItem();
}


void PatchbayWidget::editISocket ()
{
	(m_pPatchbay->ISocketList())->editSocketItem();
}


void PatchbayWidget::copyISocket ()
{
	(m_pPatchbay->ISocketList())->copySocketItem();
}


void PatchbayWidget::moveUpISocket ()
{
	(m_pPatchbay->ISocketList())->moveUpSocketItem();
}


void PatchbayWidget::moveDownISocket ()
{
	(m_pPatchbay->ISocketList())->moveDownSocketItem();
}


// Connect current selected ports.
void PatchbayWidget::connectSelected ()
{
	m_pPatchbay->connectSelected();
}


// Disconnect current selected ports.
void PatchbayWidget::disconnectSelected ()
{
	m_pPatchbay->disconnectSelected();
}


// Disconnect all connected ports.
void PatchbayWidget::disconnectAll ()
{
	m_pPatchbay->disconnectAll();
}


// Expand all socket items.
void PatchbayWidget::expandAll ()
{
	m_pPatchbay->expandAll();
}


// Keyboard event handler.
void PatchbayWidget::keyPressEvent ( QKeyEvent *pKeyEvent )
{
#ifdef CONFIG_DEBUG_0
	qDebug("qjackctlPatchbayForm::keyPressEvent(%d)", pKeyEvent->key());
#endif
	int iKey = pKeyEvent->key();
	switch (iKey) {
	case Qt::Key_Escape:
		close();
		break;
	default:
		QWidget::keyPressEvent(pKeyEvent);
		break;
	}
}


// end of qjackctlPatchbayForm.cpp
