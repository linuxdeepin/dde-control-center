#ifndef WEBCAMAVATARPANEL_H
#define WEBCAMAVATARPANEL_H

#include <QWidget>

class WebcamAvatarPanel : public QWidget
{
    Q_OBJECT
public:
    explicit WebcamAvatarPanel(QWidget *parent = 0);

signals:
    void selectedAvatar(const QString &path);

public slots:
};

#endif // WEBCAMAVATARPANEL_H
