Support this and other free software projects of mine by donating bitcoins:
```cpp
1HdLeSj4SfRC77tb6pDUPqeXGiw3T2RqrG
```
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=9WB9VJA9RGWTN)

JACK Control - JACK Visual Control Interface
-----------------------------------------------------

![Demo Screenshot](https://github.com/cybercatalyst/qjackctl/blob/master/jackcontrol.png "Demo screenshot")

JACK Control is a simple application to control the JACK sound server
(http://jackaudio.org). It is based on the original qjackctl, but has undergone
huge improvements in the code.

License: GNU General Public License (GPL) Version 2+
Icons: Some icons were take from the Faenza Icon Kit licensed under GNU GPLv3

Requirements
------------

In contradiction to qjackctl, JACK Control does not use the autotools build
system anymore. It can be build with Qt4 as well as Qt5. You will need Qt and
either the JACK1 or JACK2 libraries to build. For JACK2, please enable the
following line you jackcontrol.pro:

`#DEFINES += QTJACK_JACK2_SUPPORT`

How to build
------------
git clone --recursive https://github.com/cybercatalyst/qjackctl.git

Note the "--recursive"-option which is very important, since it clones the
qtjack pod into its subdirectory.

Open the top level jackcontrol.pro and build.

How to create a debian package
------------------------------
First, copy the source directory to another folder:
```
mkdir deb
cp -R jackcontrol deb
cd deb/jackcontrol
```

Then remove the .git folder and build:
```
rm -rf .git
qmake-qt4
make
```

Chown all file to root and cd out:
```
sudo chown -R root:root .
cd ..
```

Then build the package:
```
dpkg-deb --build jackcontrol
```

You'll find the package in the deb folder.

Known bugs
----------
Sometimes the build order gets confused. This is easy to solve: just trigger a
build of qtjack, then build jackcontrol.

Warning
-------
This software has undergone major changes, it is almost a complete rewrite of
qjackctl. Please note that this software is not ready to everyday use. However,
it would be nice if you leave some feedback or improvements.

What is qtjack?
---------------
QtJack is a pod (ie an includable subproject, see qt-pods.org for more info)
that allows you to access the JACK server in a Qt app nicely. It wraps the C
API of libjack, libjackserver and libjacknet in objects. In qjackctl, this
code was mixed up with user interface code.

Support
-------

JACK Control is free software. For bug reports, feature
requests, discussion forums, mailling lists, or any other matter
related to the development of this piece of software, please use the github
bugtracker or write a mail to jacob@omg-it.works .

Acknowledgments (from original the qjackctl README)
---------------------------------------------------

QjackCtl's user interface layout (and the whole idea for that matter)
was partially borrowed from origoinal Lawrie Abbott's jacko project,
which was taken from wxWindow/Python into the Qt/C++ arena.

Since 2003-08-06, qjackctl has been included in the awesome Planet CCRMA
(http://ccrma-www.stanford.edu/planetccrma/software/) software collection.
Thanks a lot Fernando!

Here are some people who helped this project in one way or another,
and in fair and strict alphabetic order:

    Alexandre Prokoudine             Kasper Souren
    Andreas Persson                  Kjetil Matheussen
    Arnout Engelen                   Ken Ellinwood
    Austin Acton                     Lawrie Abbott
    Ben Powers                       Lee Revell
    Chris Cannam                     Lucas Brasilino
    Dan Nigrin                       Marc-Olivier Barre
    Dave Moore                       Mark Knecht
    Dave Phillips                    Matthias Nagorni
    Dirk Jagdmann                    Melanie
    Dominic Sacre                    Nedko Arnaudov
    Fernando Pablo Lopez-Lezcano     Orm Finnendahl
    Filipe Tomas                     Paul Davis
    Florian Schmidt                  Robert Jonsson
    Fons Adriaensen                  Robin Gareus
    Geoff Beasley                    Roland Mas
    Jack O'Quin                      Sampo Savolainen
    Jacob Meuser                     Stephane Letz
    Jesse Chappell                   Steve Harris
    Joachim Deguara                  Taybin Rutkin
    John Schneiderman                Wilfried Huss
    Jussi Laako                      Wolfgang Woehl
    Karsten Wiese

A special mention should go to the translators of QjackCtl (see TRANSLATORS).

Thanks to you all.
