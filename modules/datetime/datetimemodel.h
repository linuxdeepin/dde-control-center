#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>

class DatetimeModel : public QObject
{
    Q_OBJECT

public:
    explicit DatetimeModel(QObject* parent =0);

    bool nTP() const;

signals:
    void NTPChanged(bool value);

public slots:
    void setNTP(bool ntp);

private:
    bool m_ntp;
};

#endif // DATETIMEMODEL_H
