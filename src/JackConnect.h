/****************************************************************************
   Copyright (C) 2003-2010, rncbc aka Rui Nuno Capela. All rights reserved.
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

#pragma once

#include "About.h"
#include "connectionssplitter.h"

#include <jack/jack.h>

// Forward declarations.
class qjackctlJackPort;
class qjackctlJackClient;
class qjackctlJackClientList;
class JackConnectionsModel;

// Connection port type.
#define QJACKCTL_JACK_AUDIO		0
#define QJACKCTL_JACK_MIDI		1

// Pixmap-set array indexes/types.
#define QJACKCTL_JACK_CLIENTI	0	// Input client item pixmap.
#define QJACKCTL_JACK_CLIENTO	1	// Output client item pixmap.
#define QJACKCTL_JACK_PORTPTI	2	// Physcal Terminal Input port pixmap.
#define QJACKCTL_JACK_PORTPTO	3	// Physical Terminal Output port pixmap.
#define QJACKCTL_JACK_PORTPNI	4	// Physical Non-terminal Input port pixmap.
#define QJACKCTL_JACK_PORTPNO	5	// Physical Non-terminal Output port pixmap.
#define QJACKCTL_JACK_PORTLTI	6	// Logical Terminal Input port pixmap.
#define QJACKCTL_JACK_PORTLTO	7	// Logical Terminal Output port pixmap.
#define QJACKCTL_JACK_PORTLNI	8	// Logical Non-terminal Input port pixmap.
#define QJACKCTL_JACK_PORTLNO	9	// Logical Non-terminal Output port pixmap.
#define QJACKCTL_JACK_PIXMAPS	10	// Number of pixmaps in array.

class qjackctlJackPort : public JackPortTreeWidgetItem
{
public:

	// Constructor.
	qjackctlJackPort(qjackctlJackClient *pClient,
		const QString& sPortName, jack_port_t *pJackPort);
	// Default destructor.
	~qjackctlJackPort();

	// Jack handles accessors.
	jack_client_t *jackClient() const;
	jack_port_t   *jackPort() const;

private:

	// Instance variables.
	jack_port_t *m_pJackPort;
};


// Jack client list item.
class qjackctlJackClient : public JackClientTreeWidgetItem
{
public:

	// Constructor.
	qjackctlJackClient(qjackctlJackClientList *pClientList,
		const QString& sClientName);
	// Default destructor.
	~qjackctlJackClient();

	// Jack client accessors.
	jack_client_t *jackClient() const;

	// Jack port lookup.
	qjackctlJackPort *findJackPort(jack_port_t *pJackPort);
};


// Jack client list.
class qjackctlJackClientList : public JackClientList
{
public:

	// Constructor.
    qjackctlJackClientList(ClientListTreeWidget *pListView, bool bReadable);
	// Default destructor.
	~qjackctlJackClientList();

	// Jack port lookup.
	qjackctlJackPort *findJackClientPort(jack_port_t *pJackPort);

	// Client:port refreshner (return newest item count).
	int updateClientPorts();

	// Jack client port aliases mode.
	static void setJackClientPortAlias(int iJackClientPortAlias);
	static int jackClientPortAlias();

private:

	// Jack client port aliases mode.
	static int g_iJackClientPortAlias;
};
