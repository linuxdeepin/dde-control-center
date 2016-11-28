#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"

#include "windowtheme.h"
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
using namespace dcc;
class ThemeWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QObject *parent = 0);

signals:

public slots:

private:

};

#endif // THEMEWIDGET_H
