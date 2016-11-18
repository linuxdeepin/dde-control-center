#include "avatarwidget.h"

#include <QDebug>
#include <QUrl>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>

#define PIX_SIZE    80

AvatarWidget::AvatarWidget(const QString &avatar, QWidget *parent)
    : QWidget(parent),

    m_delBtn(new QPushButton)
{
    QUrl url(avatar);
    m_avatar = QPixmap(url.toLocalFile()).scaled(PIX_SIZE, PIX_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_delBtn->setText("x");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_delBtn);
    mainLayout->setAlignment(m_delBtn, Qt::AlignCenter);

    setLayout(mainLayout);
    setFixedSize(PIX_SIZE, PIX_SIZE);
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    QPainterPath painterPath;
    painterPath.addEllipse(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(e->rect(), m_avatar, e->rect());

    QPen pen(Qt::white);
    pen.setWidth(5);

    painter.setPen(pen);
    painter.setBrush(Qt::transparent);
    painter.drawEllipse(rect());

    QWidget::paintEvent(e);
}
