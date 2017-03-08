/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include <QFrame>
#include <QMap>

#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

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

signals:
    void confirmed(const QString &zone);
    void cancelled();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QSize getFitSize() const;
    void setupSize();

private:
    QMap<QString, QString> m_completionCache;

    DBlurEffectWidget *m_blurEffect;

    QAbstractItemView *m_popup;

    installer::TimezoneMap *m_map;
    dcc::widgets::SearchInput *m_searchInput;
    QLabel *m_title;
    QPushButton *m_cancelBtn;
    QPushButton *m_confirmBtn;
};

} // namespace datetime
} // namespace dcc

#endif // TIMEZONECHOOSER_H
