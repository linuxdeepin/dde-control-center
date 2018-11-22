/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QPushButton>
#include "miracastmodel.h"
#include "modules/modulewidget.h"

namespace dcc {

namespace widgets {
class SettingsGroup;
class NextPageWidget;
class TitledSliderItem;
}

namespace display {

class DisplayModel;
class DisplayControlPage;
class DisplayWidget : public ModuleWidget {
    Q_OBJECT

public:
    explicit DisplayWidget();

    void setModel(DisplayModel *model);
#ifndef DCC_DISABLE_MIRACAST
    void setMiracastModel(MiracastModel *miracastModel);
#endif

Q_SIGNALS:
    void showResolutionPage() const;
    void showBrightnessPage() const;
#ifndef DCC_DISABLE_ROTATE
    void requestRotate() const;
#endif
    void requestNewConfig() const;
    void requestConfigPage(const QString &config) const;
    void requestSwitchConfig(const QString &config) const;
    void requestModifyConfig(const QString &config, bool isNewConfig) const;
    void requestModifyConfigName(const QString &oldName, const QString &newName) const;
#ifndef DCC_DISABLE_MIRACAST
    void requestMiracastConfigPage(const QDBusObjectPath &path) const;
#endif
    void requestUiScaleChanged(const double value) const;
    void requestDuplicateMode() const;
    void requestExtendMode() const;
    void requestOnlyMonitor(const QString &monName) const;

private:
    int converToSlider(const float value);
    float converToScale(const int value);

private Q_SLOTS:
    void onScreenListChanged() const;
    void onScreenSizeChanged() const;
    void onConfigListChanged();
    void onFirstConfigCreated(const QString &config);
#ifndef DCC_DISABLE_MIRACAST
    void onMiracastLinkAdded(const LinkInfo &link);
    void onMiracastLinkRemoved(const QDBusObjectPath &path);
#endif
    void onUiScaleChanged(const double scale);

private:
    DisplayModel *                   m_model;
    widgets::SettingsGroup *         m_displayControlPageGrp;
    DisplayControlPage *             m_displayControlPage;
    widgets::NextPageWidget *        m_resolution;
    widgets::NextPageWidget *        m_brightnessSettings;
    QList<widgets::NextPageWidget *> m_customSettings;
    widgets::SettingsGroup *         m_customSettingsGrp;
    widgets::SettingsGroup *         m_resolutionsGrp;
    widgets::TitledSliderItem *      m_scaleWidget;
#ifndef DCC_DISABLE_MIRACAST
    widgets::SettingsGroup *m_miracastGrp;
#endif
#ifndef DCC_DISABLE_ROTATE
    QPushButton *m_rotate;
#endif
    QPushButton *m_createConfig;

    QTimer *m_configListRefershTimer;
#ifndef DCC_DISABLE_MIRACAST
    QMap<QDBusObjectPath, NextPageWidget *> m_miracastList;
#endif
};

}  // namespace display

}  // namespace dcc

#endif  // DISPLAYWIDGET_H
