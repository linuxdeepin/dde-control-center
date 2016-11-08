#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include <QObject>
#include <QStringList>
#include <QDebug>

class DefAppWorker;

class Category : public QObject {
    Q_OBJECT

public:
    explicit Category(QObject *parent = 0);
    void setList(const QList<QStringList> &list);   //设置list到暂存区
    void setDefault(const QString &id);

    inline const QString getName() const { return m_category;}
    inline void setCategory(const QString &category){ m_category = category;}
    inline const QList<QStringList> getList() const { return m_list;}
    inline const QString getDefault(){ return m_id;}

signals:
    void itemsAdded(const QStringList &list);
    void itemsRemoved(const QStringList &list);
    void defaultChanged(const QString &id);

private:
    QList<QStringList> m_list; //暂存list
    QString m_category;
    QString m_id;
};

class DefAppModel : public QObject
{
    Q_OBJECT

    friend class DefAppWorker;

public:
    explicit DefAppModel(QObject *parent = 0);
    inline bool setAutoOpen() {return m_autoOpen;}
    Category *getCategory(const QString &category);

signals:
    void AutoOpenChanged(bool state);

private slots:
    void setAppList(const QString &category, QList<QStringList> &list); //该函数遍历对象来设置list
    void setDefault(const QString &category, const QString &id);
    void setAutoOpen(const bool state);

private:
    bool m_autoOpen;

    Category *m_modBrowser = nullptr;
    Category *m_modMail = nullptr;
    Category *m_modText = nullptr;
    Category *m_modMusic = nullptr;
    Category *m_modVideo = nullptr;
    Category *m_modPicture = nullptr;
    Category *m_modTerminal = nullptr;

    Category *m_modCDAudio = nullptr;
    Category *m_modDVDVideo = nullptr;
    Category *m_modMusicPlayer = nullptr;
    Category *m_modCamera = nullptr;
    Category *m_modSoftware = nullptr;

    QList<Category*> m_categoryList;
};

#endif // DEFAPPMODEL_H
