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

#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QPointer>

#include <dimagebutton.h>
#include <darrowrectangle.h>

DWIDGET_USE_NAMESPACE

class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationBar(QWidget *parent = nullptr);

    QString currentModuleName() const;

public Q_SLOTS:
    void setModuleVisible(const QString &module, bool visible);
    void setModuleChecked(const QString &module);

Q_SIGNALS:
    void requestModule(const QString &name) const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onNavigationButtonClicked();
    void setModuleChecked(DImageButton *button);
    QString transModuleName(const QString &moduleName) const;

private:
    QMap<QString, DImageButton *> m_navigationButtons;
    QPointer<DImageButton> m_checkedButton;
    DArrowRectangle *m_arrowRectangle;
    QLabel *m_navLabel;
};

#endif // NAVIGATIONBAR_H
