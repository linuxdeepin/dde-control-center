#ifndef COMPLEXAPPLETITEM_H
#define COMPLEXAPPLETITEM_H

#include <QFrame>
#include <QLabel>
#include "dbus/dbusnetwork.h"
#include "libdui/dimagebutton.h"
#include "../network-data/networkdata.h"

DUI_USE_NAMESPACE
using namespace NetworkPlugin;

class ComplexAppletItem final : public DImageButton
{
    Q_OBJECT
public:
    explicit ComplexAppletItem(QWidget *parent = 0);

    void setIcon(const QString &icon);
    void setIndex(const QString &value);

private:
    QLabel *m_indexLabel;
};

#endif // COMPLEXAPPLETITEM_H
