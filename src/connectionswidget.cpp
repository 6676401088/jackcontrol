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
#include "connectionswidget.h"
#include "settings.h"
#include "MainWidget.h"
#include "Patchbay.h"

// Qt includes
#include <QMessageBox>
#include <QShowEvent>
#include <QHideEvent>

ConnectionsWidget::ConnectionsWidget(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);

    m_pAudioConnect = new JackConnectionsModel(
        ui.AudioConnectView, QJACKCTL_JACK_AUDIO);
#ifdef CONFIG_JACK_MIDI
    m_pMidiConnect = new JackConnectionsModel(
        ui.MidiConnectView, QJACKCTL_JACK_MIDI);
#else
	m_pMidiConnect = NULL;
#endif

#ifdef CONFIG_ALSA_SEQ
    m_pAlsaConnect = new qjackctlAlsaConnect(ui.AlsaConnectView);
#else
	m_pAlsaConnect = NULL;
#endif

    m_pSetup = NULL;

    connect(ui.AudioConnectPushButton,
		SIGNAL(clicked()),
		SLOT(audioConnectSelected()));
    connect(ui.AudioDisconnectPushButton,
		SIGNAL(clicked()),
		SLOT(audioDisconnectSelected()));
    connect(ui.AudioDisconnectAllPushButton,
		SIGNAL(clicked()),
		SLOT(audioDisconnectAll()));
    connect(ui.AudioExpandAllPushButton,
		SIGNAL(clicked()),
		SLOT(audioExpandAll()));
    connect(ui.AudioRefreshPushButton,
		SIGNAL(clicked()),
		SLOT(audioRefreshClear()));

    connect(ui.MidiConnectPushButton,
		SIGNAL(clicked()),
		SLOT(midiConnectSelected()));
    connect(ui.MidiDisconnectPushButton,
		SIGNAL(clicked()),
		SLOT(midiDisconnectSelected()));
    connect(ui.MidiDisconnectAllPushButton,
		SIGNAL(clicked()),
		SLOT(midiDisconnectAll()));
    connect(ui.MidiExpandAllPushButton,
		SIGNAL(clicked()),
		SLOT(midiExpandAll()));
    connect(ui.MidiRefreshPushButton,
		SIGNAL(clicked()),
		SLOT(midiRefreshClear()));

    connect(ui.AlsaConnectPushButton,
		SIGNAL(clicked()),
		SLOT(alsaConnectSelected()));
    connect(ui.AlsaDisconnectPushButton,
		SIGNAL(clicked()),
		SLOT(alsaDisconnectSelected()));
    connect(ui.AlsaDisconnectAllPushButton,
		SIGNAL(clicked()),
		SLOT(alsaDisconnectAll()));
    connect(ui.AlsaExpandAllPushButton,
		SIGNAL(clicked()),
		SLOT(alsaExpandAll()));
    connect(ui.AlsaRefreshPushButton,
		SIGNAL(clicked()),
		SLOT(alsaRefreshClear()));

	// Connect it to some UI feedback slots.
    connect(ui.AudioConnectView->OListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(audioStabilize()));
    connect(ui.AudioConnectView->IListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(audioStabilize()));
    connect(ui.MidiConnectView->OListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(midiStabilize()));
    connect(ui.MidiConnectView->IListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(midiStabilize()));
    connect(ui.AlsaConnectView->OListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(alsaStabilize()));
    connect(ui.AlsaConnectView->IListView(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(alsaStabilize()));

	// Dirty dispatcher (refresh deferral).
    connect(ui.AudioConnectView,
		SIGNAL(contentsChanged()),
		SLOT(audioRefresh()));
    connect(ui.MidiConnectView,
		SIGNAL(contentsChanged()),
		SLOT(midiRefresh()));
    connect(ui.AlsaConnectView,
		SIGNAL(contentsChanged()),
		SLOT(alsaRefresh()));

	// Actual connections...
	connect(m_pAudioConnect,
        SIGNAL(disconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)),
        SLOT(audioDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)));
#ifdef CONFIG_JACK_MIDI
	connect(m_pMidiConnect,
        SIGNAL(disconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)),
        SLOT(midiDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)));
#endif
#ifdef CONFIG_ALSA_SEQ
	connect(m_pAlsaConnect,
        SIGNAL(disconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)),
        SLOT(alsaDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *)));
#endif

#ifndef CONFIG_JACK_MIDI
	m_ui.ConnectionsTabWidget->setTabEnabled(1, false);
#endif
#ifndef CONFIG_ALSA_SEQ
//	m_ui.ConnectionsTabWidget->setTabEnabled(2, false);
	m_ui.ConnectionsTabWidget->removeTab(2);
#endif

	// Start disabled.
	stabilizeAudio(false);
	stabilizeMidi(false);
	stabilizeAlsa(false);
}

ConnectionsWidget::~ConnectionsWidget ()
{
	// Destroy our connections view...
	if (m_pAudioConnect)
		delete m_pAudioConnect;
#ifdef CONFIG_JACK_MIDI
	if (m_pMidiConnect)
		delete m_pMidiConnect;
#endif
#ifdef CONFIG_ALSA_SEQ
	if (m_pAlsaConnect)
		delete m_pAlsaConnect;
#endif
}

void ConnectionsWidget::showEvent ( QShowEvent *pShowEvent )
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();

	audioRefresh();
	midiRefresh();

	alsaRefresh();

	QWidget::showEvent(pShowEvent);
}

void ConnectionsWidget::hideEvent ( QHideEvent *pHideEvent )
{
	QWidget::hideEvent(pHideEvent);

    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();
}

void ConnectionsWidget::closeEvent ( QCloseEvent * /*pCloseEvent*/ )
{
	QWidget::hide();

    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->stabilizeForm();
}

void ConnectionsWidget::setup ( Setup *pSetup )
{
	m_pSetup = pSetup;

    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm) {
		connect(m_pAudioConnect, SIGNAL(connectChanged()),
			pMainForm, SLOT(jackConnectChanged()));
	#ifdef CONFIG_JACK_MIDI
		connect(m_pMidiConnect, SIGNAL(connectChanged()),
			pMainForm, SLOT(jackConnectChanged()));
	#endif
	#ifdef CONFIG_ALSA_SEQ
		connect(m_pAlsaConnect, SIGNAL(connectChanged()),
			pMainForm, SLOT(alsaConnectChanged()));
	#endif
	}

	// Load some splitter sizes...
	if (m_pSetup) {
		QList<int> sizes;
		sizes.append(180);
		sizes.append(60);
		sizes.append(180);
        m_pSetup->loadSplitterSizes(ui.AudioConnectView, sizes);
        m_pSetup->loadSplitterSizes(ui.MidiConnectView, sizes);
        m_pSetup->loadSplitterSizes(ui.AlsaConnectView, sizes);
	#ifdef CONFIG_ALSA_SEQ
		if (!m_pSetup->bAlsaSeqEnabled) {
		//	m_ui.ConnectionsTabWidget->setTabEnabled(2, false);
            ui.ConnectionsTabWidget->removeTab(2);
		}
	#endif
	}

	// Update initial client/port aliases...
	updateAliases();
}

ConnectionsViewSplitter *ConnectionsWidget::audioConnectView () const
{
    return ui.AudioConnectView;
}

ConnectionsViewSplitter *ConnectionsWidget::midiConnectView () const
{
    return ui.MidiConnectView;
}

ConnectionsViewSplitter *ConnectionsWidget::alsaConnectView () const
{
    return ui.AlsaConnectView;
}

bool ConnectionsWidget::queryClose ()
{
	bool bQueryClose = true;

	if (m_pSetup
        && (ui.AudioConnectView->isDirty() ||
            ui.MidiConnectView->isDirty()  ||
            ui.AlsaConnectView->isDirty())) {
		switch (QMessageBox::warning(this,
			tr("Warning") + " - " QJACKCTL_SUBTITLE1,
			tr("The preset aliases have been changed:\n\n"
			"\"%1\"\n\nDo you want to save the changes?")
			.arg(m_sPreset),
			QMessageBox::Save |
			QMessageBox::Discard |
			QMessageBox::Cancel)) {
		case QMessageBox::Save:
			saveAliases();
			// Fall thru....
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bQueryClose = false;
		}
	}

	// Save some splitter sizes...
	if (m_pSetup && bQueryClose) {
        m_pSetup->saveSplitterSizes(ui.AudioConnectView);
        m_pSetup->saveSplitterSizes(ui.MidiConnectView);
        m_pSetup->saveSplitterSizes(ui.AlsaConnectView);
	}

	return bQueryClose;
}

bool ConnectionsWidget::loadAliases ()
{
	bool bResult = false;
	
	if (m_pSetup && queryClose()) {
		m_sPreset = m_pSetup->sDefPreset;
		bResult = m_pSetup->loadAliases(m_sPreset);
		if (bResult) {
            ui.AudioConnectView->setDirty(false);
            ui.MidiConnectView->setDirty(false);
            ui.AlsaConnectView->setDirty(false);
		}
	}

	return bResult;
}

bool ConnectionsWidget::saveAliases ()
{
	bool bResult = false;

	if (m_pSetup) {
		bResult = m_pSetup->saveAliases(m_sPreset);
		if (bResult) {
            ui.AudioConnectView->setDirty(false);
            ui.MidiConnectView->setDirty(false);
            ui.AlsaConnectView->setDirty(false);
		}
	}

	return bResult;
}

void ConnectionsWidget::setTabPage ( int iTabPage )
{
    ui.ConnectionsTabWidget->setCurrentIndex(iTabPage);
}

int ConnectionsWidget::tabPage () const
{
    return ui.ConnectionsTabWidget->currentIndex();
}

QFont ConnectionsWidget::connectionsFont () const
{
	// Elect one list view to retrieve current font.
    return ui.AudioConnectView->OListView()->font();
}

void ConnectionsWidget::setConnectionsFont ( const QFont & font )
{
	// Set fonts of all listviews...
    ui.AudioConnectView->OListView()->setFont(font);
    ui.AudioConnectView->IListView()->setFont(font);
    ui.MidiConnectView->OListView()->setFont(font);
    ui.MidiConnectView->IListView()->setFont(font);
    ui.AlsaConnectView->OListView()->setFont(font);
    ui.AlsaConnectView->IListView()->setFont(font);
}

void ConnectionsWidget::setConnectionsIconSize ( int iIconSize )
{
	// Set icon sizes of all views...
    ui.AudioConnectView->setIconSize(iIconSize);
    ui.MidiConnectView->setIconSize(iIconSize);
    ui.AlsaConnectView->setIconSize(iIconSize);
}

bool ConnectionsWidget::isAudioConnected () const
{
	bool bIsAudioConnected = false;

	if (m_pAudioConnect)
		bIsAudioConnected = m_pAudioConnect->canDisconnectAll();

	return bIsAudioConnected;
}

void ConnectionsWidget::audioConnectSelected ()
{
	if (m_pAudioConnect) {
		if (m_pAudioConnect->connectSelected())
			refreshAudio(false);
	}
}

void ConnectionsWidget::audioDisconnectSelected ()
{
	if (m_pAudioConnect) {
		if (m_pAudioConnect->disconnectSelected())
			refreshAudio(false);
	}
}

void ConnectionsWidget::audioDisconnectAll ()
{
	if (m_pAudioConnect) {
		if (m_pAudioConnect->disconnectAll())
			refreshAudio(false);
	}
}

void ConnectionsWidget::audioExpandAll ()
{
	if (m_pAudioConnect) {
		m_pAudioConnect->expandAll();
		stabilizeAudio(true);
	}
}

void ConnectionsWidget::audioDisconnecting (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->queryDisconnect(pOPort, pIPort, QJACKCTL_SOCKETTYPE_JACK_AUDIO);
}

void ConnectionsWidget::audioRefreshClear ()
{
	refreshAudio(true, true);
}

void ConnectionsWidget::audioRefresh ()
{
	refreshAudio(false);
}

void ConnectionsWidget::audioStabilize ()
{
	stabilizeAudio(true);
}

void ConnectionsWidget::midiConnectSelected ()
{
	if (m_pMidiConnect) {
		if (m_pMidiConnect->connectSelected())
			refreshMidi(false);
	}
}

bool ConnectionsWidget::isMidiConnected () const
{
	bool bIsMidiConnected = false;

	if (m_pMidiConnect)
		bIsMidiConnected = m_pMidiConnect->canDisconnectAll();

	return bIsMidiConnected;
}

void ConnectionsWidget::midiDisconnectSelected ()
{
	if (m_pMidiConnect) {
		if (m_pMidiConnect->disconnectSelected())
			refreshMidi(false);
	}
}

void ConnectionsWidget::midiDisconnectAll ()
{
	if (m_pMidiConnect) {
		if (m_pMidiConnect->disconnectAll())
			refreshMidi(false);
	}
}

void ConnectionsWidget::midiExpandAll ()
{
	if (m_pMidiConnect) {
		m_pMidiConnect->expandAll();
		stabilizeMidi(true);
	}
}

void ConnectionsWidget::midiDisconnecting (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->queryDisconnect(pOPort, pIPort, QJACKCTL_SOCKETTYPE_JACK_MIDI);
}

void ConnectionsWidget::midiRefreshClear ()
{
	refreshMidi(true, true);
}

void ConnectionsWidget::midiRefresh ()
{
	refreshMidi(false);
}

void ConnectionsWidget::midiStabilize ()
{
	stabilizeMidi(true);
}

bool ConnectionsWidget::isAlsaConnected () const
{
	bool bIsConnected = false;

	if (m_pAlsaConnect)
		bIsConnected = m_pAlsaConnect->canDisconnectAll();

	return bIsConnected;
}

void ConnectionsWidget::alsaConnectSelected ()
{
	if (m_pAlsaConnect) {
		if (m_pAlsaConnect->connectSelected())
			refreshAlsa(false);
	}
}

void ConnectionsWidget::alsaDisconnectSelected ()
{
	if (m_pAlsaConnect) {
		if (m_pAlsaConnect->disconnectSelected())
			refreshAlsa(false);
	}
}

void ConnectionsWidget::alsaDisconnectAll ()
{
	if (m_pAlsaConnect) {
		if (m_pAlsaConnect->disconnectAll())
			refreshAlsa(false);
	}
}

void ConnectionsWidget::alsaExpandAll ()
{
	if (m_pAlsaConnect) {
		m_pAlsaConnect->expandAll();
		stabilizeAlsa(true);
	}
}

void ConnectionsWidget::alsaDisconnecting (
    JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort )
{
    MainWidget *pMainForm = MainWidget::getInstance();
	if (pMainForm)
		pMainForm->queryDisconnect(pOPort, pIPort, QJACKCTL_SOCKETTYPE_ALSA_MIDI);
}

void ConnectionsWidget::alsaRefreshClear ()
{
	refreshAlsa(true, true);
}

void ConnectionsWidget::alsaRefresh ()
{
	refreshAlsa(false);
}

void ConnectionsWidget::alsaStabilize ()
{
	stabilizeAlsa(true);
}

void ConnectionsWidget::refreshAudio ( bool bEnabled, bool bClear )
{
	if (m_pAudioConnect == NULL)
		return;

	if (bEnabled) {
	//	m_pAudioConnect->refresh();
		stabilizeAudio(bEnabled, bClear);
	} else {
        MainWidget *pMainForm = MainWidget::getInstance();
		if (pMainForm)
			pMainForm->refreshJackConnections();
	}
}

void ConnectionsWidget::refreshMidi ( bool bEnabled, bool bClear )
{
	if (m_pMidiConnect == NULL)
		return;

	if (bEnabled) {
	//	m_pMidiConnect->refresh();
		stabilizeMidi(bEnabled, bClear);
	} else {
        MainWidget *pMainForm = MainWidget::getInstance();
		if (pMainForm)
			pMainForm->refreshJackConnections();
	}
}

void ConnectionsWidget::refreshAlsa ( bool bEnabled, bool bClear )
{
	if (m_pAlsaConnect == NULL)
		return;

	if (bEnabled) {
	//	m_pAlsaConnect->refresh();
		stabilizeAlsa(bEnabled, bClear);
	} else {
        MainWidget *pMainForm = MainWidget::getInstance();
		if (pMainForm)
			pMainForm->refreshAlsaConnections();
	}
}

void ConnectionsWidget::stabilizeAudio ( bool bEnabled, bool bClear )
{
	if (m_pAudioConnect)
		m_pAudioConnect->updateContents(!bEnabled || bClear);

	if (m_pAudioConnect && bEnabled) {
        ui.AudioConnectPushButton->setEnabled(
			m_pAudioConnect->canConnectSelected());
        ui.AudioDisconnectPushButton->setEnabled(
			m_pAudioConnect->canDisconnectSelected());
        ui.AudioDisconnectAllPushButton->setEnabled(
			m_pAudioConnect->canDisconnectAll());
        ui.AudioExpandAllPushButton->setEnabled(true);
        ui.AudioRefreshPushButton->setEnabled(true);
	} else {
        ui.AudioConnectPushButton->setEnabled(false);
        ui.AudioDisconnectPushButton->setEnabled(false);
        ui.AudioDisconnectAllPushButton->setEnabled(false);
        ui.AudioExpandAllPushButton->setEnabled(false);
        ui.AudioRefreshPushButton->setEnabled(false);
	}
}

void ConnectionsWidget::stabilizeMidi ( bool bEnabled, bool bClear )
{
	if (m_pMidiConnect)
		m_pMidiConnect->updateContents(!bEnabled || bClear);

	if (m_pMidiConnect && bEnabled) {
        ui.MidiConnectPushButton->setEnabled(
			m_pMidiConnect->canConnectSelected());
        ui.MidiDisconnectPushButton->setEnabled(
			m_pMidiConnect->canDisconnectSelected());
        ui.MidiDisconnectAllPushButton->setEnabled(
			m_pMidiConnect->canDisconnectAll());
        ui.MidiExpandAllPushButton->setEnabled(true);
        ui.MidiRefreshPushButton->setEnabled(true);
	} else {
        ui.MidiConnectPushButton->setEnabled(false);
        ui.MidiDisconnectPushButton->setEnabled(false);
        ui.MidiDisconnectAllPushButton->setEnabled(false);
        ui.MidiExpandAllPushButton->setEnabled(false);
        ui.MidiRefreshPushButton->setEnabled(false);
	}
}

void ConnectionsWidget::stabilizeAlsa ( bool bEnabled, bool bClear )
{
	if (m_pAlsaConnect)
		m_pAlsaConnect->updateContents(!bEnabled || bClear);

	if (m_pAlsaConnect && bEnabled) {
        ui.AlsaConnectPushButton->setEnabled(
			m_pAlsaConnect->canConnectSelected());
        ui.AlsaDisconnectPushButton->setEnabled(
			m_pAlsaConnect->canDisconnectSelected());
        ui.AlsaDisconnectAllPushButton->setEnabled(
			m_pAlsaConnect->canDisconnectAll());
        ui.AlsaExpandAllPushButton->setEnabled(true);
        ui.AlsaRefreshPushButton->setEnabled(true);
	} else {
        ui.AlsaConnectPushButton->setEnabled(false);
        ui.AlsaDisconnectPushButton->setEnabled(false);
        ui.AlsaDisconnectAllPushButton->setEnabled(false);
        ui.AlsaExpandAllPushButton->setEnabled(false);
        ui.AlsaRefreshPushButton->setEnabled(false);
	}
}

void ConnectionsWidget::updateAliases ()
{
	// Set alias maps for all listviews...
	if (m_pSetup && m_pSetup->bAliasesEnabled) {
		bool bRenameEnabled = m_pSetup->bAliasesEditing;
        ui.AudioConnectView->OListView()->setAliases(
			&(m_pSetup->aliasAudioOutputs), bRenameEnabled);
        ui.AudioConnectView->IListView()->setAliases(
			&(m_pSetup->aliasAudioInputs),  bRenameEnabled);
        ui.MidiConnectView->OListView()->setAliases(
			&(m_pSetup->aliasMidiOutputs), bRenameEnabled);
        ui.MidiConnectView->IListView()->setAliases(
			&(m_pSetup->aliasMidiInputs),  bRenameEnabled);
        ui.AlsaConnectView->OListView()->setAliases(
			&(m_pSetup->aliasAlsaOutputs), bRenameEnabled);
        ui.AlsaConnectView->IListView()->setAliases(
			&(m_pSetup->aliasAlsaInputs),  bRenameEnabled);
	} else {
        ui.AudioConnectView->OListView()->setAliases(NULL, false);
        ui.AudioConnectView->IListView()->setAliases(NULL, false);
        ui.MidiConnectView->OListView()->setAliases(NULL, false);
        ui.MidiConnectView->IListView()->setAliases(NULL, false);
        ui.AlsaConnectView->OListView()->setAliases(NULL, false);
        ui.AlsaConnectView->IListView()->setAliases(NULL, false);
	}
}

void ConnectionsWidget::keyPressEvent ( QKeyEvent *pKeyEvent )
{
#ifdef CONFIG_DEBUG_0
	qDebug("qjackctlConnectionsForm::keyPressEvent(%d)", pKeyEvent->key());
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
