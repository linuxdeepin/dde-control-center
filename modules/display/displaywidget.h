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
    void onScreenSizeChanged() const;

private:
    DisplayModel *m_model;
    widgets::NextPageWidget *m_resolution;
    widgets::NextPageWidget *m_mySettings;
    widgets::SettingsGroup *m_settingsGrp;
    widgets::SettingsGroup *m_resolutionsGrp;
    QPushButton *m_rotate;
    QPushButton *m_custom;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
