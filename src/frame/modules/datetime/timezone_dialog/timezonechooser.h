// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include <DBlurEffectWidget>
#include <DSuggestButton>

#include <QFrame>
#include <QMap>
#include <com_deepin_daemon_langselector.h>
#include <QCompleter>
#include "timezone.h"

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
class DatetimeModel;

class TimeZoneChooser : public QFrame
{
    Q_OBJECT
public:
    explicit TimeZoneChooser(QWidget* parent = nullptr);
    void setIsAddZone(const bool isAdd);
    inline bool isAddZone() { return m_isAddZone; }
    void setCurrentTimeZoneText(const QString &zone);
    void setMode(DatetimeModel* model);

Q_SIGNALS:
    void confirmed(const QString &zone);
    void cancelled();

public Q_SLOTS:
    void setMarkedTimeZone(const QString &timezone);
    void setRightBtnState(QString zone = "");

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

    installer::TimezoneMap *m_map;
    dcc::widgets::SearchInput *m_searchInput;
    QLabel *m_title;
    QPushButton *m_cancelBtn;
    DSuggestButton *m_confirmBtn;
    LangSelector *m_currLangSelector;
    QCompleter *m_completer;
    const installer::ZoneInfoList m_totalZones;
    DatetimeModel* m_model;
};

} // namespace datetime
} // namespace dcc

#endif // TIMEZONECHOOSER_H
