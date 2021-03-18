/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#include "batteryhealthplugin.h"

BatteryHealthPlugin::BatteryHealthPlugin()
    : m_translator(new QTranslator())
{
    // 加载插件翻译文件
    QString transPath = "/usr/share/battery-health/translations/battery-health_" + QLocale::system().name() + ".qm";
    if (m_translator->load(transPath))
        qApp->installTranslator(m_translator);
}

BatteryHealthPlugin::~BatteryHealthPlugin()
{
    if (m_translator)
        m_translator->deleteLater();

    if (m_batteryHealthwidget)
        m_batteryHealthwidget->deleteLater();
}

QIcon BatteryHealthPlugin::icon() const
{
    return QIcon(":/res/batteryhealth.svg");
}

void BatteryHealthPlugin::preInitialize(bool sync, FrameProxyInterface::PushType)
{

}

void BatteryHealthPlugin::initialize()
{

}

const QString BatteryHealthPlugin::name() const
{
    return QStringLiteral("batteryhealth");
}

const QString BatteryHealthPlugin::displayName() const
{
    return tr("BatteryHealth");
}

void BatteryHealthPlugin::active()
{
    m_batteryHealthwidget = new BatteryHealthwidget();
    m_batteryHealthwidget->setVisible(false);
    m_frameProxy->pushWidget(this, m_batteryHealthwidget);
    m_batteryHealthwidget->setVisible(true);
}

int BatteryHealthPlugin::load(const QString &path)
{
    Q_UNUSED(path);
    if (!m_batteryHealthwidget)
        active();
    return 0;
}

QStringList BatteryHealthPlugin::availPage() const
{
    QStringList list;
    list << "BatteryHealth" << "translation";

    return list;
}

QString BatteryHealthPlugin::translationPath() const
{
    return QString(":/translation/battery-health_%1.ts");
}

void BatteryHealthPlugin::deactive()
{

}

void BatteryHealthPlugin::contentPopped(QWidget * const w)
{
    Q_UNUSED(w);
}

void BatteryHealthPlugin::reset()
{

}
