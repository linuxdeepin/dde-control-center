#ifndef ACCOUNTICON_H
#define ACCOUNTICON_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QBitmap>
class UserIcon : public QLabel
{
    Q_OBJECT
public:
    explicit UserIcon(QWidget *parent = 0);
    void setIcon(const QString &iconPath, const QSize &size = QSize(0, 0));
    void setIsNormal(bool normal);

    QString iconPath() const;

signals:
    void mousePress();

protected:
    void mousePressEvent(QMouseEvent *e);

private:
    QLabel *m_iconLabel = NULL;
    QString m_iconPath = "";

    const int NORMAL_ICON_SIZE = 70;
    const int BIG_ICON_SIZE = 80;
};

#endif // ACCOUNTICON_H
