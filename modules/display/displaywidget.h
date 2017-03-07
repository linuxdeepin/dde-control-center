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
    void showBrightnessPage() const;
    void requestRotate() const;
    void requestNewConfig() const;
    void requestConfigPage(const QString &config) const;
    void requestSwitchConfig(const QString &config) const;

private slots:
    void onScreenListChanged() const;
    void onScreenSizeChanged() const;
    void onConfigListChanged();

private:
    DisplayModel *m_model;
    widgets::NextPageWidget *m_resolution;
    widgets::NextPageWidget *m_brightnessSettings;
    QList<widgets::NextPageWidget *> m_customSettings;
    widgets::SettingsGroup *m_customSettingsGrp;
    widgets::SettingsGroup *m_resolutionsGrp;
    QPushButton *m_rotate;
    QPushButton *m_createConfig;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
