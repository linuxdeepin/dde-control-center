#ifndef ACCOUNTICON_H
#define ACCOUNTICON_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QBitmap>
class AccountIcon : public QLabel
{
    Q_OBJECT
public:
    explicit AccountIcon(QWidget *parent = 0);
    void setIcon(const QString &iconPath, const QSize &size = QSize(0, 0));
    void setIsNormal(bool normal);

private:
    QLabel *m_iconLabel = NULL;

    const int NORMAL_ICON_SIZE = 70;
    const int BIG_ICON_SIZE = 80;
};

#endif // ACCOUNTICON_H
