#ifndef WINDOWTHEME_H
#define WINDOWTHEME_H
#include "settingsitem.h"
#include "translucentframe.h"
#include <settingsgroup.h>
#include <QObject>
using namespace dcc;
class WindowTheme : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit WindowTheme(QObject *parent = 0);

signals:

public slots:
};

#endif // WINDOWTHEME_H
