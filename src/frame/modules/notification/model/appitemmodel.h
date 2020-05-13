/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#pragma once

#include "interface/namespace.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace dcc {
namespace notification {

class AppItemModel : public QObject
{
    Q_OBJECT
public:
    explicit AppItemModel(QObject *parent = nullptr);
    void setItem(const QString &name, const QJsonObject &item);
    QJsonObject convertQJson();

    inline QString getAppName()const {return  m_softName;}
    void setSoftName(const QString &name);

    inline bool isAllowNotify()const {return m_isAllowNotify;}
    void setAllowNotify(const bool &state);

    inline bool isNotifySound()const {return m_isNotifySound;}
    void setNotifySound(const bool &state);

    inline bool isLockShowNotify()const {return m_isLockShowNotify;}
    void setLockShowNotify(const bool &state);

    inline bool isOnlyInNotifyCenter()const {return m_isOnlyInNotifyCenter;}
    void setOnlyInNotifyCenter(const bool &state);

    inline bool isShowNotifyPreview()const {return m_isShowNotifyPreview;}
    void setShowNotifyPreview(const bool &state);

Q_SIGNALS:
    void softNameChanged(QString name);
    void allowNotifyChanged(bool state);
    void notifySoundChanged(bool state);
    void lockShowNotifyChanged(bool state);
    void onlyInNotifyCenterChanged(bool state);
    void showNotifyPreviewChanged(bool state);

private:
    QString m_softName;//应用程序名
    bool m_isAllowNotify;//允许应用通知
    bool m_isNotifySound;//是否有通知声音
    bool m_isLockShowNotify;//锁屏显示通知
    bool m_isOnlyInNotifyCenter;//通知仅在通知中心显示
    bool m_isShowNotifyPreview;//显示消息预览
};

}
}
