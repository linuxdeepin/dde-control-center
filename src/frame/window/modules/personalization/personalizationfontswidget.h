// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
class SettingsItem;
}
}

namespace DCC_NAMESPACE {
namespace personalization {
class PersonalizationFontsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationFontsWidget(QWidget *parent = nullptr);
    virtual ~PersonalizationFontsWidget();

Q_SIGNALS:
    void requestSetFontSize(const int value);
    void requestSetDefault(const QJsonObject &value);
    void notifyFontSizeChanged(int);

public Q_SLOTS:
    void setModel(dcc::personalization::PersonalizationModel *const model);
    void setFontSize(int size);

private Q_SLOTS:
    void onSelectChanged(const QString &name);
    void onDefaultFontChanged(const QString &name, dcc::personalization::FontModel *sender = nullptr);
    void setList(const QList<QJsonObject> &list, dcc::personalization::FontModel *model = nullptr);
    void setCommboxItemFontSize(int fontSize);

private:
    void updateFontSizeSlide();

private:
    dcc::personalization::PersonalizationModel *m_model;
    QVBoxLayout *m_centralLayout;
    dcc::widgets::TitledSliderItem *m_fontSizeSlider;  //字号调节
    QComboBox *m_standardFontsCbBox;
    QComboBox *m_monoFontsCbBox;
    bool m_isAppend; //true : comboBox is appending data, ignoring currentTextChange signal
    dcc::widgets::SettingsItem *m_sfontitem;
    dcc::widgets::SettingsItem *m_mfontitem;
    int m_fontSize;
};
}
}
