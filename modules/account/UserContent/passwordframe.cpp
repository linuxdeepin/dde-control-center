#include "passwordframe.h"

PasswordFrame::PasswordFrame(QWidget *parent) : QStackedWidget(parent)
{
    initInactiveWidget();
    initActiveWidget();

    setCurrentIndex(0);
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

    PasswdLine *lineNew = new PasswdLine();
    lineNew->setTitle("New Password");
    PasswdLine *lineRepeat = new PasswdLine();
    lineRepeat->setTitle("Repeat Password");
    ConfirmButtonLine *lineButton = new ConfirmButtonLine();
    connect(lineButton, &ConfirmButtonLine::cancel, [=]{
        setCurrentIndex(0);
        setFixedHeight(lineNew->height());

        emit sizeChanged();
    });
    connect(lineButton, &ConfirmButtonLine::confirm, [=]{
        //TODO, show warning
        if (!lineNew->text().isEmpty() && !lineRepeat->text().isEmpty() && lineNew->text() == lineRepeat->text())
            emit changePassword(lineNew->text());

        setCurrentIndex(0);
        setFixedHeight(lineNew->height());

        emit sizeChanged();
    });

    layout->addWidget(lineNew);
    layout->addWidget(lineRepeat);
    layout->addWidget(lineButton);

    addWidget(m_inactiveWidget);
}


