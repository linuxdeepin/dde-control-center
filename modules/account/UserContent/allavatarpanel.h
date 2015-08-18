#ifndef ALLAVATARPANEL_H
#define ALLAVATARPANEL_H

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>

class AllAvatarPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AllAvatarPanel(QWidget *parent = 0);
    void setIconList(const QStringList &iconList);

signals:
    void selectedAvatar(const QString &path);

};

#endif // ALLAVATARPANEL_H
