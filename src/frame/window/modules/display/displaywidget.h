/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "interface/namespace.h"

#include <DFloatingButton>
#include <DListView>

#include <QModelIndex>
#include <QPoint>
#include <QGSettings>

QT_BEGIN_NAMESPACE
class QListView;
class QStandardItemModel;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace display {
class DisplayModel;
}

namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {

namespace display {

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(dcc::display::DisplayModel *model, QWidget *parent = nullptr);

public:
    void setModel();
    int showPath(const QString &path);
    QPoint getRotateBtnPos();
    inline bool isMultiMode() const { return m_isMultiScreen; }
    inline bool isShowMultiscreen() const { return m_isShowMultiscreen; }
    int getMenuIndex(QString str, bool isSingle = true);
    void initMenuUI();

public Q_SLOTS:
    void onMonitorListChanged();

private Q_SLOTS:
    void onMenuClicked(const QModelIndex &);

public:
    static int convertToSlider(const double value);
    static double convertToScale(const int value);

Q_SIGNALS:
    void requestShowMultiScreenPage() const;
    void requestShowResolutionPage() const;
    void requestShowBrightnessPage() const;
    void requestShowScalingPage() const;
    void requestShowCustomConfigPage() const;
    void requestShowTouchscreenPage() const;
    void requestRotate() const;
    void requestShowRefreshRatePage() const;
    void requestShowMultiRefreshRatePage() const;
    void requestShowMultiResolutionPage() const;

private:
    struct MenuMethod {
        QString menuText;
        QString iconName;
        QMetaMethod method;
    };

private:
    dcc::display::DisplayModel *m_model{nullptr};
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_rotate{nullptr};
    QVBoxLayout *m_centralLayout{nullptr};
    dcc::widgets::MultiSelectListView *m_menuList{nullptr};
    QModelIndex m_currentIdx;

    QStandardItemModel *m_multiModel{nullptr};
    QStandardItemModel *m_singleModel{nullptr};

    QList<MenuMethod> m_multMenuList;
    QList<MenuMethod> m_singleMenuList;

    bool m_isMultiScreen{true};
    QGSettings *m_displaySetting{nullptr};
    bool m_isShowMultiscreen;
};

}  // namespace display

}  // namespace dcc

#endif  // DISPLAYWIDGET_H_V20
