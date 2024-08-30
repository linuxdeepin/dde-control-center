//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <QProgressBar>
#include <QLabel>

class LoadingItem : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingItem(QFrame *parent = 0);
    void setProgressValue(int value);
    void setProgressBarVisible(bool visible);
    void setMessage(const QString &message);
    void setVersionVisible(bool state);
    void setSystemVersion(const QString &version);
    void setImageVisible(bool state);
    void setImageOrTextVisible(bool state);
    void setImageAndTextVisible(bool state);

private:
    QLabel *m_messageLabel;
    QProgressBar *m_progress;
    QLabel *m_labelImage;
    QLabel *m_labelText;
};
