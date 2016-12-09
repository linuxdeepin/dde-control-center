#include "checkitem.h"
#include <QHBoxLayout>
#include <QMouseEvent>

using namespace dcc;

namespace dcc {
namespace keyboard{

CheckItem::CheckItem(QFrame *parent)
    :SettingsItem(parent),
      m_checked(false),
      m_bEdit(false),
      m_circle(0)
{
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(15,0,20,0);
    m_title = new QLabel();
    m_checkBtn = new DImageButton();

    hlayout->setSpacing(1);
    hlayout->addWidget(m_title);
    hlayout->setAlignment(m_title, Qt::AlignVCenter);
    hlayout->addStretch();
    hlayout->addWidget(m_checkBtn);
    hlayout->setAlignment(m_checkBtn, Qt::AlignVCenter);
    setLayout(hlayout);

    setFixedHeight(36);

    connect(m_checkBtn, SIGNAL(clicked()), this, SLOT(onClick()));
}

void CheckItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

QString CheckItem::title() const
{
    return m_title->text();
}

void CheckItem::setChecked(bool checked)
{
    if(m_checked == checked)
        return ;

    m_checked = checked;
    if(m_checked)
    {
        m_checkBtn->setNormalPic(":/keyboard/icon/select.png");
        m_checkBtn->setHoverPic(":/keyboard/icon/select.png");
        m_checkBtn->setPressPic(":/keyboard/icon/select.png");
        m_checkBtn->show();
    }
    else
        m_checkBtn->hide();

    emit checkedChanged(m_checked);
}

void CheckItem::onEditMode(bool value)
{
    if(!m_checked)
    {
        if(value)
        {
            m_checkBtn->setNormalPic(":/keyboard/icon/list_delete_normal.png");
            m_checkBtn->setHoverPic(":/keyboard/icon/list_delete_hover.png");
            m_checkBtn->setPressPic(":/keyboard/icon/list_delete_press.png");
            m_checkBtn->show();
        }
        else
        {
            m_checkBtn->hide();
        }
    }

    m_bEdit = value;
}

void CheckItem::onClick()
{
    if(!m_checked)
    {
        emit destroySelf(this);
        this->deleteLater();
    }
}

void CheckItem::mousePressEvent(QMouseEvent *)
{
    if(m_bEdit)
        return;
    m_circle++;
}

void CheckItem::mouseReleaseEvent(QMouseEvent *)
{
    if(m_bEdit)
        return;

    if(m_circle != 1)
    {
        m_circle = 0;
        return;
    }

    setChecked(true);
    m_circle = 0;
}
}
}
