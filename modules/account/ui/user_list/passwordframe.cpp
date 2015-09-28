#include "passwordframe.h"

PasswordFrame::PasswordFrame(QWidget *parent) : QStackedWidget(parent)
{
    initInactiveWidget();
    initActiveWidget();

    setCurrentIndex(0);

    installEventFilter(this);
}

void PasswordFrame::preDestroy()
{
    m_lineNew->hideWarning();
    m_lineRepeat->hideWarning();
}

bool PasswordFrame::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        m_lineNew->hideWarning();
        m_lineRepeat->hideWarning();
    }

    return QStackedWidget::eventFilter(obj, event);
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
    AccountHeaderLine * line = new AccountHeaderLine();
    DTextButton *button = new DTextButton("\u2022\u0020\u2022\u0020\u2022\u0020\u2022");
    button->setStyleSheet("border:none;color: #b4b4b4;font-size: 14px;outline:none;");
    line->setTitle(tr("Password"));
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

    m_lineNew = new AccountPasswdLine();
    m_lineNew->setTitle(tr("New Password"));
    m_lineRepeat = new AccountPasswdLine();
    m_lineRepeat->setTitle(tr("Repeat"));
    connect(m_lineNew, &AccountPasswdLine::textChanged, this, &PasswordFrame::onPasswdChanged);
    connect(m_lineNew, &AccountPasswdLine::focusChanged, this, &PasswordFrame::onPasswdFocusChanged);
    connect(m_lineRepeat, &AccountPasswdLine::textChanged, this, &PasswordFrame::onPasswdRepeatChanged);
    connect(m_lineRepeat, &AccountPasswdLine::focusChanged, this, &PasswordFrame::onPasswdRepeatFocusChanged);

    AccountConfirmButtonLine *lineButton = new AccountConfirmButtonLine();
    connect(lineButton, &AccountConfirmButtonLine::cancel, [=]{
        setCurrentIndex(0);
        setFixedHeight(m_lineNew->height());

        emit sizeChanged();

        resetData();
    });
    connect(lineButton, &AccountConfirmButtonLine::confirm, [=]{
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


