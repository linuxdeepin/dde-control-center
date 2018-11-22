/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PERSONALIZATIONMODULE_H
#define PERSONALIZATIONMODULE_H

#include "interfaces/moduleinterface.h"

namespace dcc
{
namespace personalization
{
class PersonalizationWidget;
class PersonalizationModel;
class PersonalizationWork;
class PersonalizationModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit PersonalizationModule(FrameProxyInterface *frame, QObject *parent = 0);
    virtual ~PersonalizationModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

private:
    void showThemeWidget();
    void showFontsWidget();
    void showStanardFontsListWidget();
    void showMonoFontsListWidget();

private:
    void contentPopped(ContentWidget *const w);

private:
    PersonalizationWidget *m_personalizationWidget;
    PersonalizationModel  *m_model;
    PersonalizationWork   *m_work;
};
}
}


#endif // PERSONALIZATIONMODULE_H
