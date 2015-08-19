#include "passwordline.h"

InputLine::InputLine(QWidget *parent) : DHeaderLine(parent)
{
    m_edit = new DPasswordEdit();
    m_edit->setFixedWidth(160);

    setContent(m_edit);
}

QString InputLine::text() const
{
//    return m_edit->text();
    return "";
}

ButtonLine::ButtonLine(QWidget *parent) : DHeaderLine(parent)
{
    DTextButton *cancelButton = new DTextButton("Cancel");
    connect(cancelButton, &DTextButton::clicked, this, &ButtonLine::cancel);
    DTextButton *applyButton = new DTextButton("Confirm");
    connect(applyButton, &DTextButton::clicked, this, &ButtonLine::apply);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(cancelButton);
    layout->addSpacing(DUI::BUTTON_MARGIN);
    layout->addWidget(applyButton);
    contentWidget->setLayout(layout);

    setContent(contentWidget);
}

PasswordLine::PasswordLine(QWidget *parent) : QStackedWidget(parent)
{
    initInactiveWidget();
    initActiveWidget();

    setCurrentIndex(0);
}

void PasswordLine::initInactiveWidget()
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

void PasswordLine::initActiveWidget()
{
    m_inactiveWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_inactiveWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    InputLine *lineNew = new InputLine();
    lineNew->setTitle("New Password");
    InputLine *lineRepeat = new InputLine();
    lineRepeat->setTitle("Repeat Password");
    ButtonLine *lineButton = new ButtonLine();
    connect(lineButton, &ButtonLine::cancel, [=]{
        setCurrentIndex(0);
        setFixedHeight(lineNew->height());

        emit sizeChanged();
    });
    connect(lineButton, &ButtonLine::apply, [=]{
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


