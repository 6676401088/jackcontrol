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

// Qt includes
#include <QWidget>
class QComboBox;
class QSplitter;

// Own includes
#include "connectalias.h"
#include "connectionssplitter.h"

// Icon size combobox item indexes.
#define QJACKCTL_ICON_16X16 0
#define QJACKCTL_ICON_32X32 1
#define QJACKCTL_ICON_64X64 2

// Server settings preset struct.
struct Preset {
    QString serverPrefix;
    QString serverName;
    bool    realtime;
    bool    softMode;
    bool    monitor;
    bool    shorts;
    bool    noMemoryLock;
    bool    unlockMemory;
    bool    HWMonitor;
    bool    HWMeter;
    bool    ignoreHW;
    int     priority;
    int     frames;
    int     sampleRate;
    int     periods;
    int     wordLength;
    int     wait;
    int     channels;
    QString driver;
    QString interface;
    int     audio;
    int     dither;
    int     timeout;
    QString inputDevice;
    QString outputDevice;
    int     inputChannels;
    int     outputChannels;
    int     inputLatency;
    int     outputLatency;
    int     startDelay;
    bool    verbose;
    int     maximumNumberOfPorts;
    QString midiDriver;
    QString serverSuffix;
};

// Common settings profile class.
class Settings {
public:
    enum AudioMode {
        AudioModeDuplex = 0,
        AudioModeCaptureOnly = 1,
        AudioModePlaybackOnly = 2
    };

    Settings();
    ~Settings();

	// The settings object accessor.
	QSettings& settings();

	// Explicit I/O methods.
	void loadSetup();
	void saveSetup();

	// Command line arguments parser.
	bool parse_args(const QStringList& args);
	// Command line usage helper.
	void print_usage(const QString& arg0);

	// Default (translated) preset name.
	QString sDefPresetName;

	// Immediate server start options.
	bool bStartJack;
	bool bStartJackCmd;

	// Server stop options.
	bool bStopJack;

	// User supplied command line.
	QString sCmdLine;

	// Current (default) preset name.
	QString sDefPreset;

	// Available presets list.
	QStringList presets;

	// Options...
	bool    bSingleton;
	QString sServerName;
	bool    bStartupScript;
	QString sStartupScriptShell;
	bool    bPostStartupScript;
	QString sPostStartupScriptShell;
	bool    bShutdownScript;
	QString sShutdownScriptShell;
	bool    bPostShutdownScript;
	QString sPostShutdownScriptShell;
	bool    bStdoutCapture;
	QString sXrunRegex;
	bool    bActivePatchbay;
	QString sActivePatchbayPath;
#ifdef CONFIG_AUTO_REFRESH
	bool    bAutoRefresh;
	int     iTimeRefresh;
#endif
	bool    bMessagesLog;
	QString sMessagesLogPath;
	bool    bBezierLines;
	int     iTimeDisplay;
	int     iTimeFormat;
	QString sMessagesFont;
	bool    bMessagesLimit;
	int     iMessagesLimitLines;
	QString sDisplayFont1;
	QString sDisplayFont2;
	bool    bDisplayEffect;
	bool    bDisplayBlink;
	int     iJackClientPortAlias;
    ConnectionsSplitter::IconSize iConnectionsIconSize;
	QString sConnectionsFont;
	bool    bQueryClose;
	bool    bKeepOnTop;
	bool    bSystemTray;
	bool    bStartMinimized;
	bool    bDelayedSetup;
	bool    bServerConfig;
	QString sServerConfigName;
	bool    bServerConfigTemp;
	bool    bQueryShutdown;
	bool    bAlsaSeqEnabled;
	bool    bDBusEnabled;
	bool    bAliasesEnabled;
	bool    bAliasesEditing;
	bool    bLeftButtons;
	bool    bRightButtons;
	bool    bTransportButtons;
	bool    bTextLabels;
	int     iBaseFontSize;

	// Defaults...
	QString sPatchbayPath;
	// Recent patchbay listing.
	QStringList patchbays;

	// Recent session directories.
	QStringList sessionDirs;

	// Last open tab page...
	int iMessagesStatusTabPage;
	int iConnectionsTabPage;

	// Last session save type...
	bool bSessionSaveVersion;
	
	// Aliases containers.
    ConnectAlias aliasAudioOutputs;
    ConnectAlias aliasAudioInputs;
    ConnectAlias aliasMidiOutputs;
    ConnectAlias aliasMidiInputs;
    ConnectAlias aliasAlsaOutputs;
    ConnectAlias aliasAlsaInputs;

	// Aliases preset management methods.
	bool loadAliases(const QString& sPreset);
	bool saveAliases(const QString& sPreset);

	// Preset management methods.
	bool loadPreset(Preset& preset, const QString& sPreset);
	bool savePreset(Preset& preset, const QString& sPreset);
	bool deletePreset(const QString& sPreset);

	// Combo box history persistence helper prototypes.
	void loadComboBoxHistory(QComboBox *pComboBox, int iLimit = 8);
	void saveComboBoxHistory(QComboBox *pComboBox, int iLimit = 8);

	// Splitter widget sizes persistence helper methods.
	void loadSplitterSizes(QSplitter *pSplitter, QList<int>& sizes);
	void saveSplitterSizes(QSplitter *pSplitter);

	// Widget geometry persistence helper prototypes.
	void saveWidgetGeometry(QWidget *pWidget, bool bVisible = false);
	void loadWidgetGeometry(QWidget *pWidget, bool bVisible = false);

private:

	// Our proper settings profile.
    QSettings _settings;
};