#include "useroptionitem.h"

#include <QLabel>
#include <QDebug>
#include <QUrl>
#include <QHBoxLayout>
#include <QPainter>

using namespace dcc::widgets;
using namespace dcc::accounts;

UserOptionItem::UserOptionItem(QFrame *parent)
    : widgets::NextPageWidget(parent),

      m_avatarLabel(new QLabel)
{
    m_avatarLabel->setFixedSize(24, 24);

    QHBoxLayout *mainLayout = static_cast<QHBoxLayout *>(layout());
    mainLayout->insertWidget(0, m_avatarLabel);
    mainLayout->setSpacing(10);
}

void UserOptionItem::setAvatar(const QString &avatar)
{
    QPainterPath painterPath;
    painterPath.addEllipse(QRect(0, 0, 24, 24));

    QUrl url(avatar);
    const QPixmap pixmap = QPixmap(url.toLocalFile()).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap pic(24, 24);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(QRect(0, 0, 24, 24), pixmap);

    m_avatarLabel->setPixmap(pic);
}
