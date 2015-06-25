DEFINES += \
  SUPPORTS_ALSA \
  SUPPORTS_SUN \
  SUPPORTS_OSS \
  #SUPPORTS_COREAUDIO \
  #SUPPORTS_PORTAUDIO

TARGET = jackcontrol
TEMPLATE = app
QT += widgets

contains(DEFINES, SUPPORTS_ALSA) {
  LIBS += -lasound
}

INCLUDEPATH += \
    connectionsdrawer \
    jackconnectionsdrawer

HEADERS += \
    statuswidget.h \
    connectionswidget.h \
    mainwindow.h \
    jackservice.h \
    connectionsdrawer/clienttreewidgetitem.h \
    connectionsdrawer/portconnectionswidget.h \
    connectionsdrawer/porttreewidgetitem.h \
    connectionsdrawer/connectionsdrawer.h \
    jackconnectionsdrawer/jackconnectionsdrawer.h \
    connectionsdrawer/clienttreewidget.h \
    jackconnectionsdrawer/jackclienttreewidgetitem.h \
    jackconnectionsdrawer/jackaudioporttreewidgetitem.h \
    jackconnectionsdrawer/jackmidiporttreewidgetitem.h \
    settings.h \
    jackpresetswidget.h \
    soundcardcombobox.h

SOURCES += \
    statuswidget.cpp \
    connectionswidget.cpp \
    mainwindow.cpp \
    jackservice.cpp \
    application.cpp \
    connectionsdrawer/clienttreewidgetitem.cpp \
    connectionsdrawer/portconnectionswidget.cpp \
    connectionsdrawer/porttreewidgetitem.cpp \
    connectionsdrawer/connectionsdrawer.cpp \
    jackconnectionsdrawer/jackconnectionsdrawer.cpp \
    connectionsdrawer/clienttreewidget.cpp \
    jackconnectionsdrawer/jackclienttreewidgetitem.cpp \
    jackconnectionsdrawer/jackaudioporttreewidgetitem.cpp \
    jackconnectionsdrawer/jackmidiporttreewidgetitem.cpp \
    settings.cpp \
    jackpresetswidget.cpp \
    soundcardcombobox.cpp

FORMS += \
    statuswidget.ui \
    connectionswidget.ui \
    mainwindow.ui \
    jackpresetswidget.ui

RESOURCES += \
    resources.qrc

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

include(../pods.pri)
