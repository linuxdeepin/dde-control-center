//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "widgets/titlelabel.h"

#include <dpicturesequenceview.h>
#include <QWidget>
#include <QLabel>

class FingerInfoWidget : public QWidget
{
    Q_OBJECT
public:
    enum enrollStage {
      enrollFirstStage = 1,
      enrollSecondStage
    };

    explicit FingerInfoWidget(QWidget *parent = nullptr);

    void setStatueMsg(const QString &title, const QString &msg, bool reset = true);

    void setProsses(int pro);
    void reEnter();
    void finished();
    void stopLiftTimer();

Q_SIGNALS:
    void playEnd();

private:
    DTK_WIDGET_NAMESPACE::DPictureSequenceView *m_view;
    QLabel *m_tipLbl;
    DCC_NAMESPACE::TitleLabel *m_titleLbl;
    bool m_isFinished;

    QString m_defTip{""};
    QString m_defTitle{""};
    int m_pro{0};
    QTimer *m_titleTimer;
    QTimer *m_msgTimer;
    QTimer *m_liftTimer;
    QString m_theme;
    bool m_reset{false};
    bool m_isStageOne{true};
};
