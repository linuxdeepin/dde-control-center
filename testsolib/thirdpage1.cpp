#include "thirdpage1.h"

#include <QVBoxLayout>
#include <QPushButton>

ThirdPage1::ThirdPage1(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *contentLayout = new QVBoxLayout(this);
    QPushButton *button1 = new QPushButton("button1");
    QPushButton *button2 = new QPushButton("button2");
    QPushButton *button3 = new QPushButton("button3");
    contentLayout->setMargin(0);
    contentLayout->addWidget(button1);
    contentLayout->addWidget(button2);
    contentLayout->addWidget(button3);
    setLayout(contentLayout);
}
