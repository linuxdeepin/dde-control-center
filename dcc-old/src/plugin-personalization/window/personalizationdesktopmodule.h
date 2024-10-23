//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONDESKTOPMODULE_H
#define PERSONALIZATIONDESKTOPMODULE_H

#include "interface/pagemodule.h"

class PersonalizationModel;
class PersonalizationWorker;

class PersonalizationDesktopModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit PersonalizationDesktopModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent = nullptr);

private:
    QWidget *initWindowEffect(ModuleObject *module);
    QWidget *initTransparentEffect(ModuleObject *module);
    QWidget *initMiniEffect(ModuleObject *module);
    QWidget *initRoundEffect(ModuleObject *module);

    QWidget *initCompactMode(ModuleObject *module);

private:
    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
};

#endif // PERSONALIZATIONDESKTOPMODULE_H
