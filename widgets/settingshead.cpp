#include "settingshead.h"
#include <QHBoxLayout>
#include <QMouseEvent>

namespace dcc {
namespace widgets {

SettingsHead::SettingsHead(QFrame *parent)
    :SettingsItem(parent)
{
    setMouseTracking(true);
    setFixedHeight(36);
    m_title = new QLabel;
    m_edit = new QLabel;
    m_edit->setObjectName("Edit");
    m_edit->setAlignment(Qt::AlignCenter);
    m_edit->installEventFilter(this);
    m_edit->setText(tr("Edit"));
    m_edit->setMouseTracking(true);

    QFont font = m_edit->font();
    QFontMetrics fm(font);
    m_edit->setFixedWidth(fm.width(tr("Cancel")));
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_edit);

    setLayout(mainLayout);
}

void SettingsHead::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SettingsHead::setEditEnable(bool state)
{
    m_edit->setVisible(state);
}

void SettingsHead::initStatus()
{
    emit editChanged(false);
    m_edit->setText(tr("Edit"));
}

void SettingsHead::mouseMoveEvent(QMouseEvent *e)
{
    if(!m_edit->rect().contains(e->pos()))
    {
        setCursor(Qt::ArrowCursor);
    }
}

// FIXME:
bool SettingsHead::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_edit)
    {
        if(event->type() == QEvent::MouseMove)
        {
            QMouseEvent* e = static_cast<QMouseEvent *>(event);
            if(m_edit->rect().contains(e->pos()))
            {
                m_edit->setCursor(QCursor(Qt::PointingHandCursor));
                return true;
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* e = static_cast<QMouseEvent *>(event);
            if(m_edit->rect().contains(e->pos()))
            {
                QString text = m_edit->text() == "Edit" ? "Cancel" : "Edit";
                m_edit->setText(text);
                emit editChanged(text=="Cancel");
                return true;
            }
        }
    }
    return false;
}

TimeButton::TimeButton(QFrame *parent)
    :SettingsItem(parent)
{
    m_confirm = new QPushButton(tr("Confirm"));
    m_cancel = new QPushButton(tr("Cancel"));

    QHBoxLayout *layout =new QHBoxLayout();
    layout->setSpacing(1);
    layout->addWidget(m_cancel);
    layout->addWidget(m_confirm);

    setLayout(layout);

    connect(m_confirm, SIGNAL(clicked()), this, SIGNAL(confirm()));
    connect(m_cancel, SIGNAL(clicked()), this, SIGNAL(cancel()));
}

}
}
