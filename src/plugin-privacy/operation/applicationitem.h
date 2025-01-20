// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include <DLog>
#include <QIcon>
#include <QMap>
#include <QObject>

Q_DECLARE_LOGGING_CATEGORY(DCC_PRIVACY)

class ApplicationItem;
using ApplicationItemList = QList<ApplicationItem>;
using ApplicationItemListPtr = QList<ApplicationItem*>;

class ApplicationItem : public QObject
{
    Q_OBJECT
public:

    enum PermissionType {
        CameraPermission = 0x100,   // 摄像头
        FoldersPermission = 0x200,  // 文件和文件夹

        DocumentFoldersPermission,
        PictureFoldersPermission,
        DesktopFoldersPermission,
        VideoFoldersPermission,
        MusicFoldersPermission,
        DownloadFoldersPermission,

        GroupPermissionMask = 0xff00, // 组权限掩码
    };

    Q_ENUM(PermissionType)
    explicit ApplicationItem(QObject *parent = nullptr);
    ~ApplicationItem();

    inline unsigned getUniqueID() const { return m_uniqueID; }

    QString id() const;
    QMap<QString, QString> execs() const;
    QString name() const;
    QString appPath() const;
    QString package() const;
    QString icon() const;
    QString sortField() const;

    bool isPremissionEnabled(int premission) const;
    void setPremissionEnabled(int premission, bool enabled);

Q_SIGNALS:
    void dataChanged();
    void appPathChanged();
    void packageChanged();
    void requestSetPremissionEnabled(int premission, bool enabled, ApplicationItem *item);

protected Q_SLOTS:
    void emitDataChanged();

    void setUniqueID(unsigned ID);
    void onIdChanged(const QString &id);
    void onExecsChanged(const QMap<QString, QString> &execs);
    void onNameChanged(const QString &name);
    void onAppPathChanged(const QString &path);
    void onPackageChanged(const QString &package);
    void onExecutablePathsChanged(const QStringList &paths);
    void onIconChanged(const QString &icon);
    bool onPremissionEnabledChanged(int premission, bool enabled);

    QStringList executablePaths() const;

private:
    unsigned m_uniqueID;
    QString m_id;                    // 应用id
    QMap<QString, QString> m_execs;  // 应用Desktop的所有Exec
    QString m_name;                  // 显示名
    QString m_appPath;               // app路径
    QString m_package;               // 包名
    QStringList m_executablePaths;   // 可执行程序路径
    QString m_iconStr;               // 图标名
    QMap<int, bool> m_premissionMap; // 应用权限
    QString m_sortField;             // 用于排序
    QString m_desktopPath;

    friend class PrivacySecurityWorker;
    friend class PrivacySecurityModel;
};

#endif // APPLICATIONITEM_H
