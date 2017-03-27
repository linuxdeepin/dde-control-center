#include "nextpagewidget.h"

#include <QHBoxLayout>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

NextPageWidget::NextPageWidget(QFrame *parent)
    : SettingsItem(parent),
      m_title(new NormalLabel),
      m_value(new TipsLabel),
      m_nextPageBtn(new dcc::widgets::NextButton)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(20, 0, 10, 0);
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_value);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(m_nextPageBtn);

    setFixedHeight(36);
    setLayout(mainLayout);
    setObjectName("NextPageWidget");

    connect(m_nextPageBtn, &widgets::NextButton::clicked, this, &NextPageWidget::acceptNextPage);
    connect(m_nextPageBtn, &widgets::NextButton::clicked, this, &NextPageWidget::clicked);
}

void NextPageWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void NextPageWidget::setValue(const QString &value)
{
    m_value->setText(value);
}

void NextPageWidget::setIcon(const QPixmap &icon)
{
    m_value->setPixmap(icon);
}

void NextPageWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    emit selected();
    emit clicked();
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

}
}
