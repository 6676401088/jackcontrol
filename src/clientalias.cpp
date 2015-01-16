/****************************************************************************
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

// Own includes
#include "clientalias.h"

ClientAlias::ClientAlias(const QString& sClientName, const QString& sClientAlias) {
    if (sClientAlias.isEmpty()) {
        m_rxClientName.setPattern(escapeRegExpDigits(sClientName));
        m_sClientAlias = sClientName;
    } else {
        m_rxClientName.setPattern(sClientName);
        m_sClientAlias = sClientAlias;
    }
}

ClientAlias::~ClientAlias() {
    m_ports.clear();
}

QString ClientAlias::clientName() const {
    return m_rxClientName.pattern();
}

bool ClientAlias::matchClientName(const QString& sClientName) {
    return m_rxClientName.exactMatch(sClientName);
}

const QString& ClientAlias::clientAlias() const {
    return m_sClientAlias;
}

void ClientAlias::setClientAlias(const QString& sClientAlias ) {
    m_sClientAlias = sClientAlias;
}


// Port aliasing methods.
QString ClientAlias::portAlias ( const QString& sPortName ) const
{
    QString sPortAlias = m_ports[sPortName];
    if (sPortAlias.isEmpty())
        sPortAlias = sPortName;
    return sPortAlias;
}

void ClientAlias::setPortAlias ( const QString& sPortName,
    const QString& sPortAlias )
{
    m_ports[sPortName] = sPortAlias;
}


// Save client/port aliases definitions.
void ClientAlias::saveSettings ( QSettings& settings,
    const QString& sClientKey )
{
    settings.beginGroup(sClientKey);
    settings.setValue("/Name", m_rxClientName.pattern());
    settings.setValue("/Alias", m_sClientAlias);
    int iPort = 0;
    QMap<QString, QString>::ConstIterator iter = m_ports.begin();
    while (iter != m_ports.end()) {
        settings.beginGroup("/Port" + QString::number(++iPort));
        settings.setValue("/Name", iter.key());
        settings.setValue("/Alias", iter.value());
        settings.endGroup();
        ++iter;
    }
    settings.endGroup();
}


// Escape and format a string as a regular expresion.
QString ClientAlias::escapeRegExpDigits ( const QString& s,
    int iThreshold )
{
    QString sDigits;
    QString sResult;
    QString sEscape = QRegExp::escape(s);
    int iDigits = 0;

    for (int i = 0; i < sEscape.length(); i++) {
        const QChar& ch = sEscape.at(i);
        if (ch.isDigit()) {
            if (iDigits < iThreshold)
                sDigits += ch;
            else
                sDigits = "[0-9]+";
            iDigits++;
        } else {
            if (iDigits > 0) {
                sResult += sDigits;
                sDigits = QString::null;
                iDigits = 0;
            }
            sResult += ch;
        }
    }

    if (iDigits > 0)
        sResult += sDigits;

    return sResult;
}


// Need for generid sort.
bool ClientAlias::operator< ( const ClientAlias& other )
{
    return (m_sClientAlias < other.clientAlias());
}
