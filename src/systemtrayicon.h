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
#include <QWidget>
#include <QSystemTrayIcon>

class SystemTrayIcon : public QSystemTrayIcon {
	Q_OBJECT

public:
    SystemTrayIcon(QWidget *pParent = 0);
    ~SystemTrayIcon();

	// Background mask methods.
	void setBackground(const QColor& background);
	const QColor& background() const;

    // Set system tray icon overlay.
    void setPixmapOverlay(const QPixmap& pmOverlay);
	const QPixmap& pixmapOverlay() const;

	// System tray icon/pixmaps update method.
	void updatePixmap();

	// Redirect to hide.
	void close();

signals:

	// Clicked signal.
	void clicked();

	// Xrun reset signal.
	void middleClicked();

	// Context menu signal.
	void contextMenuRequested(const QPoint& pos);

protected slots:

	// Handle systeam tray activity.
	void activated(QSystemTrayIcon::ActivationReason);

private:

	// Instance pixmap and background color.
	QIcon   m_icon;
	QPixmap m_pixmap;
	QPixmap m_pixmapOverlay;
	QColor  m_background;
};

