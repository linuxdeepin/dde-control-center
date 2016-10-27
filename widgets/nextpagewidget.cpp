#include "nextpagewidget.h"

#include <QHBoxLayout>

using namespace dcc;

DWIDGET_USE_NAMESPACE

NextPageWidget::NextPageWidget(QFrame *parent)
    : SettingsItem(parent)
{
    m_title = new QLabel;
    m_title->setText("title");

    m_value = new QLabel;
    m_value->setText("");

    m_nextPageBtn = new DImageButton(":/icon/next_normal.png",
                                     ":/icon/next_hover.png",
                                     ":/icon/next_press.png");

    m_nextPageBtn->setFixedSize(32, 32);
//    m_nextPageBtn->setStyleSheet("background-color:red;");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_value);
    mainLayout->addWidget(m_nextPageBtn);

    setLayout(mainLayout);

    connect(m_nextPageBtn, &DImageButton::clicked, this, &NextPageWidget::clicked);
}

void NextPageWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void NextPageWidget::setValue(const QString &value)
{
    m_value->setText(value);
}

/*
void NextPageWidget::setDisplay(bool display)
{
    if(m_display == display)
        return ;

    QBoxLayout* layout = (QBoxLayout*)this->layout();
    if(!layout)
        return;

    if(display)
    {
        layout->insertWidget(layout->indexOf(m_nextPageBtn), m_value);
    }
    else
    {
        layout->removeWidget(m_value);
        m_value->setParent(NULL);
    }
    m_display = display;
}

bool NextPageWidget::display() const
{
    return m_display;
}
*/
