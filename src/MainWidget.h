/****************************************************************************
   Copyright (C) 2003-2014, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include "ui_MainWidget.h"

// Own includes
#include "Setup.h"

// Qt includes
#include <QProcess>
#include <QDateTime>

// JACK includes
#include <jack/jack.h>

#ifdef CONFIG_ALSA_SEQ
#include <alsa/asoundlib.h>
#else
typedef void snd_seq_t;
#endif


// Forward declarations.
class Setup;
class MessagesStatusWidget;
class SessionWidget;
class ConnectionsWidget;
class PatchbayWidget;
class PatchbayRack;
class SystemTrayIcon;
class JackPortTreeWidgetItem;

class QSocketNotifier;

#ifdef CONFIG_DBUS
class QDBusInterface;
class qjackctlDBusLogWatcher;
#endif	

class MainWidget : public QWidget
{
	Q_OBJECT
public:
    MainWidget(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
    ~MainWidget();

    static MainWidget *getInstance();

    bool setup(Setup * pSetup);

	jack_client_t *jackClient() const;
	snd_seq_t *alsaSeq() const;

	void appendMessages(const QString& s);

	bool isActivePatchbay(const QString& sPatchbayPath) const;
	void updateActivePatchbay();
	void setActivePatchbay(const QString& sPatchbayPath);
	void setRecentPatchbays(const QStringList& patchbays);

	void stabilizeForm();

	void stabilize(int msecs);

	void refreshXrunStats();

	void refreshJackConnections();
	void refreshAlsaConnections();

	void refreshPatchbay();

	void queryDisconnect(
        JackPortTreeWidgetItem *pOPort, JackPortTreeWidgetItem *pIPort, int iSocketType);

	void setQuitForce(bool bQuitForce);
	bool isQuitForce() const;

public slots:

	void startJack();
	void stopJack();

	void resetXrunStats();

protected slots:

	void readStdout();

	void jackStarted();
	void jackError(QProcess::ProcessError);
	void jackFinished();
	void jackCleanup();
	void jackStabilize();

	void stdoutNotifySlot(int);
	void alsaNotifySlot(int);

	void timerSlot();

	void jackConnectChanged();
	void alsaConnectChanged();

	void cableConnectSlot(const QString&, const QString&, unsigned int);

	void toggleMainForm();
	void toggleMessagesStatusForm();
	void toggleMessagesForm();
	void toggleStatusForm();
	void toggleSessionForm();
	void toggleConnectionsForm();
	void togglePatchbayForm();

	void showSetupForm();
	void showAboutForm();

	void transportRewind();
	void transportBackward();
	void transportPlay(bool);
	void transportStart();
	void transportStop();
	void transportForward();

	void systemTrayContextMenu(const QPoint&);
	void activatePresetsMenu(QAction *);
	void activatePreset(const QString&);
	void activatePreset(int);

	void quitMainForm();

protected:

	bool queryClose();

	void closeEvent(QCloseEvent * pCloseEvent);
	void customEvent(QEvent *pEvent);

	void appendStdoutBuffer(const QString&);
	void flushStdoutBuffer();

	QString formatExitStatus(int iExitStatus) const;

	void shellExecute(const QString& sShellCommand,
		const QString& sStartMessage, const QString& sStopMessage);

	void stopJackServer();

	QString& detectXrun(QString& s);
	void updateXrunStats(float fXrunLast);

	void appendMessagesColor(const QString& s, const QString& c);
	void appendMessagesText(const QString& s);
	void appendMessagesError(const QString& s);

	void updateMessagesFont();
	void updateMessagesLimit();
	void updateConnectionsFont();
	void updateConnectionsIconSize();
	void updateJackClientPortAlias();
	void updateBezierLines();
	void updateDisplayEffect();
	void updateTimeDisplayFonts();
	void updateTimeDisplayToolTips();
	void updateTimeFormat();
	void updateAliases();
	void updateButtons();

	void updateXrunCount();

	QString formatTime(float secs) const;
	QString formatElapsedTime(int iStatusItem, const QTime& t, bool bElapsed) const;
	void updateElapsedTimes();

	void portNotifyEvent();
	void xrunNotifyEvent();
	void buffNotifyEvent();
	void shutNotifyEvent();
	void exitNotifyEvent();

	void startJackClientDelay();
	bool startJackClient(bool bDetach);
	void stopJackClient();

	void refreshConnections();
	void refreshStatus();

	void updateStatusItem(int iStatusItem, const QString& sText);
	void updateTitleStatus();
	void updateServerState(int iState);
	void updateSystemTray();

	void showDirtySettingsWarning();
	void showDirtySetupWarning();

	void contextMenuEvent(QContextMenuEvent *);
	void mousePressEvent(QMouseEvent *pMouseEvent);

#ifdef CONFIG_DBUS

	// D-BUS: Set/reset parameter values
	// from current selected preset options.
	void setDBusParameters();

	// D-BUS: Set parameter values (with reset option).
	bool setDBusEngineParameter(
		const QString& param, const QVariant& value, bool bSet = true);
	bool setDBusDriverParameter(
		const QString& param, const QVariant& value, bool bSet = true);
	bool setDBusParameter(
		const QStringList& path, const QVariant& value, bool bSet = true);

	// D-BUS: Reset parameter (to default) values.
	bool resetDBusEngineParameter(const QString& param);
	bool resetDBusDriverParameter(const QString& param);
	bool resetDBusParameter(const QStringList& path);

	// D-BUS: Set parameter values.
	QVariant getDBusEngineParameter(const QString& param);
	QVariant getDBusDriverParameter(const QString& param);
	QVariant getDBusParameter(const QStringList& path);

#endif

	// Quotes string with embedded whitespace.
	QString formatQuoted(const QString& s) const;

	// Guarded transport play/pause toggle.
	void transportPlayStatus(bool bOn);

private:
    Ui::MainWidget m_ui;

    Setup *m_pSetup;

	QProcess *m_pJack;

	int m_iServerState;

	jack_client_t *m_pJackClient;
	bool m_bJackDetach;
	bool m_bJackShutdown;

	snd_seq_t *m_pAlsaSeq;

#ifdef CONFIG_DBUS
	QDBusInterface *m_pDBusControl;
	QDBusInterface *m_pDBusConfig;
	qjackctlDBusLogWatcher *m_pDBusLogWatcher;
	bool m_bDBusStarted;
#endif	

	int m_iStartDelay;
	int m_iTimerDelay;
	int m_iTimerRefresh;
	int m_iJackRefresh;
	int m_iAlsaRefresh;
	int m_iJackDirty;
	int m_iAlsaDirty;
	int m_iStatusBlink;
	int m_iStatusRefresh;
	int m_iPatchbayRefresh;

	QSocketNotifier *m_pStdoutNotifier;
	QSocketNotifier *m_pAlsaNotifier;

	int   m_iXrunCallbacks;
	int   m_iXrunSkips;
	int   m_iXrunStats;
	int   m_iXrunCount;
	float m_fXrunTotal;
	float m_fXrunMax;
	float m_fXrunMin;
	float m_fXrunLast;
	QTime m_tXrunLast;
	QTime m_tResetLast;

    MessagesStatusWidget *m_pMessagesStatusForm;
    SessionWidget     *m_pSessionForm;
    ConnectionsWidget *m_pConnectionsForm;
    PatchbayWidget    *m_pPatchbayForm;
    PatchbayRack *m_pPatchbayRack;
    SystemTrayIcon *m_pSystemTray;
    Preset m_preset;

	QString m_sStdoutBuffer;
	QString m_sTimeDashes;
	QString m_sJackCmdLine;

	bool  m_bQuitForce;
	float m_fSkipAccel;

	int m_iTransportPlay;

	// Kind-of singleton reference.
    static MainWidget *g_pMainForm;
};
