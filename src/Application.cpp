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

// Own includes
#include "About.h"
#include "Setup.h"
#include "MainWindow.h"

// Qt includes
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>

#define CONFIG_QUOTE1(x) #x
#define CONFIG_QUOTED(x) CONFIG_QUOTE1(x)

#if defined(DATADIR)
#define CONFIG_DATADIR CONFIG_QUOTED(DATADIR)
#else
#define CONFIG_DATADIR CONFIG_PREFIX "/share"
#endif

#if defined(LOCALEDIR)
#define CONFIG_LOCALEDIR CONFIG_QUOTED(LOCALEDIR)
#else
#define CONFIG_LOCALEDIR CONFIG_DATADIR "/locale"
#endif

#if defined(Q_WS_X11)

#include <unistd.h>

#include <QX11Info>

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#define QJACKCTL_XUNIQUE "qjackctlApplication"

#endif


class Application : public QApplication
{
public:

    Application(int& argc, char **argv) : QApplication(argc, argv),
        m_pQtTranslator(0), m_pMyTranslator(0), _mainWindow(0)
	{
		// Load translation support.
		QLocale loc;
		if (loc.language() != QLocale::C) {
			// Try own Qt translation...
			m_pQtTranslator = new QTranslator(this);
			QString sLocName = "qt_" + loc.name();
			QString sLocPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
			if (m_pQtTranslator->load(sLocName, sLocPath)) {
				QApplication::installTranslator(m_pQtTranslator);
			} else {
				delete m_pQtTranslator;
				m_pQtTranslator = 0;
			#ifdef CONFIG_DEBUG
				qWarning("Warning: no translation found for '%s' locale: %s/%s.qm",
					loc.name().toUtf8().constData(),
					sLocPath.toUtf8().constData(),
					sLocName.toUtf8().constData());
			#endif
			}
			// Try own application translation...
			m_pMyTranslator = new QTranslator(this);
			sLocName = "qjackctl_" + loc.name();
			if (m_pMyTranslator->load(sLocName, sLocPath)) {
				QApplication::installTranslator(m_pMyTranslator);
			} else {
				sLocPath = CONFIG_LOCALEDIR;
				if (m_pMyTranslator->load(sLocName, sLocPath)) {
					QApplication::installTranslator(m_pMyTranslator);
				} else {
					delete m_pMyTranslator;
					m_pMyTranslator = 0;
				#ifdef CONFIG_DEBUG
					qWarning("Warning: no translation found for '%s' locale: %s/%s.qm",
						loc.name().toUtf8().constData(),
						sLocPath.toUtf8().constData(),
						sLocName.toUtf8().constData());
				#endif
				}
			}
		}
	#if defined(Q_WS_X11)
		m_pDisplay = NULL;
		m_wOwner = None;
	#endif
	}

	// Destructor.
    ~Application()
	{
		if (m_pMyTranslator) delete m_pMyTranslator;
		if (m_pQtTranslator) delete m_pQtTranslator;
	}

	// Main application widget accessors.
    void setMainWindow(MainWindow *mainWindow)
	{
        _mainWindow = mainWindow;
//	#if defined(Q_WS_X11)
//		if (m_pDisplay) {
//			XGrabServer(m_pDisplay);
//			m_wOwner = m_pWidget->winId();
//			XSetSelectionOwner(m_pDisplay, m_aUnique, m_wOwner, CurrentTime);
//			XUngrabServer(m_pDisplay);
//		}
//	#endif
	}

    MainWindow *mainWindow() const { return _mainWindow; }

	// Check if another instance is running,
    // and raise its proper main widget...
	bool setup(const QString& sServerName)
	{
	#if defined(Q_WS_X11)
		m_pDisplay = QX11Info::display();
		QString sUnique = QJACKCTL_XUNIQUE;
		if (sServerName.isEmpty()) {
			const char *pszServerName = ::getenv("JACK_DEFAULT_SERVER");
			if (pszServerName) {
				sUnique += '_';
				sUnique += pszServerName;
			}
		} else {
			sUnique += '_';
			sUnique += sServerName;
		}
		char szHostName[255];
		if (::gethostname(szHostName, sizeof(szHostName)) == 0) {
			sUnique += '@';
			sUnique += szHostName;
		}
		m_aUnique = XInternAtom(m_pDisplay, sUnique.toUtf8().constData(), false);
		XGrabServer(m_pDisplay);
		m_wOwner = XGetSelectionOwner(m_pDisplay, m_aUnique);
		XUngrabServer(m_pDisplay);
		if (m_wOwner != None) {
			// First, notify any freedesktop.org WM
			// that we're about to show the main widget...
			Screen *pScreen = XDefaultScreenOfDisplay(m_pDisplay);
			int iScreen = XScreenNumberOfScreen(pScreen);
			XEvent ev;
			memset(&ev, 0, sizeof(ev));
			ev.xclient.type = ClientMessage;
			ev.xclient.display = m_pDisplay;
			ev.xclient.window = m_wOwner;
			ev.xclient.message_type = XInternAtom(m_pDisplay, "_NET_ACTIVE_WINDOW", false);
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = 0; // Source indication.
			ev.xclient.data.l[1] = 0; // Timestamp.
			ev.xclient.data.l[2] = 0; // Requestor's currently active window (none)
			ev.xclient.data.l[3] = 0;
			ev.xclient.data.l[4] = 0;
			XSelectInput(m_pDisplay, m_wOwner, StructureNotifyMask);
			XSendEvent(m_pDisplay, RootWindow(m_pDisplay, iScreen), false,
				(SubstructureNotifyMask | SubstructureRedirectMask), &ev);
			XSync(m_pDisplay, false);
			XRaiseWindow(m_pDisplay, m_wOwner);
			// And then, let it get caught on destination
			// by QApplication::x11EventFilter...
			QByteArray value = QJACKCTL_XUNIQUE;
			XChangeProperty(
				m_pDisplay,
				m_wOwner,
				m_aUnique,
				m_aUnique, 8,
				PropModeReplace,
				(unsigned char *) value.data(),
				value.length());
			// Done.
			return true;
		}
	#endif
		return false;
	}

#if defined(Q_WS_X11)
	bool x11EventFilter(XEvent *pEv)
	{
		if (m_pWidget && m_wOwner != None
			&& pEv->type == PropertyNotify
			&& pEv->xproperty.window == m_wOwner
			&& pEv->xproperty.state == PropertyNewValue) {
			// Always check whether our property-flag is still around...
			Atom aType;
			int iFormat = 0;
			unsigned long iItems = 0;
			unsigned long iAfter = 0;
			unsigned char *pData = 0;
			if (XGetWindowProperty(
					m_pDisplay,
					m_wOwner,
					m_aUnique,
					0, 1024,
					false,
					m_aUnique,
					&aType,
					&iFormat,
					&iItems,
					&iAfter,
					&pData) == Success
				&& aType == m_aUnique && iItems > 0 && iAfter == 0) {
				// Avoid repeating it-self...
				XDeleteProperty(m_pDisplay, m_wOwner, m_aUnique);
				// Just make it always shows up fine...
				m_pWidget->show();
				m_pWidget->raise();
				m_pWidget->activateWindow();
				// FIXME: Do our best speciality, although it should be
				// done iif configuration says so, we'll do it anyway!
				qjackctlMainForm *pMainForm = qjackctlMainForm::getInstance();
				if (pMainForm)
					pMainForm->startJack();
			}
			// Free any left-overs...
			if (iItems > 0 && pData)
				XFree(pData);
		}
		return QApplication::x11EventFilter(pEv);
	}
#endif

	// Session shutdown handler.
	void commitData(QSessionManager& sm)
	{
//        MainWidget *pMainForm = MainWidget::getInstance();
//		if (pMainForm)
//			pMainForm->setQuitForce(true);
	}

private:
	// Translation support.
	QTranslator *m_pQtTranslator;
	QTranslator *m_pMyTranslator;

	// Instance variables.
    MainWindow *_mainWindow;

#if defined(Q_WS_X11)
	Display *m_pDisplay;
	Atom     m_aUnique;
	Window   m_wOwner;
#endif
};


//-------------------------------------------------------------------------
// stacktrace - Signal crash handler.
//

#ifdef CONFIG_STACKTRACE
#if defined(__GNUC__) && defined(Q_OS_LINUX)

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void stacktrace ( int signo )
{
	pid_t pid;
	int rc;
	int status = 0;
	char cmd[80];

	// Reinstall default handler; prevent race conditions...
	signal(signo, SIG_DFL);

	static const char *shell  = "/bin/sh";
	static const char *format = "gdb -q --batch --pid=%d"
		" --eval-command='thread apply all bt'";

	snprintf(cmd, sizeof(cmd), format, (int) getpid());

	pid = fork();

	// Fork failure!
	if (pid < 0)
		return;

	// Fork child...
	if (pid == 0) {
		execl(shell, shell, "-c", cmd, NULL);
		_exit(1);
		return;
	}

	// Parent here: wait for child to terminate...
	do { rc = waitpid(pid, &status, 0); }
	while ((rc < 0) && (errno == EINTR));

	// Dispatch any logging, if any...
	QApplication::processEvents(QEventLoop::AllEvents, 3000);

	// Make sure everyone terminates...
	kill(pid, SIGTERM);
	_exit(1);
}

#endif
#endif

int main ( int argc, char **argv )
{
	Q_INIT_RESOURCE(qjackctl);
#ifdef CONFIG_STACKTRACE
#if defined(__GNUC__) && defined(Q_OS_LINUX)
	signal(SIGILL,  stacktrace);
	signal(SIGFPE,  stacktrace);
	signal(SIGSEGV, stacktrace);
	signal(SIGABRT, stacktrace);
	signal(SIGBUS,  stacktrace);
#endif
#endif
    Application app(argc, argv);

	// Construct default settings; override with command line arguments.
    Setup settings;
	if (!settings.parse_args(app.arguments())) {
		app.quit();
		return 1;
	}

	// Check if we'll just start an external program...
	if (!settings.sCmdLine.isEmpty()) {
		jack_client_t *pJackClient
			= jack_client_open("qjackctl-start", JackNoStartServer, NULL);
		if (pJackClient) {
			jack_client_close(pJackClient);
			int iExitStatus = ::system(settings.sCmdLine.toUtf8().constData());
			app.quit();
			return iExitStatus;
		}
	}

	// Have another instance running?
	if (settings.bSingleton) {
		if (app.setup(settings.sServerName)) {
			app.quit();
			return 2;
		}
	}

	// Dark themes grayed/disabled color group fix...
	QPalette pal(app.palette());
	if (pal.base().color().value() < 0x7f) {
	#if QT_VERSION >= 0x050000
		const QColor& color = pal.window().color();
		const int iGroups = int(QPalette::Active | QPalette::Inactive) + 1;
		for (int i = 0; i < iGroups; ++i) {
			const QPalette::ColorGroup group = QPalette::ColorGroup(i);
			pal.setBrush(group, QPalette::Light,    color.lighter(150));
			pal.setBrush(group, QPalette::Midlight, color.lighter(120));
			pal.setBrush(group, QPalette::Dark,     color.darker(150));
			pal.setBrush(group, QPalette::Mid,      color.darker(120));
			pal.setBrush(group, QPalette::Shadow,   color.darker(200));
		}
	//	pal.setColor(QPalette::Disabled, QPalette::ButtonText, pal.mid().color());
	#endif
		pal.setColorGroup(QPalette::Disabled,
			pal.windowText().color().darker(),
			pal.button(),
			pal.light(),
			pal.dark(),
			pal.mid(),
			pal.text().color().darker(),
			pal.text().color().lighter(),
			pal.base(),
			pal.window());
		app.setPalette(pal);
	}

	// Set default base font...
	int iBaseFontSize = app.font().pointSize();
	if (settings.iBaseFontSize > 0)
		iBaseFontSize = settings.iBaseFontSize;
	app.setFont(QFont(app.font().family(), iBaseFontSize));

	// What style do we create these forms?
	Qt::WindowFlags wflags = Qt::Window
		| Qt::CustomizeWindowHint
		| Qt::WindowTitleHint
		| Qt::WindowSystemMenuHint
		| Qt::WindowMinMaxButtonsHint
		| Qt::WindowCloseButtonHint;
	if (settings.bKeepOnTop)
		wflags |= Qt::Tool;


	// Construct the main form, and show it to the world.
    MainWindow mainWindow(0, wflags);

    mainWindow.setup(&settings);
	// If we have a systray icon, we'll skip this.
	if (!settings.bSystemTray) {
        mainWindow.show();
        mainWindow.adjustSize();
	}

	// Settle this one as application main widget...
    app.setMainWindow(&mainWindow);

	return app.exec();
}

// end of qjackctl.cpp