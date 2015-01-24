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
#include "about.h"
#include "settings.h"
#include "mainwindow.h"
#include "systemtrayicon.h"

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

class Application : public QApplication {
public:

    Application(int& argc, char **argv)
        : QApplication(argc, argv) {
        loadTranslators();
	}

    ~Application() {
	}

    void loadTranslators() {
        m_pQtTranslator = 0;
        m_pMyTranslator = 0;
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
    }

private:
	QTranslator *m_pQtTranslator;
	QTranslator *m_pMyTranslator;

};

int main(int argc, char **argv) {
    Application app(argc, argv);
    SystemTrayIcon systemTrayIcon;
    systemTrayIcon.setIcon(QIcon("://images/aportlni_64x64.png"));
    systemTrayIcon.show();
	// Construct default settings; override with command line arguments.
    Settings settings;
	if (!settings.parse_args(app.arguments())) {
		app.quit();
		return 1;
	}

    MainWindow mainWindow;
    mainWindow.show();
	return app.exec();
}
