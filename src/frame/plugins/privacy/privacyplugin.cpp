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

#include "privacyplugin.h"

#include <QTranslator>

PrivacyPlugin::PrivacyPlugin()
    : m_translator(new QTranslator())
{
    // 加载插件翻译文件
    QString transPath = "/usr/share/privacy-plugin/translations/privacy-plugin_" + QLocale::system().name() + ".qm";
    if (m_translator->load(transPath))
        qApp->installTranslator(m_translator);
}

PrivacyPlugin::~PrivacyPlugin()
{
    if (m_translator)
        m_translator->deleteLater();

    if (m_privacyWidget)
        m_privacyWidget->deleteLater();

    if (m_locationWidget)
        m_locationWidget->deleteLater();
}

QIcon PrivacyPlugin::icon() const
{
    return QIcon(":/res/privacy.svg");
}

void PrivacyPlugin::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync)
    Q_UNUSED(pushtype);
}

void PrivacyPlugin::initialize()
{

}

const QString PrivacyPlugin::name() const
{
    return QStringLiteral("privacy");
}

const QString PrivacyPlugin::displayName() const
{
    return tr("Privacy");
}

void PrivacyPlugin::active()
{
    m_privacyWidget = new PrivacyWidget();
    m_privacyWidget->setVisible(false);
    m_frameProxy->pushWidget(this, m_privacyWidget);
    // 初始化时显示定位服务三级界面
    ShowPrivacyView(0);
    connect(m_privacyWidget, &PrivacyWidget::requestShowPrivacyView, this, &PrivacyPlugin::ShowPrivacyView);
    m_privacyWidget->setVisible(true);
}

int PrivacyPlugin::load(const QString &path)
{
    Q_UNUSED(path);
    if (!m_privacyWidget)
        active();
    return 0;
}

QStringList PrivacyPlugin::availPage() const
{
    QStringList list;
    list << "Privacy" << "translation";

    return list;
}

QString PrivacyPlugin::translationPath() const
{
    return QString(":/translation/assistant-control-plugin_%1.ts");
}

void PrivacyPlugin::deactive()
{

}

void PrivacyPlugin::contentPopped(QWidget * const w)
{
    Q_UNUSED(w);
}

void PrivacyPlugin::reset()
{

}

void PrivacyPlugin::ShowPrivacyView(int index)
{
    switch (index) {
    case PrivacyType::Locayion:
        m_locationWidget = new LocationWidget();
        m_locationWidget->setVisible(false);
        m_frameProxy->pushWidget(this, m_locationWidget);
        m_locationWidget->setVisible(true);
    }
}
