/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
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
#include "currencyformat.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "window/utils.h"
#include "modules/datetime/datetimemodel.h"
#include "window/dconfigwatcher.h"

#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QComboBox>

using namespace dcc::datetime;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::datetime;

CurrencyFormat::CurrencyFormat(dcc::datetime::DatetimeModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_layout(new QVBoxLayout)
    , m_currencySymbolCbx(new ComboxWidget)
    , m_positiveCurrencyFormatCbx(new ComboxWidget)
    , m_negativeCurrencyFormatCbx(new ComboxWidget)
{
    TitleLabel *headTitle = new TitleLabel(tr("Currency Format"));     //货币格式
    m_layout->setSpacing(10);
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addWidget(headTitle, 0, Qt::AlignLeft);

    SettingsGroup *timeGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    timeGrp->layout()->setContentsMargins(0, 0, 0, 0);

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_currencySymbolCbx->setTitle(tr("Currency Symbol"));   //货币符号

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_positiveCurrencyFormatCbx->setTitle(tr("Positive Currency Format"));  //货币正数

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_negativeCurrencyFormatCbx->setTitle(tr("Negative Currency Format"));  //货币负数

    timeGrp->appendItem(m_currencySymbolCbx);
    timeGrp->appendItem(m_positiveCurrencyFormatCbx);
    timeGrp->appendItem(m_negativeCurrencyFormatCbx);
    m_layout->addWidget(timeGrp);
    m_layout->addStretch(0);
    initComboxWidgetList();
    setLayout(m_layout);

    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingCurrencysymbol", m_currencySymbolCbx);
    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingPositive", m_positiveCurrencyFormatCbx);
    DConfigWatcher::instance()->bind(DConfigWatcher::datetime, "FromatsettingNegative", m_negativeCurrencyFormatCbx);

    QStringList cbxList;
    cbxList << "datetimeFromatsettingCurrencysymbol"
            << "datetimeFromatsettingPositive"
            << "datetimeFromatsettingNegative";
    auto func_is_visible = [headTitle]() {
        const QString dsgCurrencysymbol = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingCurrencysymbol");
        const QString dsgPositive = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingPositive");
        const QString dsgNegative = DConfigWatcher::instance()->getStatus(DConfigWatcher::datetime, "FromatsettingNegative");
        bool isHeadVisible = dsgCurrencysymbol == "Hidden" && dsgPositive == "Hidden" && dsgNegative == "Hidden";
        headTitle->setVisible(!isHeadVisible);
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

CurrencyFormat::~CurrencyFormat()
{
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "datetimeFromatsettingCurrencysymbol");
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "datetimeFromatsettingPositive");
    DConfigWatcher::instance()->erase(DConfigWatcher::datetime, "datetimeFromatsettingNegative");
}

void CurrencyFormat::initComboxWidgetList()
{
    QString currencySymbol = m_model->currencySymbol();
    m_currencySymbolCbx->comboBox()->addItems(QStringList() << QString("￥") << QString("$") << QString("€"));
    m_positiveCurrencyFormatCbx->comboBox()->addItems(QStringList()
                                                      << QString("%1%2").arg(currencySymbol).arg("1.1")
                                                      << QString("%1%2").arg("1.1").arg(currencySymbol)
                                                      << QString("%1%2").arg(currencySymbol).arg(" 1.1")
                                                      << QString("%1%2").arg("1.1 ").arg(currencySymbol));
    m_negativeCurrencyFormatCbx->comboBox()->addItems(QStringList()
                                                      << QString("-%1%2").arg(currencySymbol).arg("1.1")
                                                      << QString("%1-%2").arg(currencySymbol).arg("1.1")
                                                      << QString("%1%2-").arg(currencySymbol).arg("1.1")
                                                      << QString("-%1%2").arg("1.1").arg(currencySymbol)
                                                      << QString("%1-%2").arg("1.1").arg(currencySymbol)
                                                      << QString("%1%2-").arg("1.1").arg(currencySymbol));

    m_currencySymbolCbx->comboBox()->setCurrentText(m_model->currencySymbol());
    m_positiveCurrencyFormatCbx->comboBox()->setCurrentText(m_model->positiveCurrencyFormat());
    m_negativeCurrencyFormatCbx->comboBox()->setCurrentText(m_model->negativeCurrencyFormat());
    //初次进入该页面，获取正负正数位子
    m_currencySymbolFormatPlace = m_currencySymbolCbx->comboBox()->currentText();
    m_positiveCurrencyFormatPlace = m_positiveCurrencyFormatCbx->comboBox()->currentIndex();
    m_negativeCurrencyFormatPlace = m_negativeCurrencyFormatCbx->comboBox()->currentIndex();

    //from widget set to work interface
    connect(m_currencySymbolCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (!m_model || index < 0 || !m_currencySymbolCbx || !m_currencySymbolCbx->comboBox()) return;
        m_model->setFormatFormWidget("CurrencySymbol", m_currencySymbolCbx->comboBox()->currentText(), __LINE__);
    });
    connect(m_positiveCurrencyFormatCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (!m_model || index < 0 || !m_positiveCurrencyFormatCbx || !m_positiveCurrencyFormatCbx->comboBox()) return;
        m_model->setFormatFormWidget("PositiveCurrencyFormat", m_positiveCurrencyFormatCbx->comboBox()->currentText(), __LINE__);
    });
    connect(m_negativeCurrencyFormatCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        if (!m_model || index < 0 || !m_negativeCurrencyFormatCbx || !m_negativeCurrencyFormatCbx->comboBox()) return;
        m_model->setFormatFormWidget("NegativeCurrencyFormat", m_negativeCurrencyFormatCbx->comboBox()->currentText(), __LINE__);
    });

    //get interface data set to widget
    connect(m_model, &DatetimeModel::CurrencySymbolChanged, this, [this](const QString &value) {
        if (m_currencySymbolCbx && m_currencySymbolCbx->comboBox()) {
            m_currencySymbolCbx->comboBox()->setCurrentText(value);
            Q_EMIT currencySymbolFormatChanged(value);
        }
    });
    connect(m_model, &DatetimeModel::PositiveCurrencyFormatChanged, this, [this](const QString &value) {
        if (m_positiveCurrencyFormatCbx && m_positiveCurrencyFormatCbx->comboBox()) {
            m_positiveCurrencyFormatCbx->comboBox()->setCurrentText(value);
            Q_EMIT positiveCurrencyFormatChanged(m_positiveCurrencyFormatCbx->comboBox()->currentIndex());
        }
    });
    connect(m_model, &DatetimeModel::NegativeCurrencyFormatChanged, this, [this](const QString &value) {
        if (m_negativeCurrencyFormatCbx && m_negativeCurrencyFormatCbx->comboBox()) {
            m_negativeCurrencyFormatCbx->comboBox()->setCurrentText(value);
            Q_EMIT negativeCurrencyChanged(m_negativeCurrencyFormatCbx->comboBox()->currentIndex());
        }
    });

    connect(m_model, &DatetimeModel::CurrencySymbolChanged, this, [this](QString value) {
        QString currencySymbol = value;
        QStringList currencySymbolList;
        //正数列表
        currencySymbolList << QString("%1%2").arg(currencySymbol).arg(QString("1.1"))
                           << QString("%1%2").arg(QString("1.1")).arg(currencySymbol)
                           << QString("%1 %2").arg(currencySymbol).arg(QString("1.1"))
                           << QString("%1 %2").arg(QString("1.1")).arg(currencySymbol);
        qDebug() << " DatetimeModel::CurrencySymbolChanged, currencySymbolList : " << currencySymbolList;

        int place = m_positiveCurrencyFormatCbx->comboBox()->currentIndex();
        m_positiveCurrencyFormatCbx->comboBox()->clear();
        m_positiveCurrencyFormatCbx->comboBox()->addItems(currencySymbolList);
        if (m_model->positiveCurrencyFormat() != currencySymbolList.at(place)) {
            m_model->setFormatFormWidget("PositiveCurrencyFormat", currencySymbolList.at(place), __LINE__);
        }

        //负数列表 -¥1.1、¥-1.1、¥1.1-、-1.1¥、1.1-¥和1.1¥-
        currencySymbolList.clear();
        currencySymbolList << QString("-%1%2").arg(currencySymbol).arg(QString("1.1"))
                           << QString("%1-%2").arg(currencySymbol).arg(QString("1.1"))
                           << QString("%1%2-").arg(currencySymbol).arg(QString("1.1"))
                           << QString("-%1%2").arg(QString("1.1")).arg(currencySymbol)
                           << QString("%1-%2").arg(QString("1.1")).arg(currencySymbol)
                           << QString("%1%2-").arg(QString("1.1")).arg(currencySymbol);
        qDebug() << " DatetimeModel::CurrencySymbolChanged, currencySymbolList : " << currencySymbolList;

        place = m_negativeCurrencyFormatCbx->comboBox()->currentIndex();
        m_negativeCurrencyFormatCbx->comboBox()->clear();
        m_negativeCurrencyFormatCbx->comboBox()->addItems(currencySymbolList);
        if (m_model->negativeCurrencyFormat() != currencySymbolList.at(place)) {
            m_model->setFormatFormWidget("NegativeCurrencyFormat", currencySymbolList.at(place), __LINE__);
        }
    });
}
