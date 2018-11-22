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

#include "fontsizewidget.h"
#include "../../widget/fontitem.h"
#include "../../model/fontsizemodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

FontSizeWidget::FontSizeWidget(QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_mainWidget = new SettingsGroup;
    m_sizeWidget = new TitledSliderItem(tr("Size"));

    m_sizeWidget->slider()->setType(DCCSlider::Vernier);
    m_sizeWidget->slider()->setRange(0, 6);
    m_sizeWidget->slider()->setTickPosition(QSlider::TicksBelow);
    m_sizeWidget->slider()->setTickInterval(1);
    m_sizeWidget->slider()->setPageStep(1);
    m_sizeWidget->setObjectName("FontSizeWidget");
    m_mainWidget->appendItem(m_sizeWidget);
    m_mainlayout->addWidget(m_mainWidget);

    m_mainlayout->setSpacing(0);
    m_mainlayout->setMargin(0);

    setLayout(m_mainlayout);

    m_fontSlider = m_sizeWidget->slider();
    connect(m_fontSlider, &QSlider::valueChanged, this, &FontSizeWidget::requestSetFontSize);
    connect(m_fontSlider, &QSlider::valueChanged, this, &FontSizeWidget::setFontSizeTip);
}

void FontSizeWidget::setModel(FontSizeModel *const model)
{
    connect(model, &FontSizeModel::sizeChanged, this, &FontSizeWidget::setFontSize);
    setFontSize(model->getFontSize());
}

void FontSizeWidget::setFontSize(int size)
{
    m_fontSlider->blockSignals(true);
    m_fontSlider->setValue(size);
    m_fontSlider->blockSignals(false);
    m_sizeWidget->setValueLiteral(delayToLiteralString(size));
}

void FontSizeWidget::setFontSizeTip(int size)
{
    m_sizeWidget->setValueLiteral(delayToLiteralString(size));
}

QString FontSizeWidget::delayToLiteralString(const int delay) const
{
    switch (delay) {
    case 0:
        return "11px";
    case 1:
        return "12px";
    case 2:
        return "13px";
    case 3:
        return "15px";
    case 4:
        return "16px";
    case 5:
        return "18px";
    case 6:
        return "20px";
    default:
        return "12px";
    }
}
