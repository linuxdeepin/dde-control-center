/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#ifndef COMBOXWIDGET_H
#define COMBOXWIDGET_H

#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QStringList;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class ComboxWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit ComboxWidget(QFrame *parent = nullptr);
    explicit ComboxWidget(const QString &title, QFrame *parent = nullptr);
    explicit ComboxWidget(QWidget *widget, QFrame *parent = nullptr);

    void setComboxOption(const QStringList &options);
    void setCurrentText(const QString &curText);
    void setCurrentIndex(const int index);
    void setTitle(const QString &title);

    QComboBox *comboBox();

Q_SIGNALS:
    void onIndexChanged(int index);
    void onSelectChanged(const QString &selected);
    void dataChanged(const QVariant &data);
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *m_leftWidget;
    QComboBox *m_switchComboBox;
    QLabel *m_titleLabel;
    QString m_str;
};

}
}

#endif // COMBOXWIDGET_H
