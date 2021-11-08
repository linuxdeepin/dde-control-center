/*
 * Copyright (C) 2014 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef NOTIFICATIONENTITY_H
#define NOTIFICATIONENTITY_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <memory>
#include <QDateTime>
/*!
 * \~chinese \class 通知的数据结构类
 * \~chinese \brief 设置或者返回通知的信息
 */
class NotificationEntity : public QObject
{
    Q_OBJECT

public:
    NotificationEntity(const QString &appName = QString(), const QString &id = QString(),
                       const QString &appIcon = QString(), const QString &summary = QString(),
                       const QString &body = QString(), const QStringList &actions = QStringList(),
                       const QVariantMap hints = QVariantMap(), const QString &ctime = QString::number(QDateTime::currentMSecsSinceEpoch()),
                       const QString &replacesId = QString(), const QString &timeout = QString(),
                       QObject *parent = nullptr);

    NotificationEntity(const NotificationEntity &notify);

    QString appName() const;
    void setAppName(const QString &appName);

    uint id() const;
    void setId(const QString &id);

    QString appIcon() const;
    void setAppIcon(const QString &appIcon);

    QString summary() const;
    void setSummary(const QString &summary);

    QString body() const;
    void setBody(const QString &body);

    QStringList actions() const;
    void setActions(const QStringList &actions);

    QVariantMap hints() const;
    void setHints(const QVariantMap &hints);

    QString ctime() const;
    void setTime(const QString &time);

    QString replacesId() const;
    void setReplacesId(const QString &replacesId);

    QString timeout() const;
    void setTimeout(const QString &timeout);

    bool isTitle() { return m_isTitle;}
    void setIsTitle(bool is);

    void setHideCount(int count);
    int hideCount() {return m_hideCount;}

    void setShowInNotifyCenter(bool isShow);
    bool isShowInNotifyCenter() {return m_showInNotifyCenter;}

    void setShowPreview(bool show);
    bool isShowPreview() { return m_showPreview; }

    void setCurrentIndex(int idx);
    int currentIndex() { return m_index; }

private:
    QString m_appName;
    uint m_id;
    QString m_appIcon;
    QString m_summary;
    QString m_body;
    QStringList m_actions;
    QVariantMap m_hints;
    QString m_ctime;
    QString m_replacesId;
    QString m_timeout;
    bool m_isTitle;     //是否是标题
    int m_hideCount;    //此条消息下面被层叠的消息数量
    bool m_showPreview;
    int m_index;
    bool m_showInNotifyCenter;
};

Q_DECLARE_METATYPE(std::shared_ptr<NotificationEntity>);

#endif // NOTIFICATIONENTITY_H
