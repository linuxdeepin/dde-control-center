#include <gtest/gtest.h>
#include <gtest/gtest.h>

#include <QDebug>
#include <QtWidgets/QApplication>

#ifdef SANITIZER_CHECK
#include <sanitizer/asan_interface.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    qDebug() << "run...result:" << ret;

#ifdef SANITIZER_CHECK
    __sanitizer_set_report_path("asan.log");
#endif

    return ret;
}
