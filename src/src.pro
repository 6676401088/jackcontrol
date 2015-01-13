# qjackctl.pro
#
TARGET = qjackctl

TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

include(src.pri)

#DEFINES += DEBUG

HEADERS += \
    InterfaceComboBox.h \
    About.h \
    AboutDialog.h \
    MainWidget.h \
    Setup.h \
    SetupDialog.h \
    SystemTrayIcon.h \
    AlsaConnect.h \
    ConnectAlias.h \
    ConnectionsWidget.h \
    JackConnect.h \
    MessagesStatusWidget.h \
    SocketDialog.h \
    PatchbayFile.h \
    Patchbay.h \
    PatchbayRack.h \
    Session.h \
    SessionWidget.h \
    Status.h \
    ConnectViewSplitter.h \
    PatchbayWidget.h \
    MainWindow.h

SOURCES += \
    AboutDialog.cpp \
    Setup.cpp \
    SetupDialog.cpp \
    SystemTrayIcon.cpp \
    ConnectionsWidget.cpp \
    AlsaConnect.cpp \
    ConnectAlias.cpp \
    InterfaceComboBox.cpp \
    JackConnect.cpp \
    SocketDialog.cpp \
    Patchbay.cpp \
    MainWidget.cpp \
    MessagesStatusWidget.cpp \
    PatchbayFile.cpp \
    PatchbayForm.cpp \
    PatchbayRack.cpp \
    SessionWidget.cpp \
    Session.cpp \
    ConnectViewSplitter.cpp \
    MainWindow.cpp \
    Application.cpp

FORMS += \
    AboutDialog.ui \
    ConnectionsWidget.ui \
    MainWidget.ui \
    MessagesStatusWidget.ui \
    PatchbayWidget.ui \
    SessionWidget.ui \
    SetupDialog.ui \
    SocketDialog.ui \
    MainWindow.ui

RESOURCES += \
	qjackctl.qrc

TRANSLATIONS += \
	translations/qjackctl_cs.ts \
	translations/qjackctl_de.ts \
	translations/qjackctl_es.ts \
	translations/qjackctl_fr.ts \
	translations/qjackctl_it.ts \
	translations/qjackctl_ja.ts \
	translations/qjackctl_nl.ts \
	translations/qjackctl_ru.ts

unix {

	# variables
	OBJECTS_DIR = .obj
	MOC_DIR     = .moc
	UI_DIR      = .ui

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	BINDIR = $$PREFIX/bin
	DATADIR = $$PREFIX/share
	LOCALEDIR = $(localedir)

	DEFINES += DATADIR=\"$$DATADIR\"

	!isEmpty(LOCALEDIR) {
		DEFINES += LOCALEDIR=\"$$LOCALEDIR\"
	}

	# make install
	INSTALLS += target desktop icon

	target.path = $$BINDIR

	desktop.path = $$DATADIR/applications
	desktop.files += $${TARGET}.desktop

	icon.path = $$DATADIR/icons/hicolor/32x32/apps
	icon.files += images/$${TARGET}.png 
}

# XML/DOM support
QT += xml


# QT5 support
!lessThan(QT_MAJOR_VERSION, 5) {
        QT += widgets
}

INCLUDEPATH += .. ../qjack

LIBS += -L../qjack/lib \
                -lqjack \
                -ljack \
                -ljackserver
