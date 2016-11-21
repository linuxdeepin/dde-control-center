#include "nextpagewidget.h"

#include <QHBoxLayout>

using namespace dcc;

DWIDGET_USE_NAMESPACE

NextPageWidget::NextPageWidget(QFrame *parent)
    : SettingsItem(parent)
{
    m_title = new NormalLabel;
    m_title->setStyleSheet("color: white;"
                           "margin: 0 0 0 8px;");

    m_value = new NormalLabel;

    m_nextPageBtn = new DImageButton;
    m_nextPageBtn->setFixedSize(32, 32);
//    m_nextPageBtn->setStyleSheet("background-color:red;");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_value);
    mainLayout->addWidget(m_nextPageBtn);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    setFixedHeight(36);
    setLayout(mainLayout);
    setObjectName("NextPageWidget");

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
