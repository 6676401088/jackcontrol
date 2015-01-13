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

// Own includes
#include "MainWindow.h"
#include "SetupDialog.h"
#include "AboutDialog.h"

// uic includes
#include "ui_MainWindow.h"

// QJackAudio includes
#include <System>
#include <Client>
#include <Server>

// Qt includes
#include <QDebug>

#if !defined(WIN32)
#include <unistd.h>
#endif

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags windowFlags) :
    QMainWindow(parent, windowFlags),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupStdOutRedirect();
    setupStatusTab();

    _jackServer = new QJack::Server();

    //hw:PCH,0|hw:PCH,0|256|3|44100|0|0|nomon|swmeter|-|32bit

    _jackClient = new QJack::Client();
    connect(_jackClient, SIGNAL(connectedToServer()), this, SLOT(handleConnectedToServer()));
    connect(_jackClient, SIGNAL(disconnectedFromServer()), this, SLOT(handleDisconnectedFromServer()));

    connect(QJack::System::instance(), SIGNAL(error(QString)), this, SLOT(handleError(QString)));

    ui->actionStartJackServer->setEnabled(true);
    ui->actionStopJackServer->setEnabled(false);
}

bool MainWindow::setup(Setup *setup)
{
    // Finally, fix settings descriptor
    // and stabilize the form.
    _setup = setup;
    return true;


//    // All forms are to be created right now.
//    m_pMessagesStatusForm = new MessagesStatusWidget (pParent, wflags);
//    m_pSessionForm     = new SessionWidget     (pParent, wflags);
//    m_pConnectionsForm = new ConnectionsWidget (pParent, wflags);
//    m_pPatchbayForm    = new PatchbayWidget    (pParent, wflags);
//    // Setup appropriately...
//    m_pMessagesStatusForm->setTabPage(_setup->iMessagesStatusTabPage);
//    m_pMessagesStatusForm->setLogging(
//        _setup->bMessagesLog, _setup->sMessagesLogPath);
//    m_pSessionForm->setup(_setup);
//    m_pConnectionsForm->setTabPage(_setup->iConnectionsTabPage);
//    m_pConnectionsForm->setup(_setup);
//    m_pPatchbayForm->setup(_setup);

//    // Set the patchbay cable connection notification signal/slot.
//    QObject::connect(m_pPatchbayRack, SIGNAL(cableConnected(const QString&, const QString&, unsigned int)),
//        this, SLOT(cableConnectSlot(const QString&, const QString&, unsigned int)));

//    // Try to restore old window positioning and appearence.
//    _setup->loadWidgetGeometry(this, true);

//    // Make it final show...
//    m_ui.StatusDisplayFrame->show();

//    // Set other defaults...
//    updateDisplayEffect();
//    updateTimeDisplayFonts();
//    updateTimeDisplayToolTips();
//    updateTimeFormat();
//    updateMessagesFont();
//    updateMessagesLimit();
//    updateConnectionsFont();
//    updateConnectionsIconSize();
//    updateJackClientPortAlias();
//    updateBezierLines();
//    updateActivePatchbay();
//    updateSystemTray();

//    // And for the whole widget gallore...
//    _setup->loadWidgetGeometry(m_pMessagesStatusForm);
//    _setup->loadWidgetGeometry(m_pSessionForm);
//    _setup->loadWidgetGeometry(m_pConnectionsForm);
//    _setup->loadWidgetGeometry(m_pPatchbayForm);

//    // Initial XRUN statistics reset.
//    resetXrunStats();

//#ifdef CONFIG_ALSA_SEQ
//    if (m_pSetup->bAlsaSeqEnabled) {
//        // Start our ALSA sequencer interface.
//        if (snd_seq_open(&m_pAlsaSeq, "hw", SND_SEQ_OPEN_DUPLEX, 0) < 0)
//            m_pAlsaSeq = NULL;
//        if (m_pAlsaSeq) {
//            snd_seq_port_subscribe_t *pAlsaSubs;
//            snd_seq_addr_t seq_addr;
//            struct pollfd pfd[1];
//            int iPort = snd_seq_create_simple_port(
//                m_pAlsaSeq,	"qjackctl",
//                SND_SEQ_PORT_CAP_WRITE
//                | SND_SEQ_PORT_CAP_SUBS_WRITE
//                | SND_SEQ_PORT_CAP_NO_EXPORT,
//                SND_SEQ_PORT_TYPE_APPLICATION
//            );
//            if (iPort >= 0) {
//                snd_seq_port_subscribe_alloca(&pAlsaSubs);
//                seq_addr.client = SND_SEQ_CLIENT_SYSTEM;
//                seq_addr.port   = SND_SEQ_PORT_SYSTEM_ANNOUNCE;
//                snd_seq_port_subscribe_set_sender(pAlsaSubs, &seq_addr);
//                seq_addr.client = snd_seq_client_id(m_pAlsaSeq);
//                seq_addr.port   = iPort;
//                snd_seq_port_subscribe_set_dest(pAlsaSubs, &seq_addr);
//                snd_seq_subscribe_port(m_pAlsaSeq, pAlsaSubs);
//                snd_seq_poll_descriptors(m_pAlsaSeq, pfd, 1, POLLIN);
//                m_pAlsaNotifier
//                    = new QSocketNotifier(pfd[0].fd, QSocketNotifier::Read);
//                QObject::connect(m_pAlsaNotifier,
//                    SIGNAL(activated(int)),
//                    SLOT(alsaNotifySlot(int)));
//            }
//        }
//        // Could we start without it?
//        if (m_pAlsaSeq) {
//            // Rather obvious setup.
//            if (m_pConnectionsForm)
//                m_pConnectionsForm->stabilizeAlsa(true);
//        } else {
//            appendMessagesError(
//                tr("Could not open ALSA sequencer as a client.\n\n"
//                "ALSA MIDI patchbay will be not available."));
//        }
//    }
//#endif
//#ifdef CONFIG_DBUS
//    // Register D-Bus service...
//    if (m_pSetup->bDBusEnabled) {
//        const QString s; // Just an empty string.
//        const QString sDBusName("org.rncbc.qjackctl");
//        QDBusConnection dbus = QDBusConnection::systemBus();
//        dbus.connect(s, s, sDBusName, "start",
//            this, SLOT(startJack()));
//        dbus.connect(s, s, sDBusName, "stop",
//            this, SLOT(stopJack()));
//        dbus.connect(s, s, sDBusName, "main",
//            this, SLOT(toggleMainForm()));
//        dbus.connect(s, s, sDBusName, "messages",
//            this, SLOT(toggleMessagesForm()));
//        dbus.connect(s, s, sDBusName, "status",
//            this, SLOT(toggleStatusForm()));
//        dbus.connect(s, s, sDBusName, "session",
//            this, SLOT(toggleSessionForm()));
//        dbus.connect(s, s, sDBusName, "connections",
//            this, SLOT(toggleConnectionsForm()));
//        dbus.connect(s, s, sDBusName, "patchbay",
//            this, SLOT(togglePatchbayForm()));
//        dbus.connect(s, s, sDBusName, "rewind",
//            this, SLOT(transportRewind()));
//        dbus.connect(s, s, sDBusName, "backward",
//            this, SLOT(transportBackward()));
//        dbus.connect(s, s, sDBusName, "play",
//            this, SLOT(transportStart()));
//        dbus.connect(s, s, sDBusName, "pause",
//            this, SLOT(transportStop()));
//        dbus.connect(s, s, sDBusName, "forward",
//            this, SLOT(transportForward()));
//        dbus.connect(s, s, sDBusName, "setup",
//            this, SLOT(showSetupForm()));
//        dbus.connect(s, s, sDBusName, "about",
//            this, SLOT(showAboutForm()));
//        dbus.connect(s, s, sDBusName, "quit",
//            this, SLOT(quitMainForm()));
//        dbus.connect(s, s, sDBusName, "preset",
//            this, SLOT(activatePreset(const QString&)));
//        // Session related slots...
//        if (m_pSessionForm) {
//            dbus.connect(s, s, sDBusName, "load",
//                m_pSessionForm, SLOT(loadSession()));
//            dbus.connect(s, s, sDBusName, "save",
//                m_pSessionForm, SLOT(saveSessionSave()));
//        #ifdef CONFIG_JACK_SESSION
//            dbus.connect(s, s, sDBusName, "savequit",
//                m_pSessionForm, SLOT(saveSessionSaveAndQuit()));
//            dbus.connect(s, s, sDBusName, "savetemplate",
//                m_pSessionForm, SLOT(saveSessionSaveTemplate()));
//        #endif
//        }
//        // Detect whether jackdbus is avaliable...
//        QDBusConnection dbusc = QDBusConnection::sessionBus();
//        m_pDBusControl = new QDBusInterface(
//            "org.jackaudio.service",		// Service
//            "/org/jackaudio/Controller",	// Path
//            "org.jackaudio.JackControl",	// Interface
//            dbusc);							// Connection
//        QDBusMessage dbusm = m_pDBusControl->call("IsStarted");
//        if (dbusm.type() == QDBusMessage::ReplyMessage) {
//            // Yes, jackdbus is available and/or already started
//            // -- use jackdbus control interface...
//            appendMessages(tr("D-BUS: Service is available (%1 aka jackdbus).")
//                .arg(m_pDBusControl->service()));
//            // Parse reply (should be boolean)
//            m_bDBusStarted = dbusm.arguments().first().toBool();
//            // Register server start/stop notification slots...
//            dbusc.connect(
//                m_pDBusControl->service(),
//                m_pDBusControl->path(),
//                m_pDBusControl->interface(),
//                "ServerStarted", this,
//                SLOT(jackStarted()));
//            dbusc.connect(
//                m_pDBusControl->service(),
//                m_pDBusControl->path(),
//                m_pDBusControl->interface(),
//                "ServerStopped", this,
//                SLOT(jackFinished()));
//            // -- use jackdbus configure interface...
//            m_pDBusConfig = new QDBusInterface(
//                m_pDBusControl->service(),	// Service
//                m_pDBusControl->path(),		// Path
//                "org.jackaudio.Configure",	// Interface
//                m_pDBusControl->connection());	// Connection
//            // Start our log watcher thread...
//            m_pDBusLogWatcher = new qjackctlDBusLogWatcher(
//                QDir::homePath() + "/.log/jack/jackdbus.log");
//            m_pDBusLogWatcher->start();
//            // Ready now.
//        } else {
//            // No, jackdbus is not available, not started
//            // or not even installed -- use classic jackd, BAU...
//            appendMessages(tr("D-BUS: Service not available (%1 aka jackdbus).")
//                .arg(m_pDBusControl->service()));
//            // Destroy tentative jackdbus interface.
//            delete m_pDBusControl;
//            m_pDBusControl = NULL;
//        }
//    }
//#endif

//    // Load patchbay form recent paths...
//    if (m_pPatchbayForm) {
//        m_pPatchbayForm->setRecentPatchbays(_setup->patchbays);
//        if (!_setup->sPatchbayPath.isEmpty()
//            && QFileInfo(_setup->sPatchbayPath).exists())
//            m_pPatchbayForm->loadPatchbayFile(_setup->sPatchbayPath);
//        m_pPatchbayForm->updateRecentPatchbays();
//        m_pPatchbayForm->stabilizeForm();
//    }

//    // Try to find if we can start in detached mode (client-only)
//    // just in case there's a JACK server already running.
//    startJackClient(true);
//    // Final startup stabilization...
//    stabilizeForm();
//    jackStabilize();

//    // Look for immediate server startup?...
//    if (_setup->bStartJack || !_setup->sCmdLine.isEmpty())
//        _setup->bStartJackCmd = true;
//    if (_setup->bStartJackCmd)
//        startJack();

//    // Register the first timer slot.
//    QTimer::singleShot(QJACKCTL_TIMER_MSECS, this, SLOT(timerSlot()));

//    // We're ready to go...
    //return true;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionStartJackServer_triggered() {
    QJack::DriverMap drivers = _jackServer->availableDrivers();
    QJack::Driver alsaDriver = drivers["alsa"];

    QJack::ParameterMap parameters = alsaDriver.parameters();
    parameters["rate"].setValue(44100);
    parameters["device"].setValue("hw:PCH,0");

    _jackServer->start(alsaDriver);


    _jackClient->connectToServer("QJackControl2");
    ui->actionStartJackServer->setEnabled(false);
}

void MainWindow::on_actionStopJackServer_triggered() {
    _jackClient->disconnectFromServer();
    _jackServer->stop();
    ui->actionStopJackServer->setEnabled(false);
    ui->actionStartJackServer->setEnabled(true);
}

void MainWindow::on_actionSetup_triggered() {
    SetupDialog setupDialog(this);
    setupDialog.exec();
}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionTransportRewind_triggered() {

}

void MainWindow::on_actionTransportBackward_triggered() {

}

void MainWindow::on_actionTransportStart_triggered() {
    _jackClient->startTransport();
    ui->actionTransportStop->setChecked(false);
}

void MainWindow::on_actionTransportStop_triggered() {
    _jackClient->stopTransport();
    ui->actionTransportStart->setChecked(false);

}

void MainWindow::on_actionTransportForward_triggered() {

}

void MainWindow::handleConnectedToServer() {

    _jackClient->registerAudioOutPort("test");
    _jackClient->activate();

    ui->actionStopJackServer->setEnabled(true);
}

void MainWindow::handleDisconnectedFromServer() {

    ui->actionStartJackServer->setEnabled(true);
}

void MainWindow::handleError(QString errorMessage) {
    message(errorMessage, MessageTypeError);
}

void MainWindow::stdOutActivated(int fileDescriptor) {
    char achBuffer[1024];
    int  cchBuffer = ::read(fileDescriptor, achBuffer, sizeof(achBuffer) - 1);
    if (cchBuffer > 0) {
        achBuffer[cchBuffer] = (char) 0;
        message(achBuffer, MessageTypeStdOut);
    }
}

void MainWindow::setupStdOutRedirect() {
#if !defined(WIN32)
    // Redirect our own stdout/stderr.
    int pipeDescriptors[2];
    if(::pipe(pipeDescriptors) == 0) {
        ::dup2(pipeDescriptors[1], STDOUT_FILENO);
        ::dup2(pipeDescriptors[1], STDERR_FILENO);
        _stdOutSocketNotifier = new QSocketNotifier(pipeDescriptors[0], QSocketNotifier::Read, this);
        connect(_stdOutSocketNotifier, SIGNAL(activated(int)), this, SLOT(stdOutActivated(int)));
    } else {
        message(tr("Failed to redirect messages from stdout/stderr."));
    }
#endif
}

void MainWindow::setupStatusTab() {

}


void MainWindow::message(QString message, MessageType messageType) {
    QString color;
    QString timeStamp = QTime::currentTime().toString("[hh:mm:ss.zzz]");

    switch (messageType) {
    case MessageTypeNormal:
        color = "#888888";
        break;
    case MessageTypeError:
        color = "#ff3333";
        break;
    case MessageTypeStdOut:
        color = "#229922";
        break;
    }
    message.replace("\n", "<br />");
    ui->messagesTextEdit->append(QString("<font color=\"%1\">%2 %3</font>")
                                 .arg(color)
                                 .arg(timeStamp)
                                 .arg(message));
}
