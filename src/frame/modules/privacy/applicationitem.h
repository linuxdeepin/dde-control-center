// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include <QIcon>
#include <QMap>
#include <QObject>

enum PermissionType {
    CameraPermission = 1,       // 摄像头
    FoldersPermission,          // 文件和文件夹
    DocumentsFoldersPermission, // 文档文件夹
    PicturesFoldersPermission,  // 图片文件夹
};

class ApplicationItem : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationItem(QObject *parent = nullptr);
    ~ApplicationItem();

    inline unsigned getUniqueID() const { return m_uniqueID; }
    QString id() const;
    QString name() const;
    QString appPath() const;
    QIcon icon();
    QString sortField() const;

    bool isPremissionEnabled(int premission) const;
    void setPremissionEnabled(int premission, bool enabled);

Q_SIGNALS:
    void dataChanged();
    void appPathChanged();
    void requestSetPremissionEnabled(int premission, bool enabled, ApplicationItem *item);

protected Q_SLOTS:
    void emitDataChanged();

    void setUniqueID(unsigned ID);
    void onIdChanged(const QString &id);
    void onNameChanged(const QString &name);
    void onAppPathChanged(const QString &path);
    void onExecutablePathsChanged(const QStringList &paths);
    void onIconChanged(const QString &icon);
    void onPremissionEnabledChanged(int premission, bool enabled);

    QStringList executablePaths() const;

private:
    unsigned m_uniqueID;
    QString m_id;                    // .desktop名，用于Launcherd的id
    QString m_name;                  // 显示名
    QString m_appPath;               // app路径
    QStringList m_executablePaths;   // 可执行程序路径
    QString m_iconStr;               // 图标名
    QIcon m_icon;                    // 图标
    QMap<int, bool> m_premissionMap; // 应用权限
    QString m_sortField;             // 用于排序

    friend class PrivacySecurityWorker;
    friend class PrivacySecurityModel;
};

#endif // APPLICATIONITEM_H
