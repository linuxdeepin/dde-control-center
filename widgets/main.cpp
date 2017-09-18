/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<QApplication>

#include "settingsgroup.h"
#include "settingsitem.h"

#include <QTimer>

using namespace dcc;

int main(int argc, char** argv)
{
	QApplication a(argc, argv);

    QFrame f;
    f.setFixedSize(400, 600);

	SettingsGroup sg(&f);
    sg.setFixedWidth(200);
    sg.move(100, 100);

    SettingsItem * item1 = new SettingsItem;
    item1->setFixedHeight(100);

    SettingsItem * item2 = new SettingsItem;
    item2->setFixedHeight(200);

    sg.appendItem(item1);
    sg.appendItem(item2);

    QTimer::singleShot(1000, [item1] { item1->setFixedHeight(item1->height() + 50 ); });

	f.show();

	return a.exec();
}

