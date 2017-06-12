#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#define MEDIAKEY 2

#include <QObject>

namespace dcc {
namespace keyboard{

class ShortcutItem;
struct ShortcutInfo
{
    ShortcutInfo();

    QString accels;
    QString id;
    QString name;
    QString command;
    quint32 type;
    ShortcutItem* item;

    bool operator==(const ShortcutInfo &info) const
    {
        return id == info.id && type == info.type;
    }

    QString toString() {
        return name + accels + command + id + QString::number(type);
    }
};

typedef QList<ShortcutInfo> ShortcutInfoList;

class ShortcutModel : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutModel(QObject *parent = 0);
    ~ShortcutModel();
    enum InfoType{
        System,
        Custom,
        Media,
        Window,
        Workspace
    };

    QList<ShortcutInfo*> systemInfo() const;
    QList<ShortcutInfo*> windowInfo() const;
    QList<ShortcutInfo*> workspaceInfo() const;
    QList<ShortcutInfo*> customInfo() const;
    QList<ShortcutInfo*> infos() const;

    void delInfo(ShortcutInfo* info);

signals:
    void listChanged(QList<ShortcutInfo *>, InfoType);
    void addCustomInfo(ShortcutInfo* info);

public slots:
    void onParseInfo(const QString& info);
    void onCustomInfo(const QString& json);

private:
    QString m_info;
    QList<ShortcutInfo*> m_infos;
    QList<ShortcutInfo*> m_systemInfos;
    QList<ShortcutInfo*> m_windowInfos;
    QList<ShortcutInfo*> m_workspaceInfos;
    QList<ShortcutInfo*> m_customInfos;
};

}
}
#endif // SHORTCUTMODEL_H
