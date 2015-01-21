/****************************************************************************
   Copyright (C) 2003-2012, rncbc aka Rui Nuno Capela. All rights reserved.
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

// Own includes
#include "about.h"
#include "settings.h"

// Qt includes
#include <QComboBox>
#include <QSplitter>
#include <QList>
#include <QTimer>
#include <QTextStream>

Settings::Settings()
    : _settings(QJACKCTL_DOMAIN, QJACKCTL_TITLE)
{
	bStartJackCmd = false;
	sDefPresetName = QObject::tr("(default)");

	loadSetup();
}

Settings::~Settings()
{
	saveSetup();
}

QSettings& Settings::settings ()
{
    return _settings;
}

void Settings::loadSetup ()
{
    _settings.beginGroup("/Presets");
    sDefPreset = _settings.value("/DefPreset", sDefPresetName).toString();
	QString sPrefix = "/Preset%1";
	int i = 0;
	for (;;) {
        QString sItem = _settings.value(sPrefix.arg(++i)).toString();
		if (sItem.isEmpty())
			break;
		presets.append(sItem);
	}
    _settings.endGroup();

    _settings.beginGroup("/Options");
    bSingleton               = _settings.value("/Singleton", true).toBool();
//	sServerName              = m_settings.value("/ServerName").toString();
    bStartJack               = _settings.value("/StartJack", false).toBool();
//	bStartJackCmd            = m_settings.value("/StartJackCmd", false).toBool();
    bStopJack                = _settings.value("/StopJack", true).toBool();
    bStartupScript           = _settings.value("/StartupScript", false).toBool();
    sStartupScriptShell      = _settings.value("/StartupScriptShell").toString();
    bPostStartupScript       = _settings.value("/PostStartupScript", false).toBool();
    sPostStartupScriptShell  = _settings.value("/PostStartupScriptShell").toString();
    bShutdownScript          = _settings.value("/ShutdownScript", false).toBool();
    sShutdownScriptShell     = _settings.value("/ShutdownScriptShell").toString();
    bPostShutdownScript      = _settings.value("/PostShutdownScript", false).toBool();
    sPostShutdownScriptShell = _settings.value("/PostShutdownScriptShell").toString();
    bStdoutCapture           = _settings.value("/StdoutCapture", true).toBool();
    sXrunRegex               = _settings.value("/XrunRegex", "xrun of at least ([0-9|\\.]+) msecs").toString();
    bActivePatchbay          = _settings.value("/ActivePatchbay", false).toBool();
    sActivePatchbayPath      = _settings.value("/ActivePatchbayPath").toString();
#ifdef CONFIG_AUTO_REFRESH
	bAutoRefresh             = m_settings.value("/AutoRefresh", false).toBool();
	iTimeRefresh             = m_settings.value("/TimeRefresh", 10).toInt();
#endif
    bMessagesLog             = _settings.value("/MessagesLog", false).toBool();
    sMessagesLogPath         = _settings.value("/MessagesLogPath", "qjackctl.log").toString();
    bBezierLines             = _settings.value("/BezierLines", false).toBool();
    iTimeDisplay             = _settings.value("/TimeDisplay", 0).toInt();
    iTimeFormat              = _settings.value("/TimeFormat", 0).toInt();
    sMessagesFont            = _settings.value("/MessagesFont").toString();
    bMessagesLimit           = _settings.value("/MessagesLimit", true).toBool();
    iMessagesLimitLines      = _settings.value("/MessagesLimitLines", 1000).toInt();
    sDisplayFont1            = _settings.value("/DisplayFont1").toString();
    sDisplayFont2            = _settings.value("/DisplayFont2").toString();
    bDisplayEffect           = _settings.value("/DisplayEffect", true).toBool();
    bDisplayBlink            = _settings.value("/DisplayBlink", true).toBool();
    iJackClientPortAlias     = _settings.value("/JackClientPortAlias", 0).toInt();
    iConnectionsIconSize     = (ConnectionsDrawer::IconSize)_settings.value("/ConnectionsIconSize", QJACKCTL_ICON_16X16).toInt();
    sConnectionsFont         = _settings.value("/ConnectionsFont").toString();
    bQueryClose              = _settings.value("/QueryClose", true).toBool();
    bKeepOnTop               = _settings.value("/KeepOnTop", false).toBool();
    bSystemTray              = _settings.value("/SystemTray", false).toBool();
    bStartMinimized          = _settings.value("/StartMinimized", false).toBool();
    bDelayedSetup            = _settings.value("/DelayedSetup", false).toBool();
    bServerConfig            = _settings.value("/ServerConfig", true).toBool();
    sServerConfigName        = _settings.value("/ServerConfigName", ".jackdrc").toString();
    bServerConfigTemp        = _settings.value("/ServerConfigTemp", false).toBool();
    bQueryShutdown           = _settings.value("/QueryShutdown", true).toBool();
    bAlsaSeqEnabled          = _settings.value("/AlsaSeqEnabled", true).toBool();
    bDBusEnabled             = _settings.value("/DBusEnabled", false).toBool();
    bAliasesEnabled          = _settings.value("/AliasesEnabled", false).toBool();
    bAliasesEditing          = _settings.value("/AliasesEditing", false).toBool();
    bLeftButtons             = _settings.value("/LeftButtons", true).toBool();
    bRightButtons            = _settings.value("/RightButtons", true).toBool();
    bTransportButtons        = _settings.value("/TransportButtons", true).toBool();
    bTextLabels              = _settings.value("/TextLabels", true).toBool();
    iBaseFontSize            = _settings.value("/BaseFontSize", 0).toInt();
    _settings.endGroup();

    _settings.beginGroup("/Defaults");
    sPatchbayPath = _settings.value("/PatchbayPath").toString();
    iMessagesStatusTabPage = _settings.value("/MessagesStatusTabPage", 0).toInt();
    iConnectionsTabPage = _settings.value("/ConnectionsTabPage", 0).toInt();
    bSessionSaveVersion = _settings.value("/SessionSaveVersion", true).toBool();
    _settings.endGroup();

	// Load recent patchbay list...
    _settings.beginGroup("/Patchbays");
	sPrefix = "/Patchbay%1";
	i = 0;
	for (;;) {
        QString sItem = _settings.value(sPrefix.arg(++i)).toString();
		if (sItem.isEmpty())
			break;
		patchbays.append(sItem);
	}
    _settings.endGroup();

	// Load recent session directory list...
    _settings.beginGroup("/SessionDirs");
	sPrefix = "/SessionDir%1";
	i = 0;
	for (;;) {
        QString sItem = _settings.value(sPrefix.arg(++i)).toString();
		if (sItem.isEmpty())
			break;
		sessionDirs.append(sItem);
	}
    _settings.endGroup();
}


// Explicit save method.
void Settings::saveSetup ()
{
	// Save all settings and options...
    _settings.beginGroup("/Program");
    _settings.setValue("/Version", QJACKCTL_VERSION);
    _settings.endGroup();

    _settings.beginGroup("/Presets");
    _settings.setValue("/DefPreset", sDefPreset);
	// Save last preset list.
	QString sPrefix = "/Preset%1";
	int i = 0;
	QStringListIterator iter(presets);
	while (iter.hasNext())
        _settings.setValue(sPrefix.arg(++i), iter.next());
	// Cleanup old entries, if any...
    while (!_settings.value(sPrefix.arg(++i)).toString().isEmpty())
        _settings.remove(sPrefix.arg(i));
    _settings.endGroup();

    _settings.beginGroup("/Options");
    _settings.setValue("/Singleton",               bSingleton);
//	m_settings.setValue("/ServerName",              sServerName);
    _settings.setValue("/StartJack",               bStartJack);
//	m_settings.setValue("/StartJackCmd",            bStartJackCmd);
    _settings.setValue("/StopJack",                bStopJack);
    _settings.setValue("/StartupScript",           bStartupScript);
    _settings.setValue("/StartupScriptShell",      sStartupScriptShell);
    _settings.setValue("/PostStartupScript",       bPostStartupScript);
    _settings.setValue("/PostStartupScriptShell",  sPostStartupScriptShell);
    _settings.setValue("/ShutdownScript",          bShutdownScript);
    _settings.setValue("/ShutdownScriptShell",     sShutdownScriptShell);
    _settings.setValue("/PostShutdownScript",      bPostShutdownScript);
    _settings.setValue("/PostShutdownScriptShell", sPostShutdownScriptShell);
    _settings.setValue("/StdoutCapture",           bStdoutCapture);
    _settings.setValue("/XrunRegex",               sXrunRegex);
    _settings.setValue("/ActivePatchbay",          bActivePatchbay);
    _settings.setValue("/ActivePatchbayPath",      sActivePatchbayPath);
#ifdef CONFIG_AUTO_REFRESH
	m_settings.setValue("/AutoRefresh",             bAutoRefresh);
	m_settings.setValue("/TimeRefresh",             iTimeRefresh);
#endif
    _settings.setValue("/MessagesLog",             bMessagesLog);
    _settings.setValue("/MessagesLogPath",         sMessagesLogPath);
    _settings.setValue("/BezierLines",             bBezierLines);
    _settings.setValue("/TimeDisplay",             iTimeDisplay);
    _settings.setValue("/TimeFormat",              iTimeFormat);
    _settings.setValue("/MessagesFont",            sMessagesFont);
    _settings.setValue("/MessagesLimit",           bMessagesLimit);
    _settings.setValue("/MessagesLimitLines",      iMessagesLimitLines);
    _settings.setValue("/DisplayFont1",            sDisplayFont1);
    _settings.setValue("/DisplayFont2",            sDisplayFont2);
    _settings.setValue("/DisplayEffect",           bDisplayEffect);
    _settings.setValue("/DisplayBlink",            bDisplayBlink);
    _settings.setValue("/JackClientPortAlias",     iJackClientPortAlias);
    _settings.setValue("/ConnectionsIconSize",     iConnectionsIconSize);
    _settings.setValue("/ConnectionsFont",         sConnectionsFont);
    _settings.setValue("/QueryClose",              bQueryClose);
    _settings.setValue("/KeepOnTop",               bKeepOnTop);
    _settings.setValue("/SystemTray",              bSystemTray);
    _settings.setValue("/StartMinimized",          bStartMinimized);
    _settings.setValue("/DelayedSetup",            bDelayedSetup);
    _settings.setValue("/ServerConfig",            bServerConfig);
    _settings.setValue("/ServerConfigName",        sServerConfigName);
    _settings.setValue("/ServerConfigTemp",        bServerConfigTemp);
    _settings.setValue("/QueryShutdown",           bQueryShutdown);
    _settings.setValue("/AlsaSeqEnabled",          bAlsaSeqEnabled);
    _settings.setValue("/DBusEnabled",             bDBusEnabled);
    _settings.setValue("/AliasesEnabled",          bAliasesEnabled);
    _settings.setValue("/AliasesEditing",          bAliasesEditing);
    _settings.setValue("/LeftButtons",             bLeftButtons);
    _settings.setValue("/RightButtons",            bRightButtons);
    _settings.setValue("/TransportButtons",        bTransportButtons);
    _settings.setValue("/TextLabels",              bTextLabels);
    _settings.setValue("/BaseFontSize",            iBaseFontSize);
    _settings.endGroup();

    _settings.beginGroup("/Defaults");
    _settings.setValue("/PatchbayPath", sPatchbayPath);
    _settings.setValue("/MessagesStatusTabPage", iMessagesStatusTabPage);
    _settings.setValue("/ConnectionsTabPage", iConnectionsTabPage);
    _settings.setValue("/SessionSaveVersion", bSessionSaveVersion);
    _settings.endGroup();

	// Save patchbay list...
    _settings.beginGroup("/Patchbays");
	sPrefix = "/Patchbay%1";
	i = 0;
	QStringListIterator iter2(patchbays);
	while (iter2.hasNext())
        _settings.setValue(sPrefix.arg(++i), iter2.next());
	// Cleanup old entries, if any...
    while (!_settings.value(sPrefix.arg(++i)).toString().isEmpty())
        _settings.remove(sPrefix.arg(i));
    _settings.endGroup();

	// Save session directory list...
    _settings.beginGroup("/SessionDirs");
	sPrefix = "/SessionDir%1";
	i = 0;
	QStringListIterator iter3(sessionDirs);
	while (iter3.hasNext())
        _settings.setValue(sPrefix.arg(++i), iter3.next());
	// Cleanup old entries, if any...
    while (!_settings.value(sPrefix.arg(++i)).toString().isEmpty())
        _settings.remove(sPrefix.arg(i));
    _settings.endGroup();

	// Commit all changes to disk.
    _settings.sync();
}


//---------------------------------------------------------------------------
// Aliases preset management methods.

bool Settings::loadAliases ( const QString& sPreset )
{
	QString sSuffix;
	if (sPreset != sDefPresetName && !sPreset.isEmpty()) {
		sSuffix = '/' + sPreset;
		// Check if on list.
		if (!presets.contains(sPreset))
			return false;
	}

	// Load preset aliases...
	const QString sAliasesKey = "/Aliases" + sSuffix;
    _settings.beginGroup(sAliasesKey);
    _settings.beginGroup("/Jack");	// FIXME: Audio
    aliasAudioOutputs.loadSettings(_settings, "/Outputs");
    aliasAudioInputs.loadSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.beginGroup("/Midi");
    aliasMidiOutputs.loadSettings(_settings, "/Outputs");
    aliasMidiInputs.loadSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.beginGroup("/Alsa");
    aliasAlsaOutputs.loadSettings(_settings, "/Outputs");
    aliasAlsaInputs.loadSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.endGroup();

	return true;
}

bool Settings::saveAliases ( const QString& sPreset )
{
	QString sSuffix;
	if (sPreset != sDefPresetName && !sPreset.isEmpty()) {
		sSuffix = "/" + sPreset;
		// Append to list if not already.
		if (!presets.contains(sPreset))
			presets.prepend(sPreset);
	}

	// Save preset aliases...
	const QString sAliasesKey = "/Aliases" + sSuffix;
    _settings.remove(sAliasesKey);
    _settings.beginGroup(sAliasesKey);
    _settings.beginGroup("/Jack");	// FIXME: Audio
    aliasAudioOutputs.saveSettings(_settings, "/Outputs");
    aliasAudioInputs.saveSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.beginGroup("/Midi");
    aliasMidiOutputs.saveSettings(_settings, "/Outputs");
    aliasMidiInputs.saveSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.beginGroup("/Alsa");
    aliasAlsaOutputs.saveSettings(_settings, "/Outputs");
    aliasAlsaInputs.saveSettings(_settings, "/Inputs");
    _settings.endGroup();
    _settings.endGroup();

	return true;
}


//---------------------------------------------------------------------------
// Preset management methods.

bool Settings::loadPreset ( Preset& preset, const QString& sPreset )
{
	QString sSuffix;
	if (sPreset != sDefPresetName && !sPreset.isEmpty()) {
		sSuffix = '/' + sPreset;
		// Check if on list.
		if (!presets.contains(sPreset))
			return false;
	}

    _settings.beginGroup("/Settings" + sSuffix);
    preset.serverPrefix = _settings.value("/Server", "jackd").toString();
    preset.serverName  = _settings.value("/ServerName").toString();
    preset.realtime    = _settings.value("/Realtime", true).toBool();
    preset.softMode    = _settings.value("/SoftMode", false).toBool();
    preset.monitor     = _settings.value("/Monitor", false).toBool();
    preset.shorts      = _settings.value("/Shorts", false).toBool();
    preset.noMemoryLock   = _settings.value("/NoMemLock", false).toBool();
    preset.unlockMemory   = _settings.value("/UnlockMem", false).toBool();
    preset.HWMonitor       = _settings.value("/HWMon", false).toBool();
    preset.HWMeter     = _settings.value("/HWMeter", false).toBool();
    preset.ignoreHW    = _settings.value("/IgnoreHW", false).toBool();
    preset.priority    = _settings.value("/Priority", 0).toInt();
    preset.frames      = _settings.value("/Frames", 1024).toInt();
    preset.sampleRate  = _settings.value("/SampleRate", 48000).toInt();
    preset.periods     = _settings.value("/Periods", 2).toInt();
    preset.wordLength  = _settings.value("/WordLength", 16).toInt();
    preset.wait        = _settings.value("/Wait", 21333).toInt();
    preset.channels        = _settings.value("/Chan", 0).toInt();
    preset.driver      = _settings.value("/Driver", "alsa").toString();
    preset.interface   = _settings.value("/Interface").toString();
    preset.audio       = _settings.value("/Audio", 0).toInt();
    preset.dither      = _settings.value("/Dither", 0).toInt();
    preset.timeout     = _settings.value("/Timeout", 500).toInt();
    preset.inputDevice    = _settings.value("/InDevice").toString();
    preset.outputDevice   = _settings.value("/OutDevice").toString();
    preset.inputChannels  = _settings.value("/InChannels", 0).toInt();
    preset.outputChannels = _settings.value("/OutChannels", 0).toInt();
    preset.inputLatency   = _settings.value("/InLatency", 0).toInt();
    preset.outputLatency  = _settings.value("/OutLatency", 0).toInt();
    preset.startDelay  = _settings.value("/StartDelay", 2).toInt();
    preset.verbose     = _settings.value("/Verbose", false).toBool();
    preset.maximumNumberOfPorts     = _settings.value("/PortMax", 256).toInt();
    preset.midiDriver  = _settings.value("/MidiDriver").toString();
    preset.serverSuffix = _settings.value("/ServerSuffix").toString();
    _settings.endGroup();

#ifdef CONFIG_JACK_MIDI
    if (!preset.midiDriver.isEmpty() &&
        preset.midiDriver != "raw" &&
        preset.midiDriver != "seq")
        preset.midiDriver.clear();
#endif

	return true;
}

bool Settings::savePreset ( Preset& preset, const QString& sPreset )
{
	QString sSuffix;
	if (sPreset != sDefPresetName && !sPreset.isEmpty()) {
		sSuffix = '/' + sPreset;
		// Append to list if not already.
		if (!presets.contains(sPreset))
			presets.prepend(sPreset);
	}

    _settings.beginGroup("/Settings" + sSuffix);
    _settings.setValue("/Server",      preset.serverPrefix);
    _settings.setValue("/ServerName",  preset.serverName);
    _settings.setValue("/Realtime",    preset.realtime);
    _settings.setValue("/SoftMode",    preset.softMode);
    _settings.setValue("/Monitor",     preset.monitor);
    _settings.setValue("/Shorts",      preset.shorts);
    _settings.setValue("/NoMemLock",   preset.noMemoryLock);
    _settings.setValue("/UnlockMem",   preset.unlockMemory);
    _settings.setValue("/HWMon",       preset.HWMonitor);
    _settings.setValue("/HWMeter",     preset.HWMeter);
    _settings.setValue("/IgnoreHW",    preset.ignoreHW);
    _settings.setValue("/Priority",    preset.priority);
    _settings.setValue("/Frames",      preset.frames);
    _settings.setValue("/SampleRate",  preset.sampleRate);
    _settings.setValue("/Periods",     preset.periods);
    _settings.setValue("/WordLength",  preset.wordLength);
    _settings.setValue("/Wait",        preset.wait);
    _settings.setValue("/Chan",        preset.channels);
    _settings.setValue("/Driver",      preset.driver);
    _settings.setValue("/Interface",   preset.interface);
    _settings.setValue("/Audio",       preset.audio);
    _settings.setValue("/Dither",      preset.dither);
    _settings.setValue("/Timeout",     preset.timeout);
    _settings.setValue("/InDevice",    preset.inputDevice);
    _settings.setValue("/OutDevice",   preset.outputDevice);
    _settings.setValue("/InChannels",  preset.inputChannels);
    _settings.setValue("/OutChannels", preset.outputChannels);
    _settings.setValue("/InLatency",   preset.inputLatency);
    _settings.setValue("/OutLatency",  preset.outputLatency);
    _settings.setValue("/StartDelay",  preset.startDelay);
    _settings.setValue("/Verbose",     preset.verbose);
    _settings.setValue("/PortMax",     preset.maximumNumberOfPorts);
    _settings.setValue("/MidiDriver",  preset.midiDriver);
    _settings.setValue("/ServerSuffix", preset.serverSuffix);
    _settings.endGroup();

	return true;
}

bool Settings::deletePreset ( const QString& sPreset )
{
	QString sSuffix;
	if (sPreset != sDefPresetName && !sPreset.isEmpty()) {
		sSuffix = '/' + sPreset;
		int iPreset = presets.indexOf(sPreset);
		if (iPreset < 0)
			return false;
		presets.removeAt(iPreset);
        _settings.remove("/Settings" + sSuffix);
        _settings.remove("/Aliases" + sSuffix);
	}
	return true;
}


//-------------------------------------------------------------------------
// Command-line argument stuff.
//

// Help about command line options.
void Settings::print_usage ( const QString& arg0 )
{
	QTextStream out(stderr);
	const QString sEot = "\n\t";
	const QString sEol = "\n\n";

	out << QObject::tr("Usage: %1"
		" [options] [command-and-args]").arg(arg0) + sEol;
	out << QJACKCTL_TITLE " - " + QObject::tr(QJACKCTL_SUBTITLE) + sEol;
	out << QObject::tr("Options:") + sEol;
	out << "  -s, --start" + sEot +
		QObject::tr("Start JACK audio server immediately") + sEol;
	out << "  -p, --preset=[label]" + sEot +
		QObject::tr("Set default settings preset name") + sEol;
	out << "  -a, --active-patchbay=[path]" + sEot +
		QObject::tr("Set active patchbay definition file") + sEol;
	out << "  -n, --server-name=[label]" + sEot +
		QObject::tr("Set default JACK audio server name") + sEol;
	out << "  -h, --help" + sEot +
		QObject::tr("Show help about command line options") + sEol;
	out << "  -v, --version" + sEot +
		QObject::tr("Show version information") + sEol;
}


// Parse command line arguments into m_settings.
bool Settings::parse_args ( const QStringList& args )
{
	QTextStream out(stderr);
	const QString sEol = "\n\n";
	int iCmdArgs = 0;
	int argc = args.count();

	for (int i = 1; i < argc; i++) {

		if (iCmdArgs > 0) {
			sCmdLine += ' ';
			sCmdLine += args.at(i);
			iCmdArgs++;
			continue;
		}

		QString sArg = args.at(i);
		QString sVal = QString::null;
		int iEqual = sArg.indexOf('=');
		if (iEqual >= 0) {
			sVal = sArg.right(sArg.length() - iEqual - 1);
			sArg = sArg.left(iEqual);
		}
		else if (i < argc - 1)
			sVal = args.at(i + 1);

		if (sArg == "-s" || sArg == "--start") {
			bStartJackCmd = true;
		}
		else if (sArg == "-p" || sArg == "--preset") {
			if (sVal.isNull()) {
				out << QObject::tr("Option -p requires an argument (preset).") + sEol;
				return false;
			}
			sDefPreset = sVal;
			if (iEqual < 0)
				i++;
		}
		else if (sArg == "-a" || sArg == "--active-patchbay") {
			if (sVal.isNull()) {
				out << QObject::tr("Option -a requires an argument (path).") + sEol;
				return false;
			}
			bActivePatchbay = true;
			sActivePatchbayPath = sVal;
			if (iEqual < 0)
				i++;
		}
		else if (sArg == "-n" || sArg == "--server-name") {
			if (sVal.isNull()) {
				out << QObject::tr("Option -n requires an argument (name).") + sEol;
				return false;
			}
			sServerName = sVal;
			if (iEqual < 0)
				i++;
		}
		else if (sArg == "-h" || sArg == "--help") {
			print_usage(args.at(0));
			return false;
		}
		else if (sArg == "-v" || sArg == "--version") {
			out << QObject::tr("Qt: %1\n").arg(qVersion());
			out << QObject::tr(QJACKCTL_TITLE ": %1\n").arg(QJACKCTL_VERSION);
			return false;
		}	// FIXME: Avoid auto-start jackd stuffed args!
		else if (sArg != "-T" && sArg != "-ndefault") {
			// Here starts the optional command line...
			sCmdLine += sArg;
			iCmdArgs++;
		}
	}

	// HACK: If there's a command line, it must be spawned on background...
	if (iCmdArgs > 0)
		sCmdLine += " &";

	// Alright with argument parsing.
	return true;
}


//---------------------------------------------------------------------------
// Combo box history persistence helper implementation.

void Settings::loadComboBoxHistory ( QComboBox *pComboBox, int iLimit )
{
	// Load combobox list from configuration settings file...
    _settings.beginGroup("/History/" + pComboBox->objectName());

    if (_settings.childKeys().count() > 0) {
		pComboBox->setUpdatesEnabled(false);
		pComboBox->setDuplicatesEnabled(false);
		pComboBox->clear();
		for (int i = 0; i < iLimit; i++) {
            const QString& sText = _settings.value(
				"/Item" + QString::number(i + 1)).toString();
			if (sText.isEmpty())
				break;
			pComboBox->addItem(sText);
		}
		pComboBox->setUpdatesEnabled(true);
	}

    _settings.endGroup();
}


void Settings::saveComboBoxHistory ( QComboBox *pComboBox, int iLimit )
{
	// Add current text as latest item...
	const QString& sCurrentText = pComboBox->currentText();
	int iCount = pComboBox->count();
	for (int i = 0; i < iCount; i++) {
		const QString& sText = pComboBox->itemText(i);
		if (sText == sCurrentText) {
			pComboBox->removeItem(i);
			iCount--;
			break;
		}
	}
	while (iCount >= iLimit)
		pComboBox->removeItem(--iCount);
	pComboBox->insertItem(0, sCurrentText);
	iCount++;

	// Save combobox list to configuration settings file...
    _settings.beginGroup("/History/" + pComboBox->objectName());
	for (int i = 0; i < iCount; i++) {
		const QString& sText = pComboBox->itemText(i);
		if (sText.isEmpty())
			break;
        _settings.setValue("/Item" + QString::number(i + 1), sText);
	}
    _settings.endGroup();
}


//---------------------------------------------------------------------------
// Splitter widget sizes persistence helper methods.

void Settings::loadSplitterSizes ( QSplitter *pSplitter,
	QList<int>& sizes )
{
	// Try to restore old splitter sizes...
	if (pSplitter) {
        _settings.beginGroup("/Splitter/" + pSplitter->objectName());
        QStringList list = _settings.value("/sizes").toStringList();
		if (!list.isEmpty()) {
			sizes.clear();
			QStringListIterator iter(list);
			while (iter.hasNext())
				sizes.append(iter.next().toInt());
		}
		pSplitter->setSizes(sizes);
        _settings.endGroup();
	}
}


void Settings::saveSplitterSizes ( QSplitter *pSplitter )
{
	// Try to save current splitter sizes...
	if (pSplitter) {
        _settings.beginGroup("/Splitter/" + pSplitter->objectName());
		QStringList list;
		QList<int> sizes = pSplitter->sizes();
		QListIterator<int> iter(sizes);
		while (iter.hasNext())
			list.append(QString::number(iter.next()));
		if (!list.isEmpty())
            _settings.setValue("/sizes", list);
        _settings.endGroup();
	}
}


//---------------------------------------------------------------------------
// Widget geometry persistence helper methods.

void Settings::loadWidgetGeometry ( QWidget *pWidget, bool bVisible )
{
	// Try to restore old form window positioning.
	if (pWidget) {
		QPoint wpos;
		QSize  wsize;
        _settings.beginGroup("/Geometry/" + pWidget->objectName());
        wpos.setX(_settings.value("/x", -1).toInt());
        wpos.setY(_settings.value("/y", -1).toInt());
        wsize.setWidth(_settings.value("/width", -1).toInt());
        wsize.setHeight(_settings.value("/height", -1).toInt());
        if (!bVisible) bVisible = _settings.value("/visible", false).toBool();
        _settings.endGroup();
//		new qjackctlDelayedSetup(pWidget, wpos, wsize,
//			bVisible && !bStartMinimized, (bDelayedSetup ? 1000 : 0));
	}
}


void Settings::saveWidgetGeometry(QWidget *pWidget, bool bVisible) {
	// Try to save form window position...
	// (due to X11 window managers ideossincrasies, we better
	// only save the form geometry while its up and visible)
	if (pWidget) {
        _settings.beginGroup("/Geometry/" + pWidget->objectName());
		const QPoint& wpos  = pWidget->pos();
		const QSize&  wsize = pWidget->size();
		if (!bVisible) bVisible = pWidget->isVisible();
        _settings.setValue("/x", wpos.x());
        _settings.setValue("/y", wpos.y());
        _settings.setValue("/width", wsize.width());
        _settings.setValue("/height", wsize.height());
        _settings.setValue("/visible", bVisible && !bStartMinimized);
        _settings.endGroup();
	}
}

