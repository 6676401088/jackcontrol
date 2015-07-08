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
    connectionswidget.h \
    mainwindow.h \
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
    soundcardcombobox.h \
    jackcontrol.h \
    messageshighlighter.h \
    jackscopewidget.h

SOURCES += \
    connectionswidget.cpp \
    mainwindow.cpp \
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
    soundcardcombobox.cpp \
    jackcontrol.cpp \
    messageshighlighter.cpp \
    jackscopewidget.cpp

FORMS += \
    connectionswidget.ui \
    mainwindow.ui \
    jackpresetswidget.ui \
    jackscopewidget.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translations/jackcontrol_de_DE.ts

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  BINDIR    = $$PREFIX/bin
  SHAREDIR  = $$PREFIX/share
  LOCALEDIR = $(localedir)

  DEFINES   += SHAREDIR=\"$$SHAREDIR\"

  !isEmpty(LOCALEDIR) {
    DEFINES += LOCALEDIR=\"$$LOCALEDIR\"
  }

  # make install
  INSTALLS += target desktop icon

  target.path = $$BINDIR

  desktop.path = $$DATADIR/applications
  desktop.files += $${TARGET}.desktop

  icon.path = $$DATADIR/icons
  icon.files += images/jackcontrol.svg
}

OTHER_FILES = \
  deploy/jackcontrol.apparmor \
  deploy/jackcontrol.desktop

include(../pods.pri)

LIBS += -ldl
