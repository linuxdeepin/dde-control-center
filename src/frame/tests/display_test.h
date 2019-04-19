#ifndef DISPLAY_TEST_H
#define DISPLAY_TEST_H

#include <QMap>
#include <QString>
#include <QTest>

#include "modules/display/displaywidget.h"

class DisplayTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testSliderValue_data()
    {
        QTest::addColumn<float>("value");
        QTest::addColumn<int>("result");

        QMap<float, int> testMap{ { 1.0, 1 },  { 1.25, 2 }, { 1.5, 3 },
                                  { 1.75, 4 }, { 2.0, 5 },  { 2.25, 6 },
                                  { 2.5, 7 },  { 2.75, 8 }, { 3.0, 9 } };

        for (auto it = testMap.constBegin(); it != testMap.constEnd(); ++it) {
            QTest::newRow("converToSlider") << it.key() << it.value();
        }
    }
    void testSliderValue()
    {
        QFETCH(float, value);
        QFETCH(int, result);

        using namespace dcc::display;

        QCOMPARE(DisplayWidget::convertToSlider(value), result);
        QCOMPARE(DisplayWidget::convertToScale(result), value);
    }
};

QTEST_MAIN(DisplayTest)
#endif  // !DISPLAY_TEST_H
