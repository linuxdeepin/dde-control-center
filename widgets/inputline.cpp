#include "inputline.h"

InputLine::InputLine(QWidget *parent) : DHeaderLine(parent)
{
    m_lineEdit = new DLineEdit();
    m_lineEdit->setFixedWidth(160);
    connect(m_lineEdit, &DLineEdit::textChanged, this, &InputLine::textChanged);

    setContent(m_lineEdit);
}

QString InputLine::text() const
{
    return m_lineEdit->text();
}
DLineEdit *InputLine::lineEdit() const
{
    return m_lineEdit;
}

void InputLine::setContent(QWidget *content)
{
    DHeaderLine::setContent(content);
}


