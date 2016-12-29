#include "optionitem.h"
#include "translucentframe.h"

namespace dcc {
namespace widgets {

OptionItem::OptionItem(QWidget *parent) :
    OptionItem("", parent)
{

}

OptionItem::OptionItem(QString title, bool selected, QWidget *parent)
    : SettingsItem(parent),
      m_title(title),
      m_selected(selected),

      m_mainLayout(new QVBoxLayout),
      m_titleLayout(new QHBoxLayout),
      m_titleWidget(new NormalLabel(title)),
      m_contentWidget(nullptr),
      m_selectedIcon(new QLabel)
{
    m_titleWidget->setObjectName("OptionItemTitle");
    m_selectedIcon->setObjectName("OptionItemSelectedIcon");
    m_selectedIcon->setVisible(m_selected);
    m_selectedIcon->setFixedSize(16, 16);

    m_titleFrame = new TranslucentFrame;
    m_titleFrame->setFixedHeight(36);
    m_titleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_titleFrame->setLayout(m_titleLayout);

    m_titleLayout->setSpacing(0);
    m_titleLayout->setMargin(0);
    m_titleLayout->addWidget(m_titleWidget);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_selectedIcon);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_titleFrame);

    setLayout(m_mainLayout);
}

void OptionItem::setTitle(const QString &title)
{
    m_title = title;

    if (m_titleWidget) {
        NormalLabel *label = qobject_cast<NormalLabel*>(m_titleWidget);
        if (label) label->setText(m_title);
    }

    setAccessibleName(m_title);
}

void OptionItem::setTitleFixedHeight(int height)
{
    m_titleFrame->setFixedHeight(height);
}

void OptionItem::setTitleWidget(QWidget *titleWidget)
{
    if (m_titleWidget) {
        m_titleLayout->removeWidget(m_titleWidget);
        m_titleWidget->deleteLater();
    }

    m_titleLayout->insertWidget(0, titleWidget);
    m_titleWidget = titleWidget;
}

void OptionItem::setContentWidget(QWidget *contentWidget)
{
    if (m_contentWidget) {
        m_mainLayout->removeWidget(m_contentWidget);
        m_contentWidget->deleteLater();
    }

    m_mainLayout->addWidget(contentWidget);
    m_contentWidget = contentWidget;
}

void OptionItem::setContentsMargins(int left, int top, int right, int bottom)
{
    m_mainLayout->setContentsMargins(left, top, right, bottom);
}

void OptionItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    setSelected(true);
}

void OptionItem::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        m_selectedIcon->setVisible(selected);

        emit selectedChanged(selected);
    }
}


}
}
