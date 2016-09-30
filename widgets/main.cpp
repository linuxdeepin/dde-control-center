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

