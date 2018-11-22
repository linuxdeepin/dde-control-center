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

#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include <DWindowManagerHelper>

#include "widgets/translucentframe.h"

DWIDGET_USE_NAMESPACE

///
/// FrameWidget can auto adjust size to fit parent Frame
///

using namespace dcc::widgets;

namespace dcc {

class ContentWidget;

}

class FrameContentWrapper;
class FrameWidget : public TranslucentFrame
{
    Q_OBJECT

public:
    explicit FrameWidget(FrameContentWrapper *parent = 0);

    dcc::ContentWidget *setContent(dcc::ContentWidget * const c);
    dcc::ContentWidget *content();

    void show();
    void hide();
    void showBack();
    void hideBack();
    void destroy();

    int animationDuration() const;

Q_SIGNALS:
    void contentDetached(dcc::ContentWidget * const w) const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private Q_SLOTS:
    void destroySelf();
    void onCompositeChanged();

private:
#ifndef DISABLE_OPACITY_ANIMATION
    QGraphicsOpacityEffect *m_opacityEffect;
#endif
    QPropertyAnimation *m_slidePosAni;

    dcc::ContentWidget *m_content;

    DWindowManagerHelper *m_wmHelper;
};

#endif // FRAMEWIDGET_H
