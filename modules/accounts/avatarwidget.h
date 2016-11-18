#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QWidget>
#include <QPushButton>

class AvatarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AvatarWidget(const QString &avatar, QWidget *parent = 0);

    void setSelected(const bool selected = true);
    void setDeletable(const bool deletable = true);

protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    bool m_hover;
    bool m_deleable;
    bool m_selected;

    QPixmap m_avatar;

    QPushButton *m_delBtn;
};

#endif // AVATARWIDGET_H
