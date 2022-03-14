/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangwei <wangwei@uniontech.com>
 *
 * Maintainer: wangwei <wangwei@uniontech.com>
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
#include <QString>
#include <QObject>

class ProtocolFile : public QObject
{
    Q_OBJECT

public:
    // 获取隐私协议
    static void getPrivacyFile(QString &zhCN_Content, QString &enUS_Content);
    // 获取用户体验计划许可协议
    static QString getUserExpContent();
    // 最终用户许可协议
    static QString getEnduserAgreement();
    // 欧拉最终用户许可协议
    static QString getEulerEnduserAgreement();
};
