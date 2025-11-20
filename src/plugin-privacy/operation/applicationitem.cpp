// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "applicationitem.h"
#include <DPinyin>

DCORE_USE_NAMESPACE

ApplicationItem::ApplicationItem(QObject *parent)
    : QObject(parent)
    , m_uniqueID(0)
{
    connect(this, &ApplicationItem::objectNameChanged, this, &ApplicationItem::dataChanged);
}

ApplicationItem::~ApplicationItem() { }

void ApplicationItem::setUniqueID(unsigned ID)
{
    m_uniqueID = ID;
}

QString ApplicationItem::id() const
{
    return m_id;
}

QMap<QString, QString> ApplicationItem::execs() const
{
    return m_execs;
}

QString ApplicationItem::name() const
{
    return m_name;
}

QString ApplicationItem::appPath() const
{
    return m_appPath;
}

QString ApplicationItem::package() const
{
    return m_package;
}

QStringList ApplicationItem::executablePaths() const
{
    return m_executablePaths;
}

QString ApplicationItem::icon() const
{
    return m_iconStr;
}

QString ApplicationItem::sortField() const
{
    return m_sortField;
}

// 默认允许
bool ApplicationItem::isPremissionEnabled(int premission) const
{
    return m_premissionMap.value(premission, true);
}

void ApplicationItem::setPremissionEnabled(int premission, bool enabled)
{
    // worker 实现
    Q_EMIT requestSetPremissionEnabled(premission, enabled, this);
}

void ApplicationItem::emitDataChanged()
{
    Q_EMIT dataChanged();
}

void ApplicationItem::onIdChanged(const QString &id)
{
    if (m_id == id)
        return;
    m_id = id;
    emitDataChanged();
}

void ApplicationItem::onExecsChanged(const QMap<QString, QString> &execs)
{
    if (m_execs == execs)
        return;
    m_execs = execs;
    emitDataChanged();
}

void ApplicationItem::onNameChanged(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    // 不区分大小写，中文按拼音排序排后面
    QString upperName = m_name.toUpper();

    int category = 2;
    for (const QChar &ch : upperName) {
        if (ch.isSpace())
            continue;
        if (ch.isDigit()) {
            category = 0;
        } else if (ch.isLetter() && ch.unicode() < 128) {
            category = 1;
        } else {
            category = 2;
        }
        break;
    }

    QString pinyin = DTK_CORE_NAMESPACE::Chinese2Pinyin(upperName);
    m_sortField = QString::number(category) + pinyin;
    emitDataChanged();
}

void ApplicationItem::onAppPathChanged(const QString &path)
{
    if (m_appPath == path)
        return;
    m_appPath = path;
    Q_EMIT appPathChanged();
}

void ApplicationItem::onPackageChanged(const QString &package)
{
    if (m_package == package)
        return;
    m_package = package;
    Q_EMIT packageChanged();
}

void ApplicationItem::onExecutablePathsChanged(const QStringList &paths)
{
    if (m_executablePaths == paths)
        return;
    m_executablePaths = paths;
}

void ApplicationItem::onIconChanged(const QString &icon)
{
    m_iconStr = icon;
    emitDataChanged();
}

bool ApplicationItem::onPremissionEnabledChanged(int premission, bool enabled)
{
    if (m_premissionMap[premission] != enabled) {
        m_premissionMap[premission] = enabled;
        return true;
    }
    return false;
}
