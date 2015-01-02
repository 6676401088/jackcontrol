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

#include "ui_SetupDialog.h"

// Forward declarations.
class Setup;
class QButtonGroup;

class SetupDialog : public QDialog
{
	Q_OBJECT
public:
    SetupDialog(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
    ~SetupDialog();

    void setup(Setup * pSetup);

public slots:

	void changeCurrentPreset(const QString&);
	void saveCurrentPreset();
	void deleteCurrentPreset();

	void changeAudio(int);
	void changeDriver(const QString&);

	void symbolStartupScript();
	void symbolPostStartupScript();
	void symbolShutdownScript();
	void symbolPostShutdownScript();

	void browseStartupScript();
	void browsePostStartupScript();
	void browseShutdownScript();
	void browsePostShutdownScript();
	void browseActivePatchbayPath();
	void browseMessagesLogPath();

	void chooseDisplayFont1();
	void chooseDisplayFont2();
	void toggleDisplayEffect(bool);
	void chooseMessagesFont();
	void chooseConnectionsFont();

	void settingsChanged();
	void optionsChanged();

protected slots:

	void accept();
	void reject();

protected:

	// A combo-box text item setter helper.
	void setComboBoxCurrentText (
		QComboBox *pComboBox, const QString& sText ) const;

	void changePreset(const QString& sPreset);
	bool savePreset(const QString& sPreset);
	bool deletePreset(const QString& sPreset);
	void resetPresets();

	void computeLatency();

	void changeDriverAudio( const QString& sDriver, int iAudio );
	void changeDriverUpdate( const QString& sDriver, bool bUpdate );

	void symbolMenu(QLineEdit * pLineEdit, QToolButton * pToolButton );

	void stabilizeForm();

private:
    Ui::SetupDialog m_ui;

    Setup *m_pSetup;
	QButtonGroup *m_pTimeDisplayButtonGroup;

	int m_iDirtySetup;
	int m_iDirtySettings;
	int m_iDirtyOptions;

	QString m_sPreset;
};
