//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "personalizationinterface.h"
#include "operation/imagehelper.h"
#include "operation/personalizationexport.hpp"
#include "operation/treelandworker.h"
#include "operation/x11worker.h"
#include "dccfactory.h"
#include "model/thememodel.h"
#include "utils.hpp"


#include <QIcon>
#include <QtQml>
#include <DGuiApplicationHelper>
ThemeVieweModel::ThemeVieweModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_themeModel(nullptr)
{

}

void ThemeVieweModel::setThemeModel(ThemeModel *model)
{
    m_themeModel = model;
    connect(m_themeModel, &ThemeModel::defaultChanged, this, &ThemeVieweModel::updateData);
    connect(m_themeModel, &ThemeModel::picAdded, this, &ThemeVieweModel::updateData);
    connect(m_themeModel, &ThemeModel::itemAdded, this, &ThemeVieweModel::updateData);
    connect(m_themeModel, &ThemeModel::itemRemoved, this, &ThemeVieweModel::updateData);
    updateData();
}

QModelIndex ThemeVieweModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_keys.size())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex ThemeVieweModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ThemeVieweModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_keys.size();

    return 0;
}

int ThemeVieweModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ThemeVieweModel::data(const QModelIndex &index, int role) const
{
    if (m_keys.isEmpty() || !index.isValid())
        return QVariant();

    int row = index.row();
    switch (role) {
    case ThemeVieweModel::NameRole:
        return m_themeModel->getList().value(m_keys.at(row))["Name"].toString();
    case Qt::ToolTipRole:
        return m_themeModel->getList().value(m_keys.at(row))["Comment"].toString();
    case Qt::DecorationRole:
        return QIcon(m_themeModel->getPicList().value(m_keys.at(row)));
    case Qt::CheckStateRole: {
        QString id = m_themeModel->getDefault();
        if (id.endsWith(".light")) {
            id.chop(6);
        } else if (id.endsWith(".dark")) {
            id.chop(5);
        }
        return m_keys.at(row) == id ? Qt::Checked : Qt::Unchecked;
    }
    case ThemeVieweModel::IdRole:
        return m_keys.at(row);
    case ThemeVieweModel::PicRole:
        return m_themeModel->getPicList().value(m_keys.at(row));
    default:
        break;
    }
    return QVariant();
}

void ThemeVieweModel::updateData()
{
    QStringList keys = m_themeModel->keys();
    if (keys.contains("custom")) {
        keys.removeAll("custom");
        keys.push_front("custom");
    }
    beginResetModel();
    m_keys = keys;
    endResetModel();
}

QHash<int, QByteArray> ThemeVieweModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[PicRole] = "pic";
    roles[Qt::CheckStateRole] = "checked";
    return roles;
}

PersonalizationInterface::PersonalizationInterface(QObject *parent) 
: QObject(parent)
, m_model(new PersonalizationModel(this))
, m_imageHelper(new ImageHelper(this))
, m_globalThemeViewModel(new ThemeVieweModel(this))
, m_iconThemeViewModel(new ThemeVieweModel(this))
, m_cursorThemeViewModel(new ThemeVieweModel(this))
{
    qmlRegisterType<PersonalizationExport>("org.deepin.dcc.personalization", 1, 0, "PersonalizationData");
    if (Dtk::Gui::DGuiApplicationHelper::testAttribute(Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform)) {
        m_work = new TreeLandWorker(m_model, this);
    } else {
        m_work = new X11Worker(m_model, this);
    }

    qmlRegisterType<PersonalizationExport>("org.deepin.dcc.personalization", 1, 0, "PersonalizationExport");

    m_globalThemeViewModel->setThemeModel(m_model->getGlobalThemeModel());
    m_iconThemeViewModel->setThemeModel(m_model->getIconModel());
    m_cursorThemeViewModel->setThemeModel(m_model->getMouseModel());

    // after do
    m_work->active(); // refresh data
    m_work->refreshTheme();
    m_work->refreshFont();

    initAppearanceSwitchModel();
}

void PersonalizationInterface::initAppearanceSwitchModel()
{
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();

    auto updateDefault = [this]() {
        ThemeModel *globalTheme = m_model->getGlobalThemeModel();
        QString mode;
        QString themeId = getGlobalThemeId(globalTheme->getDefault(), mode);
        m_appearanceSwitchModel.clear();
        m_appearanceSwitchModel.append(QVariantMap{{"text", tr("Light")}, {"value", ".light"}});
        const QJsonObject &json = globalTheme->getList().value(themeId);
        if (json.isEmpty())
            return;
        if (json["hasDark"].toBool()) {
            m_appearanceSwitchModel.append(QVariantMap{{"text", tr("Auto")}, {"value", ""}});
            m_appearanceSwitchModel.append(QVariantMap{{"text", tr("Dark")}, {"value", ".dark"}});
        }
        Q_EMIT appearanceSwitchModelChanged(m_appearanceSwitchModel);
        if (mode != m_currentAppearance) {
            m_currentAppearance = mode;
            // FIXME: 可能导致外观主题循环绑定
            // Q_EMIT currentAppearanceChanged(m_currentAppearance);
        }
    };

    updateDefault();

    connect(globalTheme, &ThemeModel::defaultChanged, updateDefault);
    connect(globalTheme, &ThemeModel::itemAdded, updateDefault);
    connect(globalTheme, &ThemeModel::itemRemoved, updateDefault);
}

QString PersonalizationInterface::platformName()
{
    return qApp->platformName();
}

void PersonalizationInterface::handleCmdParam(PersonalizationExport::ModuleType type, const QString &cmdParam)
{
    // parse cmd param
    QMap<QString, QString> paramMap;
    QStringList paramPairs = cmdParam.split('&');

    for (const auto &paramPair : paramPairs) {
        QStringList keyValue = paramPair.split('=');

        if (keyValue.size() == 2) {
            QString key = keyValue[0].trimmed();
            QString value = keyValue[1].trimmed();
            paramMap.insert(key, value);
        }
    }

    // handle cmd param
    if (type == PersonalizationExport::Theme) {
        QString operatorType;
        QString value;
        operatorType = paramMap.value("type");
        value = paramMap.value("value");

        if (operatorType == "themeType") {
            bool keepAuto = paramMap.value("keepAuto") == "true";
            if (value == "light") {
                m_work->setAppearanceTheme(".light", keepAuto);
            } else if (value == "dark") {
                m_work->setAppearanceTheme(".dark", keepAuto);
            } else if (value == "auto") {
                m_work->setAppearanceTheme("", keepAuto);
            }
        }
    } else if (type == PersonalizationExport::Wallpaper) {
        QString url;
        QString type;
        QString monitor;
        url = paramMap.value("url");
        if (!isURI(url) && QFile::exists(url)) {
            url = QUrl::fromLocalFile(url).toString();
        }
        type = paramMap.value("type");
        monitor = paramMap.value("monitor");
        if (monitor.isEmpty()) {
            monitor = m_model->getCurrentSelectScreen();
        }
        if (url.isEmpty()) {
            return;
        }
        if (type == "lock") {
            m_work->setLockBackForMonitor(monitor, url, true);
        } else if (type == "desktop") {
            m_work->setBackgroundForMonitor(monitor, url, true);
        } else if (type.isEmpty()) {
            m_work->setLockBackForMonitor(monitor, url, true);
            m_work->setBackgroundForMonitor(monitor, url, true);
        }
    }
}

DCC_FACTORY_CLASS(PersonalizationInterface)

#include "personalizationinterface.moc"
