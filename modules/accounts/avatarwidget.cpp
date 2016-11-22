#include "avatarwidget.h"

#include <QDebug>
#include <QUrl>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QApplication>

#define PIX_SIZE    60

AvatarWidget::AvatarWidget(const QString &avatar, QWidget *parent)
    : QWidget(parent),

    m_hover(false),
    m_deleable(false),
    m_selected(false),

    m_delBtn(new QPushButton)
{
    QUrl url(avatar);
    m_avatar = QPixmap(url.toLocalFile()).scaled(PIX_SIZE, PIX_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_delBtn->setText("x");
    m_delBtn->setVisible(false);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_delBtn);
    mainLayout->setAlignment(m_delBtn, Qt::AlignCenter);

    setLayout(mainLayout);
    setFixedSize(PIX_SIZE, PIX_SIZE);
    setObjectName("AvatarWidget");
}

void AvatarWidget::setSelected(const bool selected)
{
    m_selected = selected;
    update();
}

void AvatarWidget::setDeletable(const bool deletable)
{
    m_deleable = deletable;
    update();
}

void AvatarWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (rect().contains(e->pos()))
        emit clicked();

    QWidget::mouseReleaseEvent(e);
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    QPainterPath painterPath;
    painterPath.addEllipse(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(e->rect(), m_avatar, e->rect());

    QPen pen(Qt::transparent);
    pen.setWidth(4);
    if (m_selected)
        pen.setColor(Qt::white);

    painter.setPen(pen);
    painter.setBrush(Qt::transparent);
    painter.drawEllipse(rect());

    QWidget::paintEvent(e);
}

void AvatarWidget::enterEvent(QEvent *)
{
    m_hover = true;
    m_delBtn->setVisible(m_deleable);
    update();
}

void AvatarWidget::leaveEvent(QEvent *)
{
    m_hover = false;
    m_delBtn->setVisible(false);
    update();
}
