#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QWidget>
#include <QPushButton>

class AvatarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AvatarWidget(const QString &avatar, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap m_avatar;

    QPushButton *m_delBtn;
};

#endif // AVATARWIDGET_H
