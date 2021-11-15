/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <donghualin@uniontech.com>
*
* Maintainer: Zhang Qipeng <donghualin@uniontech.com>
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

private:
    dss::module::NetworkPlugin *m_pModule;
    Dtk::Widget::DFloatingButton *m_button;
};

#endif // DSSTESTWIDGET_H
