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
      m_circle(0),
      m_multiple(false)
{
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(20, 0, 10, 0);
    m_title = new QLabel();
    m_checkBtn = new DImageButton();

    hlayout->setSpacing(1);
    hlayout->addWidget(m_title);
    hlayout->setAlignment(m_title, Qt::AlignVCenter);
    hlayout->addStretch();
    hlayout->addWidget(m_checkBtn);
    hlayout->addSpacerItem(new QSpacerItem(10,36));
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
        m_checkBtn->setNormalPic(":/keyboard/themes/dark/icons/select.png");
        m_checkBtn->setHoverPic(":/keyboard/themes/dark/icons/select.png");
        m_checkBtn->setPressPic(":/keyboard/themes/dark/icons/select.png");
        m_checkBtn->show();
    }
    else
        m_checkBtn->hide();
}

void CheckItem::setMultipleMode(bool multiple)
{
    m_multiple = multiple;
}

bool CheckItem::checked() const
{
    return m_checked;
}

void CheckItem::onEditMode(bool value)
{
    if(!m_checked)
    {
        if(value)
        {
            m_checkBtn->setNormalPic(":/keyboard/themes/dark/icons/list_delete_normal.png");
            m_checkBtn->setHoverPic(":/keyboard/themes/dark/icons/list_delete_hover.png");
            m_checkBtn->setPressPic(":/keyboard/themes/dark/icons/list_delete_press.png");
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
    }

    if (m_multiple) {
        setChecked(!m_checked);
        emit checkedChanged(m_title->text());
        return;
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

    if (m_multiple) {
        setChecked(!m_checked);
        emit checkedChanged(m_title->text());
        return;
    }

    if(m_circle != 1)
    {
        m_circle = 0;
        return;
    }

    setChecked(true);
    m_circle = 0;

    emit checkedChanged(m_title->text());
}
}
}
