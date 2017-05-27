#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    explicit IconButton(QWidget *parent = 0);
    void setNormalIcon(const QIcon &icon);
    void setHoverIcon(const QIcon &icon);
    void setPressIcon(const QIcon &icon);

protected:
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

private:
    QIcon m_normal, m_hover, m_press;
};

#endif // ICONBUTTON_H
