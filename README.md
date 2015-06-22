JACK Control - JACK Visual Control Interface
-----------------------------------------------------

JACK Control is a simple application to control the JACK sound server
(http://jackaudio.org). It is based on the original qjackctl, but has undergone
huge improvements in the code.

License: GNU General Public License (GPL) Version 2+

Requirements
------------

In contradiction to qjackctl, JACK Control does not use the autotools build
system anymore. Also, it is based on Qt5, not Qt4. You will need Qt5, libjack,
libjackserver and libjacknet to build.

How to build
------------
git clone --recursive https://github.com/cybercatalyst/qjackctl.git

Note the "--recursive"-option which is very important, since it clones the
qtjack pod into its subdirectory.

Open the top level jackcontrol.pro and build.

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
