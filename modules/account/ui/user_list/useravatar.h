#ifndef USERAVATAR_H
#define USERAVATAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QBitmap>
#include <QPainter>
#include <QDebug>
#include "QPushButton"
#include "libdui/dimagebutton.h"

DUI_USE_NAMESPACE

class AvatarDeleteButton : public DImageButton
{
    Q_OBJECT
public:
    AvatarDeleteButton(QWidget *parent = 0);
};

class UserAvatar : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor borderSelectedColor READ borderSelectedColor WRITE setBorderSelectedColor)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
public:
    explicit UserAvatar(QWidget *parent = 0, bool deleteable = false);
    void setIcon(const QString &iconPath, const QSize &size = QSize(0, 0));
    void setIsNormal(bool normal);

    QString iconPath() const;

    QColor borderSelectedColor() const;
    void setBorderSelectedColor(const QColor &borderSelectedColor);

    QColor borderColor() const;
    void setBorderColor(const QColor &borderColor);

    int borderWidth() const;
    void setBorderWidth(int borderWidth);

    void setSelected(bool selected);

signals:
    void mousePress();
    void requestDelete();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *event);

private:
    void initDeleteButton();

private:
    AvatarDeleteButton *m_deleteButton = NULL;
    QLabel *m_iconLabel = NULL;
    QString m_iconPath = "";
    QColor m_borderColor;
    QColor m_borderSelectedColor;
    int m_borderWidth = 5;
    bool m_isNormal = true;
    bool m_selected = false;

    const int NORMAL_ICON_SIZE = 70;
    const int BIG_ICON_SIZE = 80;
};


#endif // USERAVATAR_H
