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
#include "mainwidget.h"
#include "patchbay.h"

// Qt includes
#include <QMessageBox>
#include <QShowEvent>
#include <QHideEvent>

ConnectionsWidget::ConnectionsWidget(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);

    _settings = NULL;

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
    connect(ui.AudioConnectView->outputTreeWidget(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(audioStabilize()));
    connect(ui.AudioConnectView->inputTreeWidget(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(audioStabilize()));
    connect(ui.MidiConnectView->outputTreeWidget(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(midiStabilize()));
    connect(ui.MidiConnectView->inputTreeWidget(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(midiStabilize()));
    connect(ui.AlsaConnectView->outputTreeWidget(),
		SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		SLOT(alsaStabilize()));
    connect(ui.AlsaConnectView->inputTreeWidget(),
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


#ifndef CONFIG_JACK_MIDI
    ui.ConnectionsTabWidget->setTabEnabled(1, false);
#endif
#ifndef CONFIG_ALSA_SEQ
//	ui.ConnectionsTabWidget->setTabEnabled(2, false);
    ui.ConnectionsTabWidget->removeTab(2);
#endif

	// Start disabled.
	stabilizeAudio(false);
	stabilizeMidi(false);
	stabilizeAlsa(false);
}

ConnectionsWidget::~ConnectionsWidget() {
}

void ConnectionsWidget::showEvent(QShowEvent *pShowEvent) {
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

void ConnectionsWidget::setSettings ( Settings *pSetup )
{
    _settings = pSetup;

	// Load some splitter sizes...
    if (_settings) {
		QList<int> sizes;
		sizes.append(180);
		sizes.append(60);
		sizes.append(180);
        _settings->loadSplitterSizes(ui.AudioConnectView, sizes);
        _settings->loadSplitterSizes(ui.MidiConnectView, sizes);
        _settings->loadSplitterSizes(ui.AlsaConnectView, sizes);
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

ConnectionsSplitter *ConnectionsWidget::audioConnectView () const
{
    return ui.AudioConnectView;
}

ConnectionsSplitter *ConnectionsWidget::midiConnectView () const
{
    return ui.MidiConnectView;
}

ConnectionsSplitter *ConnectionsWidget::alsaConnectView () const
{
    return ui.AlsaConnectView;
}

bool ConnectionsWidget::queryClose ()
{
	bool bQueryClose = true;

                // TODO: Use Qt signals here.
//	if (m_pSetup
//        && (ui.AudioConnectView->isDirty() ||
//            ui.MidiConnectView->isDirty()  ||
//            ui.AlsaConnectView->isDirty())) {
//		switch (QMessageBox::warning(this,
//			tr("Warning") + " - " QJACKCTL_SUBTITLE1,
//			tr("The preset aliases have been changed:\n\n"
//			"\"%1\"\n\nDo you want to save the changes?")
//			.arg(m_sPreset),
//			QMessageBox::Save |
//			QMessageBox::Discard |
//			QMessageBox::Cancel)) {
//		case QMessageBox::Save:
//			saveAliases();
//			// Fall thru....
//		case QMessageBox::Discard:
//			break;
//		default:    // Cancel.
//			bQueryClose = false;
//		}
//	}

	// Save some splitter sizes...
    if (_settings && bQueryClose) {
        _settings->saveSplitterSizes(ui.AudioConnectView);
        _settings->saveSplitterSizes(ui.MidiConnectView);
        _settings->saveSplitterSizes(ui.AlsaConnectView);
	}

	return bQueryClose;
}

bool ConnectionsWidget::loadAliases ()
{
	bool bResult = false;
	
    if (_settings && queryClose()) {
        _preset = _settings->sDefPreset;
        bResult = _settings->loadAliases(_preset);
		if (bResult) {
            // TODO: Use Qt signals here.
//            ui.AudioConnectView->setDirty(false);
//            ui.MidiConnectView->setDirty(false);
//            ui.AlsaConnectView->setDirty(false);
		}
	}

	return bResult;
}

bool ConnectionsWidget::saveAliases() {
	bool bResult = false;

    if (_settings) {
        bResult = _settings->saveAliases(_preset);
		if (bResult) {
                        // TODO: Use Qt signals here.
//            ui.AudioConnectView->setDirty(false);
//            ui.MidiConnectView->setDirty(false);
//            ui.AlsaConnectView->setDirty(false);
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
    return ui.AudioConnectView->outputTreeWidget()->font();
}

void ConnectionsWidget::setConnectionsFont ( const QFont & font )
{
	// Set fonts of all listviews...
    ui.AudioConnectView->outputTreeWidget()->setFont(font);
    ui.AudioConnectView->inputTreeWidget()->setFont(font);
    ui.MidiConnectView->outputTreeWidget()->setFont(font);
    ui.MidiConnectView->inputTreeWidget()->setFont(font);
    ui.AlsaConnectView->outputTreeWidget()->setFont(font);
    ui.AlsaConnectView->inputTreeWidget()->setFont(font);
}

void ConnectionsWidget::setConnectionsIconSize(ConnectionsSplitter::IconSize iconSize) {
    ui.AudioConnectView->setIconSize(iconSize);
    ui.MidiConnectView->setIconSize(iconSize);
    ui.AlsaConnectView->setIconSize(iconSize);
}

bool ConnectionsWidget::isAudioConnected() const {
	bool bIsAudioConnected = false;

//    if(m_pAudioConnect) {
//		bIsAudioConnected = m_pAudioConnect->canDisconnectAll();
//    }
	return bIsAudioConnected;
}

void ConnectionsWidget::audioConnectSelected ()
{
//	if (m_pAudioConnect) {
//		if (m_pAudioConnect->connectSelected())
//			refreshAudio(false);
//	}
}

void ConnectionsWidget::audioDisconnectSelected ()
{
//	if (m_pAudioConnect) {
//		if (m_pAudioConnect->disconnectSelected())
//			refreshAudio(false);
//	}
}

void ConnectionsWidget::audioDisconnectAll () {
//	if (m_pAudioConnect) {
//		if (m_pAudioConnect->disconnectAll())
//			refreshAudio(false);
//	}
}

void ConnectionsWidget::audioExpandAll ()
{
//	if (m_pAudioConnect) {
//		m_pAudioConnect->expandAll();
//		stabilizeAudio(true);
//	}
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
//	if (m_pMidiConnect) {
//		if (m_pMidiConnect->connectSelected())
//			refreshMidi(false);
//	}
}

bool ConnectionsWidget::isMidiConnected () const
{
	bool bIsMidiConnected = false;

//	if (m_pMidiConnect)
//		bIsMidiConnected = m_pMidiConnect->canDisconnectAll();

	return bIsMidiConnected;
}

void ConnectionsWidget::midiDisconnectSelected ()
{
//	if (m_pMidiConnect) {
//		if (m_pMidiConnect->disconnectSelected())
//			refreshMidi(false);
//	}
}

void ConnectionsWidget::midiDisconnectAll ()
{
//	if (m_pMidiConnect) {
//		if (m_pMidiConnect->disconnectAll())
//			refreshMidi(false);
//	}
}

void ConnectionsWidget::midiExpandAll ()
{
//	if (m_pMidiConnect) {
//		m_pMidiConnect->expandAll();
//		stabilizeMidi(true);
//	}
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

//	if (m_pAlsaConnect)
//		bIsConnected = m_pAlsaConnect->canDisconnectAll();

	return bIsConnected;
}

void ConnectionsWidget::alsaConnectSelected ()
{
//	if (m_pAlsaConnect) {
//		if (m_pAlsaConnect->connectSelected())
//			refreshAlsa(false);
//	}
}

void ConnectionsWidget::alsaDisconnectSelected ()
{
//	if (m_pAlsaConnect) {
//		if (m_pAlsaConnect->disconnectSelected())
//			refreshAlsa(false);
//	}
}

void ConnectionsWidget::alsaDisconnectAll ()
{
//	if (m_pAlsaConnect) {
//		if (m_pAlsaConnect->disconnectAll())
//			refreshAlsa(false);
//	}
}

void ConnectionsWidget::alsaExpandAll ()
{
//	if (m_pAlsaConnect) {
//		m_pAlsaConnect->expandAll();
//		stabilizeAlsa(true);
//	}
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
//	if (m_pAudioConnect == NULL)
//		return;

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
//	if (m_pMidiConnect == NULL)
//		return;

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
//	if (m_pAlsaConnect == NULL)
//		return;

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
//	if (m_pAudioConnect)
//		m_pAudioConnect->updateContents(!bEnabled || bClear);

//	if (m_pAudioConnect && bEnabled) {
//        ui.AudioConnectPushButton->setEnabled(
//			m_pAudioConnect->canConnectSelected());
//        ui.AudioDisconnectPushButton->setEnabled(
//			m_pAudioConnect->canDisconnectSelected());
//        ui.AudioDisconnectAllPushButton->setEnabled(
//			m_pAudioConnect->canDisconnectAll());
//        ui.AudioExpandAllPushButton->setEnabled(true);
//        ui.AudioRefreshPushButton->setEnabled(true);
//	} else {
//        ui.AudioConnectPushButton->setEnabled(false);
//        ui.AudioDisconnectPushButton->setEnabled(false);
//        ui.AudioDisconnectAllPushButton->setEnabled(false);
//        ui.AudioExpandAllPushButton->setEnabled(false);
//        ui.AudioRefreshPushButton->setEnabled(false);
//	}
}

void ConnectionsWidget::stabilizeMidi ( bool bEnabled, bool bClear )
{
//	if (m_pMidiConnect)
//		m_pMidiConnect->updateContents(!bEnabled || bClear);

//	if (m_pMidiConnect && bEnabled) {
//        ui.MidiConnectPushButton->setEnabled(
//			m_pMidiConnect->canConnectSelected());
//        ui.MidiDisconnectPushButton->setEnabled(
//			m_pMidiConnect->canDisconnectSelected());
//        ui.MidiDisconnectAllPushButton->setEnabled(
//			m_pMidiConnect->canDisconnectAll());
//        ui.MidiExpandAllPushButton->setEnabled(true);
//        ui.MidiRefreshPushButton->setEnabled(true);
//	} else {
//        ui.MidiConnectPushButton->setEnabled(false);
//        ui.MidiDisconnectPushButton->setEnabled(false);
//        ui.MidiDisconnectAllPushButton->setEnabled(false);
//        ui.MidiExpandAllPushButton->setEnabled(false);
//        ui.MidiRefreshPushButton->setEnabled(false);
//	}
}

void ConnectionsWidget::stabilizeAlsa ( bool bEnabled, bool bClear )
{
//	if (m_pAlsaConnect)
//		m_pAlsaConnect->updateContents(!bEnabled || bClear);

//	if (m_pAlsaConnect && bEnabled) {
//        ui.AlsaConnectPushButton->setEnabled(
//			m_pAlsaConnect->canConnectSelected());
//        ui.AlsaDisconnectPushButton->setEnabled(
//			m_pAlsaConnect->canDisconnectSelected());
//        ui.AlsaDisconnectAllPushButton->setEnabled(
//			m_pAlsaConnect->canDisconnectAll());
//        ui.AlsaExpandAllPushButton->setEnabled(true);
//        ui.AlsaRefreshPushButton->setEnabled(true);
//	} else {
//        ui.AlsaConnectPushButton->setEnabled(false);
//        ui.AlsaDisconnectPushButton->setEnabled(false);
//        ui.AlsaDisconnectAllPushButton->setEnabled(false);
//        ui.AlsaExpandAllPushButton->setEnabled(false);
//        ui.AlsaRefreshPushButton->setEnabled(false);
//	}
}

void ConnectionsWidget::updateAliases ()
{
	// Set alias maps for all listviews...
    if (_settings && _settings->bAliasesEnabled) {
        bool bRenameEnabled = _settings->bAliasesEditing;
        ui.AudioConnectView->outputTreeWidget()->setAliases(
            &(_settings->aliasAudioOutputs), bRenameEnabled);
        ui.AudioConnectView->inputTreeWidget()->setAliases(
            &(_settings->aliasAudioInputs),  bRenameEnabled);
        ui.MidiConnectView->outputTreeWidget()->setAliases(
            &(_settings->aliasMidiOutputs), bRenameEnabled);
        ui.MidiConnectView->inputTreeWidget()->setAliases(
            &(_settings->aliasMidiInputs),  bRenameEnabled);
        ui.AlsaConnectView->outputTreeWidget()->setAliases(
            &(_settings->aliasAlsaOutputs), bRenameEnabled);
        ui.AlsaConnectView->inputTreeWidget()->setAliases(
            &(_settings->aliasAlsaInputs),  bRenameEnabled);
	} else {
        ui.AudioConnectView->outputTreeWidget()->setAliases(NULL, false);
        ui.AudioConnectView->inputTreeWidget()->setAliases(NULL, false);
        ui.MidiConnectView->outputTreeWidget()->setAliases(NULL, false);
        ui.MidiConnectView->inputTreeWidget()->setAliases(NULL, false);
        ui.AlsaConnectView->outputTreeWidget()->setAliases(NULL, false);
        ui.AlsaConnectView->inputTreeWidget()->setAliases(NULL, false);
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
