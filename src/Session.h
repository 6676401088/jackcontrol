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

#pragma once

// Qt includes
#include <QApplication>
#include <QString>
#include <QHash>

class QSettings;

// JACK includes
#include <jack/jack.h>

class Session
{
public:
    Session();
    ~Session();

	struct ConnectItem
	{
		ConnectItem() : connected(false) {}

		QString client_name;
		QString port_name;
		bool    connected;
	};

	typedef QList<ConnectItem *> ConnectList;

	struct PortItem
	{
		PortItem() : connected(0) {}
		~PortItem() { qDeleteAll(connects); }

		QString     port_name;
		int         port_type;
		int         connected;
		ConnectList connects;
	};

	typedef QList<PortItem *> PortList;

	struct ClientItem
	{
		ClientItem() : connected(0) {}
		~ClientItem() { qDeleteAll(ports); }

		QString  client_name;
		QString  client_uuid;
		QString  client_command;
		int      connected;
		PortList ports;
	};

	typedef QHash<QString, ClientItem *> ClientList;

	// Client list accessor (read-only)
	const ClientList& clients() const;

	// House-keeper.
	void clear();

	// Critical methods.
	bool save(const QString& sSessionDir, int iSessionType = 0);
	bool load(const QString& sSessionDir);

	// Update (re)connections utility method.
	bool update();

	// Infra-client table.
	struct InfraClientItem
	{
		QString  client_name;
		QString  client_command;
	};

	typedef QHash<QString, InfraClientItem *> InfraClientList;

	// Infra-client list accessor (read-write)
	InfraClientList& infra_clients();

	// Load/save all infra-clients from/to configuration file.
	void loadInfraClients(QSettings& settings);
	void saveInfraClients(QSettings& settings);

	// Clear infra-client table.
	void clearInfraClients();

	// Check whether a given JACK client name exists...
	bool isJackClient(const QString& sClientName) const;

protected:

	// File methods.
	bool loadFile(const QString& sFilename);
	bool saveFile(const QString& sFilename);

private:

	// Instance variables.
	ClientList m_clients;	

	// Infra-clients table.
	InfraClientList m_infra_clients;
};