/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QComboBox;
QT_END_NAMESPACE

namespace dcc {
namespace personalization {
class PersonalizationModel;
class FontModel;
}
}

namespace dcc {
namespace widgets {
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace personalization {
class PersonalizationFontsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationFontsWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetFontSize(const int value);
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setModel(dcc::personalization::PersonalizationModel *const model);
    void setFontSize(int size);

private Q_SLOTS:
    void onSelectChanged(const QString &name);
    void onDefaultFontChanged(const QString &name, dcc::personalization::FontModel *sender = nullptr);
    void setList(const QList<QJsonObject> &list, dcc::personalization::FontModel *model = nullptr);
    void setCommboxItemFontSize();
private:
    dcc::personalization::PersonalizationModel *m_model;
    QVBoxLayout *m_centralLayout;
    dcc::widgets::TitledSliderItem *m_fontSizeSlider;  //字号调节
    QComboBox *m_standardFontsCbBox;
    QComboBox *m_monoFontsCbBox;
    bool m_isAppend; //true : comboBox is appending data, ignoring currentTextChange signal
};
}
}
