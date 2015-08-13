#ifndef SOUNDCONTENT_H
#define SOUNDCONTENT_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>
#include "appframe.h"
#include "deviceframe.h"
#include "dde-dock/dockpluginproxyinterface.h"

class SoundContent : public QWidget
{
    Q_OBJECT
public:
    explicit SoundContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent = 0);

signals:

public slots:

private:
    void initStyleSheet();
    void initFrame();

private:
    QString m_id = "";
    DockPluginProxyInterface * m_proxy = NULL;

};

#endif // SOUNDCONTENT_H
