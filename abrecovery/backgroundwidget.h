// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <DSpinner>

#include <QWidget>

class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundWidget(bool bRestoring = false, QWidget *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:

protected:
    void paintEvent(QPaintEvent * event) override;

private:
    DTK_WIDGET_NAMESPACE::DSpinner *m_loadingIndicator;
    bool m_bIsRestoring;
};

#endif // BACKGROUNDWIDGET_H
