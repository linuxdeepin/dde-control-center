#ifndef USERAVATAR_H
#define USERAVATAR_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
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

class UserAvatar : public QLabel
{
    Q_OBJECT
public:
    explicit UserAvatar(QWidget *parent = 0, bool deleteable = false);
    void setIcon(const QString &iconPath, const QSize &size = QSize(0, 0));
    void setIsNormal(bool normal);

    QString iconPath() const;

signals:
    void mousePress();
    void requestDelete();

protected:
    void mousePressEvent(QMouseEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    void initDeleteButton();

private:
    AvatarDeleteButton *m_deleteButton = NULL;
    QLabel *m_iconLabel = NULL;
    QString m_iconPath = "";

    const int NORMAL_ICON_SIZE = 70;
    const int BIG_ICON_SIZE = 80;
};


#endif // USERAVATAR_H
