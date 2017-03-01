#include "settingshead.h"
#include <QHBoxLayout>
#include <QMouseEvent>

#include "labels/normallabel.h"

namespace dcc {
namespace widgets {

SettingsHead::SettingsHead(QFrame *parent)
    : SettingsItem(parent),

      m_title(new NormalLabel),
      m_edit(new QPushButton),
      m_cancel(new QPushButton),

      m_state(Cancel)
{
    m_title->setObjectName("SettingsHeadTitle");

    m_edit->setFlat(true);
    m_cancel->setFlat(true);

    m_edit->setText(tr("Edit"));
    m_edit->setVisible(false);

    m_cancel->setText(tr("Cancel"));
    m_cancel->setVisible(false);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 0, 10, 0);
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_edit);
    mainLayout->addWidget(m_cancel);

    setFixedHeight(24);
    setLayout(mainLayout);

    connect(m_edit, &QPushButton::clicked, this, &SettingsHead::toEdit);
    connect(m_cancel, &QPushButton::clicked, this, &SettingsHead::toCancel);
}

void SettingsHead::setTitle(const QString &title)
{
    m_title->setText(title);
    m_edit->setAccessibleName(title);
}

void SettingsHead::setEditEnable(bool state)
{
    // reset state
    toCancel();

    m_edit->setVisible(state && m_state == Cancel);
    m_cancel->setVisible(state && m_state == Edit);
}

void SettingsHead::toEdit()
{
    m_state = Edit;
    refershButton();

    emit editChanged(true);
}

void SettingsHead::toCancel()
{
    m_state = Cancel;
    refershButton();

    emit editChanged(false);
}

void SettingsHead::refershButton()
{
    m_edit->setVisible(m_state == Cancel);
    m_cancel->setVisible(m_state == Edit);
}

}

}
