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

#include "ui_MessagesStatusWidget.h"

// Forward declarations.
class QTreeWidgetItem;
class QFile;

class MessagesStatusWidget : public QWidget
{
	Q_OBJECT

public:
    MessagesStatusWidget(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
    ~MessagesStatusWidget();

	enum TabPage { MessagesTab = 0, StatusTab = 1 };

	void setTabPage(int iTabPage);
	int tabPage() const;

	QFont messagesFont() const;
	void setMessagesFont(const QFont& font);

	int messagesLimit() const;
	void setMessagesLimit(int iLimit);

	bool isLogging() const;
	void setLogging(bool bEnabled, const QString& sFilename = QString());

	void appendMessages(const QString& s);
	void appendMessagesColor(const QString& s, const QString& c);
	void appendMessagesText(const QString& s);

	void updateStatusItem(int iStatusItem, const QString& sText);

public slots:

	void resetXrunStats();
	void refreshXrunStats();

protected:

	void appendMessagesLine(const QString& s);
	void appendMessagesLog(const QString& s);

	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void closeEvent(QCloseEvent *);

	void keyPressEvent(QKeyEvent *);

private:
    Ui::MessagesStatusWidget m_ui;

	// Instance variables.
	int m_iMessagesLines;
	int m_iMessagesLimit;
	int m_iMessagesHigh;

	// Logging stuff.
	QFile *m_pMessagesLog;

	// Status stuff.
	QTreeWidgetItem *m_apStatus[19];
};

