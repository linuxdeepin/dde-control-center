#ifndef PASSWDLINE_H
#define PASSWDLINE_H

#include <QWidget>
#include "libdui/dpasswordedit.h"
#include "libdui/dheaderline.h"
#include "libdui/dconstants.h"

DUI_USE_NAMESPACE

class PasswdLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit PasswdLine(QWidget *parent = 0);
    DPasswordEdit *passwordEdit() const;
    QString text() const;


private:
    DPasswordEdit *m_passwordEdit = NULL;
};

#endif // PASSWDLINE_H
