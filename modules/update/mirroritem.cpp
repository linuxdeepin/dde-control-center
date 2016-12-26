#include "mirroritem.h"
#include <QHBoxLayout>
#include <QProcess>

namespace dcc{
namespace update{
MirrorItem::MirrorItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setObjectName("DCC-Update-MirrorItem-SelectedBtn");
    m_selectedBtn->setVisible(false);
    m_mirrorName = new QLabel;
    m_mirrorSpeed = new QLabel;

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_mirrorName);
    layout->addStretch();
    layout->addWidget(m_selectedBtn);
    layout->addWidget(m_mirrorSpeed);

    setLayout(layout);
    setFixedHeight(45);
}

void MirrorItem::setMirrorInfo(const MirrorInfo &info)
{
    m_info = info;
    setMirrorName(info.m_name);
}

void MirrorItem::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    emit selectStateChanged(state);
}

void MirrorItem::setSpeed(const int time)
{
    if (time == -1)
        m_mirrorSpeed->setText(tr("Timeout"));
    else if (time > 2000)
        m_mirrorSpeed->setText(tr("Slow"));
    else if (time > 200)
        m_mirrorSpeed->setText(tr("Medium"));
    else
        m_mirrorSpeed->setText(tr("Fast"));
}

QSize MirrorItem::sizeHint() const
{
    return QSize(width(), height());
}

void MirrorItem::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked(this);

    QFrame::mouseReleaseEvent(e);
}

}
}
