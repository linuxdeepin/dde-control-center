#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#include <QObject>

struct ShortcutInfo
{
    ShortcutInfo();

    QString accels;
    QString id;
    QString name;
    QString pinyin;
    QString upper;
    quint32 type;
    bool used;
    bool operator==(const ShortcutInfo &info) const
    {
        return id == info.id;
    }
};

typedef QList<ShortcutInfo> ShortcutInfoList;

class ShortcutModel : public QObject
{
    Q_OBJECT


public:
    explicit ShortcutModel(QObject *parent = 0);
    ~ShortcutModel();

    QList<ShortcutInfo*> systemInfo() const;
    QList<ShortcutInfo*> windowInfo() const;
    QList<ShortcutInfo*> workspaceInfo() const;
    QList<ShortcutInfo*> infos() const;

signals:
    void parseFinish();

public slots:
    void onParseInfo(const QString& info);

private:
    QString m_info;
    QList<ShortcutInfo*> m_infos;
    QList<ShortcutInfo*> m_systemInfos;
    QList<ShortcutInfo*> m_windowInfos;
    QList<ShortcutInfo*> m_workspaceInfos;
    QList<ShortcutInfo*> m_customInfos;
};

#endif // SHORTCUTMODEL_H
