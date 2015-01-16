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

// uic includes
#include "ui_connectionswidget.h"

// Own includes
#include "JackConnect.h"
#include "AlsaConnect.h"

// Forward declarations.
class Setup;

class ConnectionsWidget : public QWidget {
	Q_OBJECT
public:
    ConnectionsWidget(QWidget *parent = 0);
    ~ConnectionsWidget();

    void setup(Setup *pSetup);

    ConnectionsViewSplitter *audioConnectView() const;
    ConnectionsViewSplitter *midiConnectView() const;
    ConnectionsViewSplitter *alsaConnectView() const;

	bool queryClose();

	enum TabPage { AudioTab = 0, MidiTab = 1, AlsaTab = 2 };

	void setTabPage(int iTabPage);
	int tabPage() const;

	QFont connectionsFont() const;
	void setConnectionsFont(const QFont& font);

	void setConnectionsIconSize(int iIconSize);

	bool isAudioConnected() const;
	bool isMidiConnected() const;
	bool isAlsaConnected() const;

	void refreshAudio(bool bEnabled, bool bClear = false);
	void refreshMidi(bool bEnabled, bool bClear = false);
	void refreshAlsa(bool bEnabled, bool bClear = false);

	void stabilizeAudio(bool bEnabled, bool bClear = false);
	void stabilizeMidi(bool bEnabled, bool bClear = false);
	void stabilizeAlsa(bool bEnabled, bool bClear = false);

    void setupAliases(Setup *pSetup);
	void updateAliases();
	bool loadAliases();
	bool saveAliases();

public slots:

	void audioConnectSelected();
	void audioDisconnectSelected();
	void audioDisconnectAll();
	void audioExpandAll();
	void audioRefreshClear();
	void audioRefresh();
	void audioStabilize();

	void midiConnectSelected();
	void midiDisconnectSelected();
	void midiDisconnectAll();
	void midiExpandAll();
	void midiRefreshClear();
	void midiRefresh();
	void midiStabilize();

	void alsaConnectSelected();
	void alsaDisconnectSelected();
	void alsaDisconnectAll();
	void alsaExpandAll();
	void alsaRefreshClear();
	void alsaRefresh();
	void alsaStabilize();

protected slots:

    void audioDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);
    void midiDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);
    void alsaDisconnecting(JackPortTreeWidgetItem *, JackPortTreeWidgetItem *);

protected:

	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void closeEvent(QCloseEvent *);

	void keyPressEvent(QKeyEvent *);

private:
    Ui::ConnectionsWidget ui;

	// Instance variables.
	JackConnectionsModel *m_pAudioConnect;
	JackConnectionsModel *m_pMidiConnect;
	qjackctlAlsaConnect *m_pAlsaConnect;
    Setup               *m_pSetup;
	QString              m_sPreset;
};
