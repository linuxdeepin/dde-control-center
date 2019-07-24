/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#ifndef DISPLAYWIDGET_H_V20
#define DISPLAYWIDGET_H_V20

#include "modules/display/iconbutton.h"
#include "modules/modulewidget.h"
#include "../../namespace.h"

#include <dimagebutton.h>

class QListView;
class QStandardItemModel;

namespace dcc {

namespace display {
class DisplayModel;
}

}

namespace DCC_NAMESPACE {

namespace display {

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = 0);

public:
    void setModel(dcc::display::DisplayModel *model);
private Q_SLOTS:

    void onMonitorListChanged();
    void onMenuClicked(const QModelIndex &);

public:
    static int convertToSlider(const float value);
    static float convertToScale(const int value);

Q_SIGNALS: Q_SIGNALS:
    void showMultiScreenPage() const;
    void showResolutionPage() const;
    void showBrightnessPage() const;
    void showScalingPage() const;
    void showCustomConfigPage() const;
    void requestRotate() const;
private:
    void initMenuUI();

private:
    struct MenuMethod {
        QString menuText;
        QMetaMethod method;
    };
private:
    dcc::display::DisplayModel *m_model{nullptr};
    DTK_WIDGET_NAMESPACE::DImageButton *m_rotate{nullptr};
    QVBoxLayout *m_centralLayout{nullptr};
    QListView *m_menuList{nullptr};

    QStandardItemModel *m_multiModel{nullptr};
    QStandardItemModel *m_singleModel{nullptr};

    QList<MenuMethod> m_multMenuList;
    QList<MenuMethod> m_singleMenuList;

    bool m_isMultiScreen{true};
};

}  // namespace display

}  // namespace dcc

#endif  // DISPLAYWIDGET_H_V20
