// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef JUMPSETTINGBUTTON_H
#define JUMPSETTINGBUTTON_H

#include "commoniconbutton.h"

#include <QFrame>
#include <QLabel>

#include <DLabel>

class JumpSettingButton : public QFrame
{
    Q_OBJECT
public:
    explicit JumpSettingButton(QWidget *parent = nullptr);
    JumpSettingButton(const QIcon& icon, const QString& description, QWidget *parent = nullptr);
    ~JumpSettingButton();

    void setDccPage(const QString &module, const QString &page);
    void setAutoShowPage(bool autoShowPage) { m_autoShowPage = autoShowPage; }
    void setIcon(const QIcon &icon);
    void setDescription(const QString& description);

signals:
    void clicked();
    void showPageRequestWasSended();

protected:
    bool event(QEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initUI();

private:
    bool m_hover;
    bool m_autoShowPage;
    QString m_fistPage;
    QString m_secondPage;
    CommonIconButton *m_iconButton;
    Dtk::Widget::DLabel *m_descriptionLabel;
};

#endif
