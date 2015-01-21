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
    SystemTrayIcon.h \
    SocketDialog.h \
    PatchbayFile.h \
    PatchbayRack.h \
    Session.h \
    Status.h \
    statuswidget.h \
    patchbaywidget.h \
    connectionswidget.h \
    mainwindow.h \
    sessionwidget.h \
    setupwidget.h \
    aboutdialog.h \
    settings.h \
    jackservice.h \
    plugtreewidgetitem.h \
    sockettreewidgetitem.h \
    socketlist.h \
    sockettreewidget.h \
    patchworkwidget.h \
    patchbaysplitter.h \
    clientalias.h \
    connectalias.h \
    patchbay.h \
    mainwidget.h \
    connectionsdrawer/clienttreewidgetitem.h \
    connectionsdrawer/portconnectionswidget.h \
    connectionsdrawer/porttreewidgetitem.h \
    connectionsdrawer/connectionsdrawer.h \
    about.h \
    jackconnectionsdrawer/JackConnect.h \
    alsaconnectionsdrawer/AlsaConnect.h \
    jackconnectionsdrawer/jackconnectionsdrawer.h \
    alsaconnectionsdrawer/alsaconnectionsdrawer.h \
    connectionsdrawer/clienttreewidget.h \
    jackconnectionsdrawer/jackclienttreewidgetitem.h \
    jackconnectionsdrawer/jackaudioporttreewidgetitem.h

SOURCES += \
    SystemTrayIcon.cpp \
    InterfaceComboBox.cpp \
    SocketDialog.cpp \
    PatchbayFile.cpp \
    PatchbayRack.cpp \
    Session.cpp \
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
    plugtreewidgetitem.cpp \
    sockettreewidgetitem.cpp \
    socketlist.cpp \
    sockettreewidget.cpp \
    patchworkwidget.cpp \
    patchbaysplitter.cpp \
    clientalias.cpp \
    connectalias.cpp \
    patchbay.cpp \
    mainwidget.cpp \
    connectionsdrawer/clienttreewidgetitem.cpp \
    connectionsdrawer/portconnectionswidget.cpp \
    connectionsdrawer/porttreewidgetitem.cpp \
    connectionsdrawer/connectionsdrawer.cpp \
    jackconnectionsdrawer/JackConnect.cpp \
    alsaconnectionsdrawer/AlsaConnect.cpp \
    jackconnectionsdrawer/jackconnectionsdrawer.cpp \
    alsaconnectionsdrawer/alsaconnectionsdrawer.cpp \
    connectionsdrawer/clienttreewidget.cpp \
    jackconnectionsdrawer/jackclienttreewidgetitem.cpp \
    jackconnectionsdrawer/jackaudioporttreewidgetitem.cpp

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

INCLUDEPATH += \
    connectionsdrawer \
    jackconnectionsdrawer \
    alsaconnectionsdrawer \
     .. ../qjack

LIBS += -L../qjack/lib \
                -lqjack \
                -ljack \
                -ljackserver
