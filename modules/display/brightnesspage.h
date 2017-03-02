#ifndef BRIGHTNESSPAGE_H
#define BRIGHTNESSPAGE_H

#include "contentwidget.h"

#include <QVBoxLayout>

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class BrightnessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit BrightnessPage(QWidget *parent = 0);

    void setModel(DisplayModel *model);

signals:
    void requestSetMonitorBrightness(Monitor *mon, const double brightness) const;

private:
    void initUI();

private:
    DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
};

}

}

#endif // BRIGHTNESSPAGE_H
