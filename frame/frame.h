#ifndef FRAME_H
#define FRAME_H

#include "pluginscontroller.h"

#include <QMainWindow>

class Frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);

private:
    PluginsController *m_pluginsController;
};

#endif // FRAME_H
