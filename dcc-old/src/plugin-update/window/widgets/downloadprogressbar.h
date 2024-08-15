//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>
#include "common.h"

class DownloadProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressBar(QWidget *parent = nullptr);

    void setMessage(const QString &message);
    void setProcessValue(const int progress);

    inline int value() const { return m_currentValue; }
    inline int minimum() const { return 0; }
    inline int maximum() const { return 100; }

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void clicked();

private:
    int m_currentValue;
    QString m_message;
};

#endif // DOWNLOADPROGRESSBAR_H
