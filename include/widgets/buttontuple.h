// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BUTTONTUPLE_H
#define BUTTONTUPLE_H

#include <DSuggestButton>
#include <DPushButton>
#include <DWarningButton>

#include <QWidget>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class LeftButton : public DPushButton
{
    Q_OBJECT
public:
    LeftButton() {}
    virtual ~LeftButton() {}
};

class RightButton : public DSuggestButton
{
    Q_OBJECT
public:
    RightButton() {}
    virtual ~RightButton() {}
};


class ButtonTuple : public QWidget
{
    Q_OBJECT
public:
    enum ButtonType {
        Normal = 0,
        Save = 1,
        Delete = 2,
    };

    explicit ButtonTuple(ButtonType type = Normal, QWidget *parent = nullptr);

    QPushButton *leftButton();
    QPushButton *rightButton();

    void removeSpacing();

private:
    void createRightButton(const ButtonType type);

Q_SIGNALS:
    void leftButtonClicked();
    void rightButtonClicked();

private:
    QPushButton *m_leftButton;
    QPushButton *m_rightButton;
};

} // namespace widgets
} // namespace dcc

#endif // BUTTONTUPLE_H
