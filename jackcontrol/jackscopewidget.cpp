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

// Qt includes
#include <QPainter>

// Own includes
#include "jackscopewidget.h"
#include "jackcontrol.h"

// UIC includes
#include "ui_jackscopewidget.h"

JackScopeWidget::JackScopeWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::JackScopeWidget) {
    _ui->setupUi(this);

    _ui->labelTimeDomain->setStyleSheet("");
    _ui->labelFrequencyDomain->setStyleSheet("");
}

JackScopeWidget::~JackScopeWidget() {
    delete _ui;
}

void JackScopeWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);

    painter.fillRect(_ui->labelTimeDomain->rect(), Qt::black);
    painter.fillRect(_ui->labelFrequencyDomain->rect(), Qt::black);

    //painter.drawLine(_ui->labelTimeDomain->pos(), _ui->labelTimeDomain->pos() + QPoint(100, 100));
}
