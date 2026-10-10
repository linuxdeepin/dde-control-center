// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDITEM_H
#define SOUNDITEM_H

#include "soundapplet.h"
#include "soundcontroller.h"
#include "commoniconbutton.h"

#include "audio1sinkinterface.h"

#include <QWidget>
#include <QIcon>

#define SOUND_KEY "sound-item-key"

using DBusSink = org::deepin::dde::audio1::Sink;

namespace Dock {
class TipsWidget;
}
class SoundView : public QWidget
{
    Q_OBJECT

public:
    explicit SoundView(QWidget *parent = 0);

    QWidget *tipsWidget();
    QWidget *popupApplet();
    QWidget *dockIcon() const { return m_iconWidget; }

    const QString contextMenu();
    void invokeMenuItem(const QString menuId, const bool checked);
    void refreshIcon();
    void refreshTips(const bool force = false);
    void setAppletMinHeight(int minHeight);

signals:
    void requestContextMenu() const;
    void requestHideApplet();

protected:
    void resizeEvent(QResizeEvent *e) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void refresh();

private:
    Dock::TipsWidget *m_tipsLabel;
    QScopedPointer<SoundApplet> m_applet;
    CommonIconButton *m_iconWidget;
};

#endif // SOUNDITEM_H
