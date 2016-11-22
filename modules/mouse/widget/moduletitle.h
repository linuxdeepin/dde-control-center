#ifndef MODULETITLE_H
#define MODULETITLE_H

#include "settingsitem.h"

#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
using namespace dcc;
class ModuleTitle : public SettingsItem
{
    Q_OBJECT
public:
    explicit ModuleTitle(const QString &title, QObject *parent = 0);
    ~ModuleTitle();

signals:

public slots:


private:
    QVBoxLayout *m_mainLayout;
    QLabel      *m_title;
};

#endif // MODULETITLE_H
