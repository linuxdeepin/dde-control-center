#ifndef NETWORKBASEEDITLINE_H
#define NETWORKBASEEDITLINE_H

#include <QWidget>

#include <libdui/dlabel.h>

#include "dbus/dbusconnectionsession.h"

DUI_USE_NAMESPACE

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
                                 bool alwaysUpdate = false,
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
    bool checkKey();

signals:
    void setTitle(const QString &title);
    void widgetShown();
    void cacheValueChanged();
    void showed();
    void showErrorAlert();
    void readOnlyChanged(bool readOnly);

protected:
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *e) Q_DECL_OVERRIDE;

    bool isValueError() const;

private:
    void setCacheValue(const QJsonValue &value);
    void updateVisible();

    DLabel *titleLabel;
    DBusConnectionSession *m_dbus;
    QJsonValue m_cacheValue;
    /// when setDBusKey if m_updateKeying is true then record key to m_tempValue
    QJsonValue m_tempValue;
    QString m_section;
    QString m_key;
    bool m_alwaysUpdate = false;
    bool m_setKeyAlways = false;
    bool m_readOnly = false;

    bool m_updateKeying = false;

    friend void updateAllEditLineVisible(NetworkBaseEditLine *);
};

#endif // NETWORKBASEEDITLINE_H
