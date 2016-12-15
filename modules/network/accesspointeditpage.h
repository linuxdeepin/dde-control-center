#ifndef ACCESSPOINTEDITPAGE_H
#define ACCESSPOINTEDITPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace network {

class AccessPointEditPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit AccessPointEditPage(const QString &path, QWidget *parent = 0);

public slots:
    void onDeviceRemoved();

private:
    const QString m_path;
};

}

}

#endif // ACCESSPOINTEDITPAGE_H
