#ifndef GRUBDBUS_H
#define GRUBDBUS_H

#include <QObject>
#include <QtDBus/QtDBus>

class GrubDbus : public QDBusAbstractInterface
{
    Q_OBJECT

    Q_PROPERTY(bool updating READ updating WRITE setUpdating NOTIFY updatingChanged)
    Q_PROPERTY(QString background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(QString itemColor READ itemColor WRITE setItemColor NOTIFY itemColorChanged)
    Q_PROPERTY(QString selectedItemColor READ selectedItemColor WRITE setSelectedItemColor NOTIFY selectedItemColorChanged)
    Q_PROPERTY(qint32 timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    static GrubDbus *instance();

    bool updating() const;
    QString background() const;
    QString itemColor() const;
    QString selectedItemColor() const;
    qint32 timeout() const;

public Q_SLOTS:
    void setUpdating(bool updating);
    void setBackground(QString background);
    void setItemColor(QString itemColor);
    void setSelectedItemColor(QString selectedItemColor);
    void setTimeout(qint32 timeout);

Q_SIGNALS:
    void updatingChanged(bool updating);
    void backgroundChanged(QString background);
    void itemColorChanged(QString itemColor);
    void selectedItemColorChanged(QString selectedItemColor);
    void timeoutChanged(qint32 timeout);

private Q_SLOTS:
    void dbusPropertyChanged(const QDBusMessage& msg);

protected:
    explicit GrubDbus(QObject *parent = 0);
    ~GrubDbus();

private:
    bool m_updating;
    QString m_background;
    QString m_itemColor;
    QString m_selectedItemColor;
    qint32 m_timeout;

    void updatePropertys(const QVariantMap &changedProps);
};

#endif // GRUBDBUS_H
