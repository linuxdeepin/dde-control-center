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
#ifndef DCC_DISABLE_ROTATE
    void requestRotate() const;
#endif
    void requestNewConfig() const;
    void requestConfigPage(const QString &config) const;
    void requestSwitchConfig(const QString &config) const;
    void requestModifyConfig(const QString &config) const;
    void requestModifyConfigName(const QString &oldName, const QString &newName) const;
#ifndef DCC_DISABLE_MIRACAST
    void requestMiracastConfigPage() const;
#endif

private slots:
    void onScreenListChanged() const;
    void onScreenSizeChanged() const;
    void onConfigListChanged();
    void onFirstConfigCreated(const QString &config);

private:
    DisplayModel *m_model;
    widgets::NextPageWidget *m_resolution;
    widgets::NextPageWidget *m_brightnessSettings;
#ifndef DCC_DISABLE_MIRACAST
    widgets::NextPageWidget *m_miracast;
#endif
    QList<widgets::NextPageWidget *> m_customSettings;
    widgets::SettingsGroup *m_customSettingsGrp;
    widgets::SettingsGroup *m_resolutionsGrp;
#ifndef DCC_DISABLE_MIRACAST
    widgets::SettingsGroup *m_miracastGrp;
#endif
#ifndef DCC_DISABLE_ROTATE
    QPushButton *m_rotate;
#endif
    QPushButton *m_createConfig;

    QTimer *m_configListRefershTimer;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
