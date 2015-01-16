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
    MainWidget.h \
    SystemTrayIcon.h \
    AlsaConnect.h \
    JackConnect.h \
    SocketDialog.h \
    PatchbayFile.h \
    Patchbay.h \
    PatchbayRack.h \
    Session.h \
    Status.h \
    ConnectViewSplitter.h \
    statuswidget.h \
    patchbaywidget.h \
    connectionswidget.h \
    mainwindow.h \
    sessionwidget.h \
    setupwidget.h \
    aboutdialog.h \
    settings.h \
    jackservice.h \
    connectionsmodel.h \
    jackportconnectorwidget.h \
    clientlisttreewidget.h \
    jackporttreewidgetitem.h \
    jackclienttreewidgetitem.h \
    jackclientlist.h \
    plugtreewidgetitem.h \
    sockettreewidgetitem.h \
    socketlist.h \
    sockettreewidget.h \
    patchworkwidget.h \
    patchbaysplitter.h \
    clientalias.h \
    connectalias.h

SOURCES += \
    SystemTrayIcon.cpp \
    AlsaConnect.cpp \
    InterfaceComboBox.cpp \
    JackConnect.cpp \
    SocketDialog.cpp \
    Patchbay.cpp \
    MainWidget.cpp \
    PatchbayFile.cpp \
    PatchbayRack.cpp \
    Session.cpp \
    ConnectViewSplitter.cpp \
    statuswidget.cpp \
    patchbaywidget.cpp \
    connectionswidget.cpp \
    mainwindow.cpp \
    sessionwidget.cpp \
    setupwidget.cpp \
    aboutdialog.cpp \
    settings.cpp \
    jackservice.cpp \
    application.cpp \
    connectionsmodel.cpp \
    jackportconnectorwidget.cpp \
    clientlisttreewidget.cpp \
    jackporttreewidgetitem.cpp \
    jackclienttreewidgetitem.cpp \
    jackclientlist.cpp \
    plugtreewidgetitem.cpp \
    sockettreewidgetitem.cpp \
    socketlist.cpp \
    sockettreewidget.cpp \
    patchworkwidget.cpp \
    patchbaysplitter.cpp \
    clientalias.cpp \
    connectalias.cpp

FORMS += \
    SocketDialog.ui \
    statuswidget.ui \
    patchbaywidget.ui \
    connectionswidget.ui \
    mainwidget.ui \
    mainwindow.ui \
    sessionwidget.ui \
    setupwidget.ui \
    aboutdialog.ui

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
