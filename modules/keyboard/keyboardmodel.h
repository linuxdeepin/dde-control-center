#ifndef KEYBOARDMODEL_H
#define KEYBOARDMODEL_H

#include <QObject>
#include "indexmodel.h"

namespace dcc {
namespace keyboard{

class KeyboardModel : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardModel(QObject *parent = 0);

    void setLayoutLists(QMap<QString, QString> lists);

    QString layoutByValue(const QString& value);
    QString langByKey(const QString& key) const;

    QString curLayout() const;
    QString curLang() const;
    QStringList userLayout() const;
    QList<MetaData> langLists() const;
    bool capsLock() const;

signals:
    void requestCurLayout(const QString& layout);
    void requestCurLang(const QString& lang);
    void requestCapsLockChanged(bool value);

public slots:
    void setLayout(const QString& value);
    void setLang(const QString& value);
    void setUserLayout(const QStringList& list);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void setLocaleList(const QList<MetaData>& langs);
    void setCapsLock(bool value);
private:
    bool m_capsLock;
    uint m_repeatInterval;
    uint m_repeatDelay;
    QString m_layout;
    QString m_lang;
    QStringList m_userLayout;
    QMap<QString, QString> m_layouts;
    QList<MetaData> m_langs;
};
}
}
#endif // KEYBOARDMODEL_H
