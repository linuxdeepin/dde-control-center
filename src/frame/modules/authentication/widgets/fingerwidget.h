// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FINGERWIDGET_H
#define FINGERWIDGET_H

#include "widgets/titlelabel.h"

#include <dpicturesequenceview.h>
#include <QWidget>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace authentication {
class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    enum enrollStage {
      enrollFirstStage = 1,
      enrollSecondStage
    };

    explicit FingerWidget(QWidget *parent = nullptr);

    void setStatueMsg(const QString &title, const QString &msg, bool reset = true);

    void setProsses(int pro);
    void reEnter();
    void finished();
    void stopLiftTimer();

Q_SIGNALS:
    void playEnd();

private:
    DPictureSequenceView *m_view;
    QLabel *m_tipLbl;
    TitleLabel *m_titleLbl;
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
}
}

#endif // FINGERWIDGET_H
