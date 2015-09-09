#include "inputline.h"

InputLine::InputLine(QWidget *parent) : DHeaderLine(parent)
{
    m_lineEdit = new DLineEdit();
    m_lineEdit->setFixedWidth(160);
    connect(m_lineEdit, &DLineEdit::textChanged, this, &InputLine::textChanged);
    connect(m_lineEdit, &DLineEdit::focusChanged, this, &InputLine::focusChanged);
    setContent(m_lineEdit);
}

InputLine::~InputLine()
{
    if (m_warningFrame)
        m_warningFrame->deleteLater();
}

void InputLine::hideWarning()
{
    if (!m_warningFrame)
        m_warningFrame = new InputWarningFrame(m_lineEdit);
    m_warningFrame->hide();
}

void InputLine::showWarning(const QString &msg)
{
    if (!m_warningFrame)
        m_warningFrame = new InputWarningFrame(m_lineEdit);
    m_warningFrame->showWarning(msg);
}

void InputLine::setText(const QString &text)
{
    m_lineEdit->setText(text);
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


