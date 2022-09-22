// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DSSTESTWIDGET_H
#define DSSTESTWIDGET_H

#include <QWidget>

namespace dss {
  namespace module {
    class NetworkPlugin;
  }
}
namespace Dtk {
  namespace Widget {
    class DFloatingButton;
  }
}

class QPushButton;

class DssTestWidget : public QWidget
{
    Q_OBJECT

public:
    DssTestWidget(QWidget *parent = Q_NULLPTR);
    ~DssTestWidget();

private:
    void loadDssPlugin();
    bool eventFilter(QObject *watched, QEvent *event);

private:
    dss::module::NetworkPlugin *m_pModule;
    Dtk::Widget::DFloatingButton *m_button;
};

#endif // DSSTESTWIDGET_H
