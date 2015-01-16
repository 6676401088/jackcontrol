/****************************************************************************
   Copyright (C) 2003-2010, rncbc aka Rui Nuno Capela. All rights reserved.

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

// Own includes
#include "connectalias.h"

// Qt includes
#include <QStringList>

ConnectAlias::ConnectAlias() {
}

// Default destructor.
ConnectAlias::~ConnectAlias() {
	qDeleteAll(*this);
	clear();
}

ClientAlias *ConnectAlias::findClientName (
    const QString& sClientName ) {
    QListIterator<ClientAlias *> iter(*this);
	while (iter.hasNext()) {
        ClientAlias *pClient = iter.next();
		if (pClient->matchClientName(sClientName))
		    return pClient;
	}

	return NULL;
}

void ConnectAlias::setClientAlias ( const QString& sClientName,
    const QString& sClientAlias ) {
    ClientAlias *pClient = findClientName(sClientName);
	if (pClient == NULL) {
        pClient = new ClientAlias(sClientName);
		append(pClient);
	}
	pClient->setClientAlias(sClientAlias);
}

QString ConnectAlias::clientAlias ( const QString& sClientName )
{
    ClientAlias *pClient = findClientName(sClientName);
	if (pClient == NULL)
		return sClientName;

	return pClient->clientAlias();
}


// Client/port aliasing methods.
void ConnectAlias::setPortAlias ( const QString& sClientName,
	const QString& sPortName, const QString& sPortAlias )
{
    ClientAlias *pClient = findClientName(sClientName);
	if (pClient == NULL) {
        pClient = new ClientAlias(sClientName);
		append(pClient);
	}
	pClient->setPortAlias(sPortName, sPortAlias);
}

QString ConnectAlias::portAlias ( const QString& sClientName,
	const QString& sPortName )
{
    ClientAlias *pClient = findClientName(sClientName);
	if (pClient == NULL)
		return sPortName;

	return pClient->portAlias(sPortName);
}


// Load/save aliases definitions.
void ConnectAlias::loadSettings ( QSettings& settings,
	const QString& sAliasesKey )
{
	clear();

	settings.beginGroup(sAliasesKey);
	QStringListIterator iter(settings.childGroups());
	while (iter.hasNext()) {
		QString sClientKey   = iter.next();
		QString sClientName  = settings.value(sClientKey + "/Name").toString();
		QString sClientAlias = settings.value(sClientKey + "/Alias").toString();
		if (!sClientName.isEmpty() && !sClientAlias.isEmpty()) {
            ClientAlias *pClient =
                new ClientAlias(sClientName, sClientAlias);
			append(pClient);
			settings.beginGroup(sClientKey);
			QStringListIterator it(settings.childGroups());
			while (it.hasNext()) {
				QString sPortKey   = it.next();
				QString sPortName  = settings.value(sPortKey + "/Name").toString();
				QString sPortAlias = settings.value(sPortKey + "/Alias").toString();
				if (!sPortName.isEmpty() && !sPortAlias.isEmpty())
					pClient->setPortAlias(sPortName, sPortAlias);
			}
			settings.endGroup();
		}
	}
	settings.endGroup();
}

void ConnectAlias::saveSettings ( QSettings& settings,
	const QString& sAliasesKey )
{
	qSort(*this);

	settings.beginGroup(sAliasesKey);
	int iClient = 0;
    QListIterator<ClientAlias *> iter(*this);
	while (iter.hasNext()) {
		(iter.next())->saveSettings(settings,
			"Client" + QString::number(++iClient));
	}
	settings.endGroup();
}
