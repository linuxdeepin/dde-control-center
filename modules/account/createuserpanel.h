#ifndef CREATEUSERPANEL_H
#define CREATEUSERPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include "libdui/dseparatorhorizontal.h"
#include "UserContent/accounttypeline.h"
#include "libdui/dheaderline.h"
#include "confirmbuttonline.h"
#include "passwdline.h"
#include "switchline.h"
#include "inputline.h"

DUI_USE_NAMESPACE

class CreateUserPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CreateUserPanel(QWidget *parent = 0);

private:
    void initHeader();
    void initInfoLine();
    void initInputLline();

private:
    QVBoxLayout *m_layout = NULL;
};

#endif // CREATEUSERPANEL_H
