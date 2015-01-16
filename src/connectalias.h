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

// Qt includes
#include <QRegExp>
#include <QMap>
#include <QSettings>

// Own includes
#include "clientalias.h"

// Client list alias map.
class ConnectAlias : public QList<ClientAlias *>
{
public:

	// Constructor.
	ConnectAlias ();
	// Default destructor.
	~ConnectAlias ();

	// Client aliasing methods.
	QString clientAlias (const QString& sClientName);
	void setClientAlias (const QString& sClientName,
		const QString& sClientAlias);

	// Port aliasing methods.
	QString portAlias (const QString& sClientName,
		const QString& sPortName);
	void setPortAlias (const QString& sClientName,
		const QString& sPortName, const QString& sPortAlias);

	// Load/save aliases definitions.
	void loadSettings(QSettings& settings, const QString& sAliasesKey);
	void saveSettings(QSettings& settings, const QString& sAliasesKey);

private:

	// Client item finder.
	ClientAlias *findClientName (const QString& sClientName);
};
