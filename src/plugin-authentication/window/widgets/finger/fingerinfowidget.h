/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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
