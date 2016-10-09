#ifndef FRAME_H
#define FRAME_H

#include "mainwidget.h"
#include "settingswidget.h"
#include "frameproxyinterface.h"

#include <QMainWindow>
#include <QStack>

class Frame : public QMainWindow, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);

public slots:
    void pushWidget(FrameWidget * const w);
    void popWidget();

private slots:
    void init();

    void showAllSettings();

private:
    FrameWidget *m_allSettingsPage;
    QStack<FrameWidget *> m_frameWidgetStack;
};

#endif // FRAME_H
