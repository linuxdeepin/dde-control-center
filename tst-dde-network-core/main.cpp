#include <gtest/gtest.h>
#include <gtest/gtest.h>

#include <QDebug>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    qDebug() << "run...result:" << ret;

    return app.exec();
}
