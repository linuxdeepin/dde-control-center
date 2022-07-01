/*
* Copyright (C) 2019 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     wubowen <wubowen@uniontech.com>
*
* Maintainer: wubowen <wubowen@uniontech.com>
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
#include "window/utils.h"
#include "numberformat.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/tipslabel.h"
#include "modules/datetime/datetimemodel.h"
#include "window/dconfigwatcher.h"

#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QComboBox>

using namespace dcc::datetime;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::datetime;

NumberFormat::NumberFormat(dcc::datetime::DatetimeModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_layout(new QVBoxLayout)
    , m_decimalSymbolCbx(new ComboxWidget)
    , m_digitGroupingSymbolCbx(new ComboxWidget)
    , m_digitGroupingCbx(new ComboxWidget)
    , m_exampleTips(new TipsLabel)
    , m_currencySymbolFormat("")
    , m_positiveCurrencyFormat(0)
    , m_negativeCurrency(0)
{
    TitleLabel *headTitle = new TitleLabel(tr("Number Format"));     //数字格式
    m_layout->setSpacing(10);
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addWidget(headTitle, 0, Qt::AlignLeft);

    SettingsGroup *timeGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    timeGrp->layout()->setContentsMargins(0, 0, 0, 0);

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_decimalSymbolCbx->setTitle(tr("Decimal Symbol"));  //小数点

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_digitGroupingSymbolCbx->setTitle(tr("Digit Grouping Symbol"));  //分隔符

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_digitGroupingCbx->setTitle(tr("Digit Grouping"));  //数字分组

    timeGrp->appendItem(m_decimalSymbolCbx);
    timeGrp->appendItem(m_digitGroupingSymbolCbx);
    timeGrp->appendItem(m_digitGroupingCbx);
    m_layout->addWidget(timeGrp);

    //待根据事件值改默认值
    m_exampleTips->setText(tr("Example") + ":" + "   ¥123,456,789.00   ¥-123,456,789.00");
    m_exampleTips->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_exampleTips, DFontSizeManager::T8);
    m_exampleTips->setForegroundRole(DPalette::BrightText);
    m_exampleTips->setEnabled(false);
    m_layout->addWidget(m_exampleTips);
    m_layout->addStretch(0);

    initComboxWidgetList();
    setLayout(m_layout);

    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingDecimalsymbol", m_decimalSymbolCbx);
    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingDigitgroupingsymbol", m_digitGroupingSymbolCbx);
    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingDigitgrouping", m_digitGroupingCbx);

    QStringList cbxList;
    cbxList << "datetimeFromatsettingDecimalsymbol"
            << "datetimeFromatsettingDigitgroupingsymbol"
            << "datetimeFromatsettingDigitgrouping";
    auto func_is_visible = [headTitle, this]() {
        const QString dsgDecimalsymbol = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingDecimalsymbol");
        const QString dsgDigitgroupingsymbol = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingDigitgroupingsymbol");
        const QString dsgDigitgrouping = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingDigitgrouping");
        bool isHeadVisible = dsgDecimalsymbol == "Hidden" && dsgDigitgroupingsymbol == "Hidden" && dsgDigitgrouping == "Hidden";
        headTitle->setVisible(!isHeadVisible);
        m_exampleTips->setVisible(!isHeadVisible);
    };
    connect(DConfigWatcher::instance(), &DConfigWatcher::requestUpdateSearchMenu, this, [=](const QString &moduleName, bool status) {
        Q_UNUSED(status)
        if (moduleName == "" || !cbxList.contains(moduleName)) {
            return;
        }
        func_is_visible();
    });
    func_is_visible();
}

NumberFormat::~NumberFormat()
{
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "FromatsettingDecimalsymbol");
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "FromatsettingDigitgroupingsymbol");
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "FromatsettingDigitgrouping");
}

void NumberFormat::SetCurrencySymbolFormat(QString value)
{
    if (m_currencySymbolFormat != value) {
        m_currencySymbolFormat = value;
        updateExample();
    }
}

void NumberFormat::SetPositiveCurrencyFormat(int value)
{
    if (m_positiveCurrencyFormat != value) {
        m_positiveCurrencyFormat = value;
        onComboxChanged();
    }
}

void NumberFormat::SetNegativeCurrency(int value)
{
    if (m_negativeCurrency != value) {
        m_negativeCurrency = value;
        updateExample();
    }
}

void NumberFormat::initComboxWidgetList()
{
    m_decimalSymbolCbx->comboBox()->addItems(QStringList()
                                             << QString(".")
                                             << QString(",")
                                             << QString("'")
                                             << QString(tr("Space")));
    m_digitGroupingSymbolCbx->comboBox()->addItems(QStringList()
                                                   << QString(".")
                                                   << QString(",")
                                                   << QString("'")
                                                   << QString(tr("Space")));
    //根据后端的值调整显示
    m_decimalSymbolCbx->comboBox()->setCurrentText(m_model->decimalSymbol());
    m_digitGroupingSymbolCbx->comboBox()->setCurrentText(m_model->digitGroupingSymbol());

    //需要根据实际数据进行调整初始值
    QString digitGroupingSymbol = m_model->digitGroupingSymbol() == tr("Space") ? " " : m_model->digitGroupingSymbol();
    QStringList digitGroupingList = QStringList()
                            << QString("123456789")
                            << QString("%1%2%3%4%5")
                               .arg(QString("123")).arg(digitGroupingSymbol)
                               .arg(QString("456")).arg(digitGroupingSymbol)
                               .arg(QString("789"))
                            << QString("%1%2%3")
                               .arg(QString("123456")).arg(digitGroupingSymbol)
                               .arg(QString("789"))
                            << QString("%1%2%3%4%5%6%7")
                               .arg(QString("12")).arg(digitGroupingSymbol)
                               .arg(QString("34")).arg(digitGroupingSymbol)
                               .arg(QString("56")).arg(digitGroupingSymbol)
                               .arg(QString("789"));
    m_digitGroupingCbx->comboBox()->addItems(digitGroupingList);

    //根据后端的值调整显示
    m_digitGroupingCbx->comboBox()->setCurrentText(m_model->digitGrouping());

    //from widget set to work interface
    connect(m_decimalSymbolCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (m_model->decimalSymbol() != m_decimalSymbolCbx->comboBox()->currentText() && index >= 0) {
            m_model->setFormatFormWidget("DecimalSymbol", m_decimalSymbolCbx->comboBox()->currentText(), __LINE__);
        }
    });
    connect(m_digitGroupingSymbolCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (m_model->digitGroupingSymbol() != m_digitGroupingSymbolCbx->comboBox()->currentText() && index >= 0) {
            m_model->setFormatFormWidget("DigitGroupingSymbol", m_digitGroupingSymbolCbx->comboBox()->currentText(), __LINE__);
        }
    });
    connect(m_digitGroupingCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (m_model->digitGrouping() != m_digitGroupingCbx->comboBox()->currentText() && index >= 0) {
            m_model->setFormatFormWidget("DigitGrouping", m_digitGroupingCbx->comboBox()->currentText(), __LINE__);
        }
    });

    //get interface data set to widget
    connect(m_model, &DatetimeModel::DecimalSymbolChanged, this, [this](const QString &value) {
        if (m_decimalSymbolCbx && m_decimalSymbolCbx->comboBox()) {
            m_decimalSymbolCbx->comboBox()->setCurrentText(value);
            Q_EMIT decimalSymbolChanged(value);
            updateExample();
        }
    });
    connect(m_model, &DatetimeModel::DigitGroupingSymbolChanged, this, [this](const QString &value) {
        if (m_digitGroupingSymbolCbx && m_digitGroupingSymbolCbx->comboBox()) {
            m_digitGroupingSymbolCbx->comboBox()->setCurrentText(value);
            onComboxChanged();
        }
    });
    connect(m_model, &DatetimeModel::DigitGroupingChanged, this, [this](const QString &value) {
        if (m_digitGroupingCbx && m_digitGroupingCbx->comboBox()) {
            m_digitGroupingCbx->comboBox()->setCurrentText(value);
            updateExample();
        }
    });

    connect(m_model, &DatetimeModel::PositiveCurrencyFormatChanged, this, [this]() {
        updateExample();
    });
    connect(m_model, &DatetimeModel::NegativeCurrencyFormatChanged, this, [this]() {
        updateExample();
    });
}

void NumberFormat::onComboxChanged()
{
    //切换小数点(decimalSymbol)，更新示例
    //切换分隔符(digitGroupingSymbol)，更新数字分组和示例
    //获取分隔符
    QString digitGroupingSymbol = m_model->digitGroupingSymbol() == tr("Space") ? " " : m_model->digitGroupingSymbol();
    int place = m_digitGroupingCbx->comboBox()->currentIndex();//0
    //更新数字分组列表
    QStringList digitGroupingSymbolList;
    digitGroupingSymbolList << QString("123456789")
                            << QString("%1%2%3%4%5")
                               .arg(QString("123")).arg(digitGroupingSymbol)
                               .arg(QString("456")).arg(digitGroupingSymbol)
                               .arg(QString("789"))
                            << QString("%1%2%3")
                               .arg(QString("123456")).arg(digitGroupingSymbol)
                               .arg(QString("789"))
                            << QString("%1%2%3%4%5%6%7")
                               .arg(QString("12")).arg(digitGroupingSymbol)
                               .arg(QString("34")).arg(digitGroupingSymbol)
                               .arg(QString("56")).arg(digitGroupingSymbol)
                               .arg(QString("789"));

    m_digitGroupingCbx->comboBox()->clear();
    m_digitGroupingCbx->comboBox()->addItems(digitGroupingSymbolList);
    if (digitGroupingSymbolList.at(place) != m_model->digitGroupingSymbol()) {
        m_model->setFormatFormWidget("DigitGrouping", digitGroupingSymbolList.at(place), __LINE__);
        updateExample(place);
    }
}

void NumberFormat::updateExample(int numplace)
{
    if (numplace > -1) {
        m_digitGroupingCbx->comboBox()->setCurrentIndex(numplace);
    }

    qInfo() << "[updateExample], digit Grouping currentText: " << m_digitGroupingCbx->comboBox()->currentText() << m_model->digitGrouping();
    //数字部分，如：123,456,789.00
    QString numberData = QString("%1%2%3")
            .arg(m_digitGroupingCbx->comboBox()->currentText()/*m_model->digitGrouping()*/)
            .arg(m_model->decimalSymbol() == tr("Space") ? " " : m_model->decimalSymbol())
            .arg("00");

    //数字正数格式
    QString positiveCurrencyFormat = m_model->positiveCurrencyFormat();
    QString currencyFormat = m_model->currencySymbol();
    switch (m_positiveCurrencyFormat) {
    case 0://¥1.1
        positiveCurrencyFormat = QString("%1%2").arg(currencyFormat).arg(numberData);
        break;
    case 1://1.1¥
        positiveCurrencyFormat = QString("%1%2").arg(numberData).arg(currencyFormat);
        break;
    case 2://¥ 1.1
        positiveCurrencyFormat = QString("%1 %2").arg(currencyFormat).arg(numberData);
        break;
    case 3://1.1 ¥
        positiveCurrencyFormat = QString("%1 %2").arg(numberData).arg(currencyFormat);
        break;
    default:
        break;
    }

    //数字负数格式
    QString negativeCurrency = m_model->negativeCurrencyFormat();
    switch (m_negativeCurrency) {
    case 0://-¥1.1
        negativeCurrency = QString("%1%2%3").arg("-").arg(currencyFormat).arg(numberData);
        break;
    case 1://¥-1.1
        negativeCurrency = QString("%1%2%3").arg(currencyFormat).arg("-").arg(numberData);
        break;
    case 2://¥1.1-
        negativeCurrency = QString("%1%2%3").arg(currencyFormat).arg(numberData).arg("-");
        break;
    case 3://-1.1¥
        negativeCurrency = QString("%1%2%3").arg("-").arg(numberData).arg(currencyFormat);
        break;
    case 4://1.1-¥
        negativeCurrency = QString("%1%2%3").arg(numberData).arg("-").arg(currencyFormat);
        break;
    case 5://1.1¥-
        negativeCurrency = QString("%1%2%3").arg(numberData).arg(currencyFormat).arg("-");
        break;
    default:
        break;
    }

    QString ret = QString("%1%2  %3  %4").arg(tr("Example")).arg(":").arg(positiveCurrencyFormat).arg(negativeCurrency);
    qInfo() << " updateExample ret : " << ret;
    m_exampleTips->setText(ret);
}