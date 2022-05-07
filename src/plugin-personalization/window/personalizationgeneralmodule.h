/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef PERSONALIZATIONGENERALMODULE_H
#define PERSONALIZATIONGENERALMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class SwitchWidget;
class TitledSliderItem;
class ComboxWidget;
DCC_END_NAMESPACE
class PersonalizationModel;
class PersonalizationWorker;
class RoundColorWidget;

class RingColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RingColorWidget(QWidget *parent = nullptr) : m_selectedItem(nullptr)
    {
        setAccessibleName("RingColorWidget");
    }
    virtual ~RingColorWidget() {}

    void setSelectedItem(RoundColorWidget* item)
    {
        m_selectedItem = item;
    }

    static const int EXTRA = 2; //2px extra space to avoid line cutted off

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    RoundColorWidget* m_selectedItem;
};

class PersonalizationGeneralModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit PersonalizationGeneralModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent = nullptr);

private:
    void initAccentColor(RingColorWidget *bgWidget);
    void initWindowEffect(DCC_NAMESPACE::SwitchWidget *wmSwitch);
    void initTransparentEffect(DCC_NAMESPACE::TitledSliderItem *transparentSlider);
    void initMiniEffect(DCC_NAMESPACE::ComboxWidget *cmbMiniEffect);
    void initRoundEffect(DCC_NAMESPACE::TitledSliderItem *winRoundSlider);

private Q_SLOTS:
    void onActiveColorClicked();
    void updateVisible(const bool is3DWm);

private:
    QList<DCC_NAMESPACE::ModuleObject *> m_modules;

    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
};

#endif // PERSONALIZATIONGENERALMODULE_H
