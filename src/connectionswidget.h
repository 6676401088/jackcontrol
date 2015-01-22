/****************************************************************************
   Copyright (C) 2003-2011, rncbc aka Rui Nuno Capela. All rights reserved.
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

// uic includes
#include "ui_connectionswidget.h"

class ConnectionsWidget : public QWidget {
	Q_OBJECT
public:
    ConnectionsWidget(QWidget *parent = 0);
    ~ConnectionsWidget();

public slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();

    void on_pushButtonDisconnectAll_clicked();

    void on_pushButtonCollapseAll_clicked();
    void on_pushButtonExpandAll_clicked();

private:
    Ui::ConnectionsWidget ui;
};
