# qjackctl.pri
#
PREFIX  = /usr/local
CONFIG += release qdbus
INCLUDEPATH += /usr/local/include /usr/include /usr/include/qt5 
LIBS += -L/usr/local/lib -L/usr/lib  -ljack -lasound -lportaudio -lX11
