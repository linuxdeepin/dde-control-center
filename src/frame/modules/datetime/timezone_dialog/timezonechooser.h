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

#include <QFrame>
#include <QMap>
#include <com_deepin_daemon_langselector.h>
#include <QCompleter>

DWIDGET_USE_NAMESPACE

using LangSelector = com::deepin::daemon::LangSelector;

class QPushButton;
class QComboBox;
class QLabel;
class QAbstractItemView;

namespace installer {
class TimezoneMap;
}

namespace dcc {
namespace widgets {
class SearchInput;
}
}

namespace dcc {
namespace datetime {

class TimeZoneChooser : public QFrame
{
    Q_OBJECT
public:
    explicit TimeZoneChooser();
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
    void setupSize();

private:
    bool m_isAddZone;
    QMap<QString, QString> m_completionCache;

    DBlurEffectWidget *m_blurEffect;

    QAbstractItemView *m_popup;

    installer::TimezoneMap *m_map;
    dcc::widgets::SearchInput *m_searchInput;
    QLabel *m_title;
    QPushButton *m_cancelBtn;
    DSuggestButton *m_confirmBtn;
    LangSelector *m_currLangSelector;
    QCompleter *m_completer;
};

} // namespace datetime
} // namespace dcc

#endif // TIMEZONECHOOSER_H
