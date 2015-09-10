#include "passwordframe.h"

PasswordFrame::PasswordFrame(QWidget *parent) : QStackedWidget(parent)
{
    initInactiveWidget();
    initActiveWidget();

    setCurrentIndex(0);
}

void PasswordFrame::preDestroy()
{
    m_lineNew->hideWarning();
    m_lineRepeat->hideWarning();
}

void PasswordFrame::onPasswdFocusChanged(bool focus)
{
    if (focus){
        m_lineNew->hideWarning();
        m_lineRepeat->hideWarning();
        if (!m_lineRepeat->text().isEmpty() && m_lineRepeat->text() != m_lineNew->text())
            m_lineRepeat->showWarning(tr("The two passwords do not match."));
    }
}

void PasswordFrame::onPasswdRepeatFocusChanged(bool focus)
{
    if (focus){
        m_lineNew->hideWarning();
        m_lineRepeat->hideWarning();
    }
}

void PasswordFrame::onPasswdChanged(const QString &)
{
    m_lineNew->hideWarning();
    m_lineRepeat->hideWarning();
    if (!m_lineRepeat->text().isEmpty() && m_lineNew->text() != m_lineRepeat->text())
        m_lineRepeat->showWarning(tr("The two passwords do not match."));
}

void PasswordFrame::onPasswdRepeatChanged(const QString &passwd)
{
    m_lineNew->hideWarning();
    m_lineRepeat->hideWarning();
    if (!m_lineRepeat->text().isEmpty() && m_lineNew->text().indexOf(passwd, 0) != 0)
        m_lineRepeat->showWarning(tr("The two passwords do not match."));
}

void PasswordFrame::initInactiveWidget()
{
    DHeaderLine * line = new DHeaderLine();
    DTextButton *button = new DTextButton("....");
    line->setTitle("Password");
    line->setContent(button);
    connect(button, &QPushButton::clicked, [=]{
        setCurrentIndex(1);
        setFixedHeight(3 * line->height());

        emit sizeChanged();
    });

    addWidget(line);
    setFixedHeight(line->height());
}

void PasswordFrame::initActiveWidget()
{
    m_inactiveWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_inactiveWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_lineNew = new PasswdLine();
    m_lineNew->setTitle("New Password");
    m_lineRepeat = new PasswdLine();
    m_lineRepeat->setTitle("Repeat Password");
    connect(m_lineNew, &PasswdLine::textChanged, this, &PasswordFrame::onPasswdChanged);
    connect(m_lineNew, &PasswdLine::focusChanged, this, &PasswordFrame::onPasswdFocusChanged);
    connect(m_lineRepeat, &PasswdLine::textChanged, this, &PasswordFrame::onPasswdRepeatChanged);
    connect(m_lineRepeat, &PasswdLine::focusChanged, this, &PasswordFrame::onPasswdRepeatFocusChanged);

    ConfirmButtonLine *lineButton = new ConfirmButtonLine();
    connect(lineButton, &ConfirmButtonLine::cancel, [=]{
        setCurrentIndex(0);
        setFixedHeight(m_lineNew->height());

        emit sizeChanged();

        resetData();
    });
    connect(lineButton, &ConfirmButtonLine::confirm, [=]{
        if (validate()){
            emit changePassword(m_lineNew->text());

            setCurrentIndex(0);
            setFixedHeight(m_lineNew->height());

            emit sizeChanged();

            resetData();
        }
    });

    layout->addWidget(m_lineNew);
    layout->addWidget(m_lineRepeat);
    layout->addWidget(lineButton);

    addWidget(m_inactiveWidget);
}

void PasswordFrame::resetData()
{
    m_lineNew->setText("");
    m_lineRepeat->setText("");
}

bool PasswordFrame::validate()
{
    if (m_lineNew->text().isEmpty()){
        m_lineNew->showWarning(tr("Password can not be empty."));
        return false;
    }

    if (m_lineRepeat->text() != m_lineNew->text()){
        m_lineRepeat->showWarning(tr("The two passwords do not match."));
        return false;
    }

    return true;
}


