//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DOUTESTWIDGET_H
#define DOUTESTWIDGET_H

#include "widgets/settingsitem.h"
#include <QStringList>
#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE
class QMouseEvent;
class QVBoxLayout;

namespace DCC_NAMESPACE
{
class SettingsItem;
class DouTestWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit DouTestWidget(QWidget *parent = nullptr);

    enum State {
        BOW,RAISE
    };

    struct DoubleTestPic {
        QStringList double_1;
        QStringList double_2;
        QStringList click_1;
        QStringList click_2;
    };

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QVBoxLayout           *m_mainlayout;
    DPictureSequenceView  *m_testWidget;
    State                  m_state;
    DoubleTestPic          m_doubleTest;
};
}


#endif // DOUTESTWIDGET_H
