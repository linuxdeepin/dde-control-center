#ifndef DOUTESTWIDGET_H
#define DOUTESTWIDGET_H

#include "settingsitem.h"
#include <QLabel>
#include <QObject>
using namespace dcc;
class DouTestWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit DouTestWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // DOUTESTWIDGET_H
