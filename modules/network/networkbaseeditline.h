#ifndef NETWORKBASEEDITLINE_H
#define NETWORKBASEEDITLINE_H

#include <QWidget>

#include "dbus/dbusconnectionsession.h"

class NetworkBaseEditLine : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString section READ section WRITE setSection)
    Q_PROPERTY(QString key READ key WRITE setKey)
    Q_PROPERTY(QJsonValue cacheValue READ cacheValue NOTIFY cacheValueChanged)
    /// update cacheValue even if other key changed
    Q_PROPERTY(bool alwaysUpdate READ alwaysUpdate WRITE setAlwaysUpdate)
    /// if true, don't compare cache value with backend when setting key
    Q_PROPERTY(bool setKeyAlways READ setKeyAlways WRITE setSetKeyAlways)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit NetworkBaseEditLine(const QString &section, const QString &key,
                                 DBusConnectionSession *dbus, const QString &title,
                                 QWidget *parent = 0);

    QString section() const;
    QString key() const;
    QJsonValue cacheValue() const;
    bool alwaysUpdate() const;
    bool setKeyAlways() const;
    bool readOnly() const;

    void setDBusKey(const QJsonValue &key);
    const QJsonValue dbusKey();
    const QJsonArray getAvailableValues();
    const QList<QJsonValue> getAvailableValuesValue();
    const QString getAvailableValuesTextByValue();
    const QStringList getAvailableValuesText();
    int getAvailableValuesIndex();
    void setRightWidget(QWidget *widget);

public slots:
    void setDBusKey(const QString &key);
    void setSection(const QString &section);
    void setKey(const QString &key);
    void setAlwaysUpdate(bool alwaysUpdate);
    void setSetKeyAlways(bool setKeyAlways);
    void setReadOnly(bool readOnly);
    void checkKey();

signals:
    void setTitle(const QString &title);
    void widgetShown();
    void cacheValueChanged();
    void showed();
    void showErrorAlert();
    void readOnlyChanged(bool readOnly);

protected:
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;

private:
    void setCacheValue(const QJsonValue &value);
    void updateVisible();
    bool isValueError() const;

    DBusConnectionSession *m_dbus;
    QJsonValue m_cacheValue;
    QString m_section;
    QString m_key;
    bool m_alwaysUpdate = false;
    bool m_setKeyAlways = false;
    bool m_readOnly = false;
};

#endif // NETWORKBASEEDITLINE_H
