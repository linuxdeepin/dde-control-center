#ifndef HISTORYAVATARPANEL_H
#define HISTORYAVATARPANEL_H

#include <QWidget>

class HistoryAvatarPanel : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryAvatarPanel(QWidget *parent = 0);
    void setIconList(const QStringList &iconList);

signals:
    void selectedAvatar(const QString &path);

public slots:
};

#endif // HISTORYAVATARPANEL_H
