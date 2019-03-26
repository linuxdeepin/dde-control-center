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
#include "scalingpage.h"

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
    void showScalingPage() const;
#ifndef DCC_DISABLE_ROTATE
    void requestRotate() const;
#endif
    void requestNewConfig(const QString &config) const;
    void requestDeleteConfig(const QString &config) const;
    void requestSwitchConfig(const QString &config) const;
    void requestModifyConfig() const;
    void requestModifyConfigName(const QString &oldName, const QString &newName) const;
    void requestRecordCurrentState() const;
#ifndef DCC_DISABLE_MIRACAST
    void requestMiracastConfigPage(const QDBusObjectPath &path) const;
#endif
    void requestUiScaleChanged(const double value) const;
    void requestDuplicateMode() const;
    void requestExtendMode() const;
    void requestOnlyMonitor(const QString &monName) const;

public:
    static int convertToSlider(const float value);
    static float convertToScale(const int value);

private Q_SLOTS:
    void onMonitorListChanged() const;
    void onScreenSizeChanged() const;
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
    widgets::SettingsGroup *         m_resolutionsGrp;
    widgets::TitledSliderItem *      m_scaleWidget;
    widgets::NextPageWidget *        m_scalingSettings;
#ifndef DCC_DISABLE_MIRACAST
    widgets::SettingsGroup *m_miracastGrp;
#endif
#ifndef DCC_DISABLE_ROTATE
    QPushButton *m_rotate;
#endif
    QPushButton *m_customConfigButton;

#ifndef DCC_DISABLE_MIRACAST
    QMap<QDBusObjectPath, NextPageWidget *> m_miracastList;
#endif
    void setIndividualScalingEnabled(bool enabled) const;
};

}  // namespace display

}  // namespace dcc

#endif  // DISPLAYWIDGET_H
