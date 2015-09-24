#include "controlline.h"

AccountPasswdLine::AccountPasswdLine(QWidget *parent) : AccountHeaderLine(parent)
{
    m_passwordEdit = new DPasswordEdit();
    m_passwordEdit->setFixedWidth(160);
    m_passwordEdit->setFixedHeight(DUI::BUTTON_HEIGHT);
    connect(m_passwordEdit, &DPasswordEdit::textChanged, this, &AccountPasswdLine::textChanged);
    connect(m_passwordEdit, &DPasswordEdit::focusChanged, this, &AccountPasswdLine::focusChanged);
    setContent(m_passwordEdit);
    m_warningFrame = new InputWarningFrame(m_passwordEdit);
}

AccountPasswdLine::~AccountPasswdLine()
{
    if (m_warningFrame)
        m_warningFrame->deleteLater();
}

void AccountPasswdLine::hideWarning()
{
    m_warningFrame->hide();
}

void AccountPasswdLine::showWarning(const QString &msg)
{
    m_warningFrame->showWarning(msg);
}

void AccountPasswdLine::setText(const QString &text)
{
    m_passwordEdit->setPassword(text);
}

DPasswordEdit *AccountPasswdLine::passwordEdit() const
{
    return m_passwordEdit;
}

QString AccountPasswdLine::text() const
{
    return m_passwordEdit->text();
}


AccountSwitchLine::AccountSwitchLine(QWidget *parent) : AccountHeaderLine(parent)
{
    m_button = new DSwitchButton(this);
    connect(m_button, &DSwitchButton::checkedChanged, this, &AccountSwitchLine::checkChanged);
    setContent(m_button);
    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);
    sh->setFixedWidth(width());
    sh->move(0, height() - 2);
}

bool AccountSwitchLine::check() const
{
    return m_button->checked();
}

void AccountSwitchLine::setCheck(bool s)
{
    m_button->setChecked(s);
}

void AccountSwitchLine::setContent(QWidget *content)
{
    DHeaderLine::setContent(content);
}


AccountTypeLine::AccountTypeLine(QWidget *parent) : AccountHeaderLine(parent)
{
    m_control = new DSegmentedControl(this);
    m_control->addSegmented(tr("Normal User"));
    m_control->addSegmented(tr("Administor"));
    connect(m_control, &DSegmentedControl::currentChanged, this, &AccountTypeLine::typeChanged);
    setContent(m_control);
    DSeparatorHorizontal * separator = new DSeparatorHorizontal(this);
    separator->setFixedWidth(width());
    separator->move(0, height() - 2);
}

int AccountTypeLine::currentIndex() const
{
    return m_control->currentIndex();
}

void AccountTypeLine::setType(int type)
{
    m_control->setCurrentIndex(type);
}



AccountConfirmButtonLine::AccountConfirmButtonLine(QWidget *parent) : AccountHeaderLine(parent)
{
    DTextButton *cancelButton = new DTextButton(tr("Cancel"));
    connect(cancelButton, &DTextButton::clicked, this, &AccountConfirmButtonLine::cancel);
    DTextButton *confirmButton = new DTextButton(tr("Confirm"));
    connect(confirmButton, &DTextButton::clicked, this, &AccountConfirmButtonLine::confirm);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(cancelButton);
    layout->addSpacing(DUI::BUTTON_MARGIN);
    layout->addWidget(confirmButton);
    contentWidget->setLayout(layout);

    setContent(contentWidget);
}


AccountInputLine::AccountInputLine(QWidget *parent) : AccountHeaderLine(parent)
{
    m_lineEdit = new DLineEdit();
    m_lineEdit->setFixedWidth(160);
    connect(m_lineEdit, &DLineEdit::textChanged, this, &AccountInputLine::textChanged);
    connect(m_lineEdit, &DLineEdit::focusChanged, this, &AccountInputLine::focusChanged);
    setContent(m_lineEdit);
    m_warningFrame = new InputWarningFrame(m_lineEdit);
}

AccountInputLine::~AccountInputLine()
{
    if (m_warningFrame)
        m_warningFrame->deleteLater();
}

void AccountInputLine::hideWarning()
{
    m_warningFrame->hide();
}

void AccountInputLine::showWarning(const QString &msg)
{
    m_warningFrame->showWarning(msg);
}

void AccountInputLine::setText(const QString &text)
{
    m_lineEdit->setText(text);
}

QString AccountInputLine::text() const
{
    return m_lineEdit->text();
}
DLineEdit *AccountInputLine::lineEdit() const
{
    return m_lineEdit;
}

void AccountInputLine::setContent(QWidget *content)
{
    DHeaderLine::setContent(content);
}


