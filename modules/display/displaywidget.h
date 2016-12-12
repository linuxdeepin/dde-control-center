#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "modulewidget.h"

#include <QPushButton>

namespace dcc {

namespace widgets {
class SettingsGroup;
class NextPageWidget;
}

namespace display {

class DisplayModel;
class DisplayWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit DisplayWidget();

    void setModel(DisplayModel *model);

signals:
    void showResolutionPage() const;
    void requestRotate() const;
    void requestCustom() const;

private slots:
    void onScreenListChanged() const;

private:
    DisplayModel *m_model;
    dcc::widgets::NextPageWidget *m_resolution;
    QPushButton *m_rotate;
    QPushButton *m_custom;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
