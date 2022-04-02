/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include <DBlurEffectWidget>
#include <DSuggestButton>

#include <QDialog>
#include <QMap>
#include <QCompleter>
#include "timezone.h"

DWIDGET_USE_NAMESPACE

class QPushButton;
class QComboBox;
class QLabel;
class QAbstractItemView;

class TimezoneMap;
class SearchInput;

DWIDGET_BEGIN_NAMESPACE
class DSearchEdit;
DWIDGET_END_NAMESPACE

class TimeZoneChooser : public QDialog
{
    Q_OBJECT
public:
    explicit TimeZoneChooser(QWidget *parent = nullptr);
    void setIsAddZone(const bool isAdd);
    inline bool isAddZone() { return m_isAddZone; }
    void setCurrentTimeZoneText(const QString &zone);

Q_SIGNALS:
    void confirmed(const QString &zone);
    void cancelled();

public Q_SLOTS:
    void setMarkedTimeZone(const QString &timezone);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QSize getFitSize() const;
    int getFontSize() const;
    void setupSize();

private:
    bool m_isAddZone;
    QMap<QString, QString> m_completionCache;

    DBlurEffectWidget *m_blurEffect;

    QAbstractItemView *m_popup;

    TimezoneMap *m_map;
    SearchInput *m_searchInput;
    QLabel *m_title;
    QPushButton *m_cancelBtn;
    DSuggestButton *m_confirmBtn;
    QCompleter *m_completer;
    const installer::ZoneInfoList m_totalZones;
};

#endif // TIMEZONECHOOSER_H
