#include "passwdline.h"


PasswdLine::PasswdLine(QWidget *parent) : DHeaderLine(parent)
{
    m_passwordEdit = new DPasswordEdit();
    m_passwordEdit->setFixedWidth(160);
    m_passwordEdit->setFixedHeight(DUI::BUTTON_HEIGHT);

    setContent(m_passwordEdit);
}

DPasswordEdit *PasswdLine::passwordEdit() const
{
    return m_passwordEdit;
}

QString PasswdLine::text() const
{
    return m_passwordEdit->text();
}
