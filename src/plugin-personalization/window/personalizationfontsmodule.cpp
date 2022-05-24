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
#include "personalizationfontsmodule.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "model/fontsizemodel.h"

#include <widgets/titledslideritem.h>
#include <widgets/widgetmodule.h>
#include <widgets/dccslider.h>
#include <widgets/comboxwidget.h>

#include <QJsonObject>
#include <QStandardItem>

#include <model/fontmodel.h>

//#include <DSlider>
//#include <QFont>

DCC_USE_NAMESPACE
//DWIDGET_USE_NAMESPACE
//DCORE_USE_NAMESPACE

PersonalizationFontsModule::PersonalizationFontsModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent)
    : ModuleObject("personalizationFont", tr("Font"), QIcon::fromTheme("dcc_Font"), parent)
    , m_standardModel(new QStandardItemModel(this))
    , m_monospacedModel(new QStandardItemModel(this))
    , m_model(model)
    , m_work(work)
{
    setChildType(ModuleObject::Page);

    setStandList(m_model->getStandFontModel()->getFontList());
    connect(m_model->getStandFontModel(), &FontModel::listChanged, this, &PersonalizationFontsModule::setStandList);

    setMonoList(m_model->getMonoFontModel()->getFontList());
    connect(m_model->getMonoFontModel(), &FontModel::listChanged, this, &PersonalizationFontsModule::setMonoList);

    appendChild(new WidgetModule<TitledSliderItem>("fontsizeslider", tr("Size"), [this](TitledSliderItem *fontSizeSlider) {
        fontSizeSlider->setTitle(tr("Size"));
        fontSizeSlider->addBackground();
        fontSizeSlider->setObjectName("fontsizeslider");
        QStringList annotions;
        annotions << "11"
                  << "12"
                  << "13"
                  << "14"
                  << "15"
                  << "16"
                  << "18"
                  << "20";
        fontSizeSlider->setAnnotations(annotions);

        DCCSlider *slider = fontSizeSlider->slider();
        slider->setOrientation(Qt::Horizontal);
        slider->setRange(0, annotions.size() - 1);
        slider->setType(DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(1);
        slider->setPageStep(1);
        auto fontSizeChanged = [fontSizeSlider](int fontSize) {
            fontSizeSlider->slider()->blockSignals(true);
            fontSizeSlider->slider()->setValue(fontSize);
            fontSizeSlider->slider()->blockSignals(false);
        };
        fontSizeChanged(m_model->getFontSizeModel()->getFontSize());
        connect(m_model->getFontSizeModel(), &FontSizeModel::sizeChanged, fontSizeSlider, fontSizeChanged);
        connect(slider, &DCCSlider::valueChanged, m_work, &PersonalizationWorker::setFontSize);
        connect(slider, &DCCSlider::sliderMoved, m_work, &PersonalizationWorker::setFontSize);
    }));
    appendChild(new WidgetModule<ComboxWidget>("perssonalFontStandard", tr("Standard Font"), [this](ComboxWidget *standardFontsCbBox) {
        standardFontsCbBox->setTitle(tr("Standard Font"));
        standardFontsCbBox->comboBox()->setModel(m_standardModel);
        initFontWidget(standardFontsCbBox, m_model->getStandFontModel());
    }));
    appendChild(new WidgetModule<ComboxWidget>("perssonalFontMono", tr("Monospaced Font"), [this](ComboxWidget *monoFontsCbBox) {
        monoFontsCbBox->setTitle(tr("Monospaced Font"));
        monoFontsCbBox->comboBox()->setModel(m_monospacedModel);
        initFontWidget(monoFontsCbBox, m_model->getMonoFontModel());
    }));
}

void PersonalizationFontsModule::active()
{
    m_work->refreshFont();
}

void PersonalizationFontsModule::setStandList(const QList<QJsonObject> &list)
{
    setList(list, m_standardModel);
    m_model->getStandFontModel()->defaultFontChanged(m_model->getStandFontModel()->getFontName());
}

void PersonalizationFontsModule::setMonoList(const QList<QJsonObject> &list)
{
    setList(list, m_monospacedModel);
    m_model->getMonoFontModel()->defaultFontChanged(m_model->getMonoFontModel()->getFontName());
}

void PersonalizationFontsModule::setList(const QList<QJsonObject> &list, QStandardItemModel *model)
{
    model->blockSignals(true);
    model->clear();
    for (QJsonObject item : list) {
        QString name = item["Name"].toString();
        QStandardItem *modelItem = new QStandardItem(name);
        modelItem->setFont(QFont(name));
        model->appendRow(modelItem);
    }
    model->blockSignals(false);
}

void PersonalizationFontsModule::initFontWidget(ComboxWidget *combox, FontModel *fontModel)
{
    combox->addBackground();
    combox->comboBox()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto defaultFont = [combox, fontModel](const QString &name) {
        for (const QJsonObject &obj : fontModel->getFontList()) {
            if (obj["Id"].toString() == name) {
                combox->comboBox()->blockSignals(true);
                combox->comboBox()->setCurrentText(obj["Name"].toString());
                combox->comboBox()->blockSignals(false);
                return;
            }
        }
        combox->comboBox()->blockSignals(true);
        combox->comboBox()->setCurrentText(fontModel->getFontName() + tr(" (Unsupported font)"));
        combox->comboBox()->blockSignals(false);
    };
    defaultFont(fontModel->getFontName());
    connect(fontModel, &FontModel::defaultFontChanged, combox, defaultFont);
    connect(combox->comboBox(), &QComboBox::currentTextChanged, this, [fontModel, this](const QString &name) {
        for (const QJsonObject &obj : fontModel->getFontList()) {
            if (obj["Name"].toString() == name) {
                m_work->setDefault(obj);
                return;
            }
        }
    });
}
