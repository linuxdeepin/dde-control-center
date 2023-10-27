// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "regionmodule.h"
#include "customregionformatdialog.h"
#include "datetimemodel.h"
#include "datetimeworker.h"
#include "widgets/widgetmodule.h"

#include <dcclistview.h>
#include <DCommandLinkButton>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

RegionModule::RegionModule(DatetimeModel *model,
                           DatetimeWorker *work,
                           QObject *parent)
    : PageModule("region", tr("Region"), parent)
    , m_model(model)
    , m_work(work)
    , m_locale(QLocale::system())
{
    if (!m_model->localeName().isEmpty())
        m_locale = QLocale(m_model->localeName());

    setNoScroll(false);

    appendChild(new ItemModule("Country", tr("Country region")));
    appendChild(new WidgetModule<DTipLabel>("CountryTip", tr(""), this, &RegionModule::initCountryTip));
    initCountryModule();
    appendChild(m_countryModule);
    appendChild(new ItemModule("regionFormat", tr("Region format")));
    appendChild(new WidgetModule<DTipLabel>("regionFormatTip", tr(""), this, &RegionModule::initRegionFormatTip));
    initLangRegionModule();
    appendChild(m_langRegionModule);
    appendChild(new WidgetModule<DCCListView>("", tr(""), [this](DListView *formatList){
        initFormatList(formatList);
    }));
    initFormatModificationModule();
    appendChild(m_formatModificationModule);

    connect(m_langRegionModule, &ItemModule::clicked, this, &RegionModule::onLangRegionClicked);
    connect(m_model, &DatetimeModel::localeNameChanged, this, [this](const QString &name){
        m_locale = QLocale(name);
    } );
}

void RegionModule::initCountryTip(DTipLabel *countryTipLabel)
{
    countryTipLabel->setWordWrap(true);
    countryTipLabel->setAlignment(Qt::AlignLeft);
    countryTipLabel->setContentsMargins(10, 0, 10, 0);
    countryTipLabel->setText(tr("Provide localized services based on your country or region."));
}

void RegionModule::initCountryModule()
{
    m_countryModule = new ItemModule("countryRegion", tr("Country/region"), [this](ModuleObject *){
        m_countryCombo = new DComboBox;
        m_countryCombo->addItems(m_model->countries());
        m_countryCombo->setCurrentText(tr(m_model->country().toUtf8()));
        connect(m_countryCombo, &QComboBox::currentTextChanged, this, [this](const QString &value) { m_work->setConfigValue(country_key, value); });
        connect(m_model, &DatetimeModel::countryChanged, this, [this](const QString &text) { m_countryCombo->setCurrentText(text); });
        return m_countryCombo;
    });
    m_countryModule->setBackground(true);
}

void RegionModule::initRegionFormatTip(DTipLabel *regionFormatTipLabel)
{
    regionFormatTipLabel->setWordWrap(true);
    regionFormatTipLabel->setAlignment(Qt::AlignLeft);
    regionFormatTipLabel->setContentsMargins(10, 0, 10, 0);
    regionFormatTipLabel->setText(tr("Set the date and time format according to language and region options."));
}

void RegionModule::initLangRegionModule()
{
    m_langRegionModule = new ItemModule("languageRegion", tr("Languange and region"), [this](ModuleObject *){
        QWidget *widget = new QWidget;
        m_langRegionLabel = new QLabel;
        m_langRegionLabel->setText(tr(m_model->langRegion().toUtf8()));
        connect(m_model, &DatetimeModel::langCountryChanged, this, [this](const QString &text){
            m_langRegionLabel->setText(tr(text.toUtf8()));
        } );
        QLabel *langRegionEnterLabel = new QLabel;
        langRegionEnterLabel->setPixmap(DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addStretch(0);
        hLayout->addWidget(m_langRegionLabel);
        hLayout->addWidget(langRegionEnterLabel);
        widget->setLayout(hLayout);
        return widget;
    });
    m_langRegionModule->setBackground(true);
    m_langRegionModule->setClickable(true);
}

void RegionModule::initFormatList(DListView *formatList)
{
    QStandardItemModel* viewModel = new QStandardItemModel;
    formatList->setFrameShape(QFrame::NoFrame);
    formatList->setSelectionMode(QAbstractItemView::NoSelection);
    formatList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    formatList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    formatList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    formatList->setSpacing(0);
    formatList->setItemSpacing(0);
    formatList->setViewportMargins(0, 0, 0, 0);
    formatList->setModel(viewModel);
    formatList->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    formatList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // first day of week
    DStandardItem *dayItem = new DStandardItem;
    dayItem->setText(tr("First day of week"));
    m_dayAction = new DViewItemAction;
    m_dayAction->setText(m_model->firstDayOfWeekFormat());
    dayItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_dayAction);
    connect(m_model, &DatetimeModel::firstDayOfWeekFormatChanged, this, [this](const QString &text) { m_dayAction->setText(text.toUtf8()); } );

    // short date
    DStandardItem *shortDateItem = new DStandardItem;
    shortDateItem->setText(tr("Short date"));
    m_shortDateAction = new DViewItemAction;
    m_shortDateAction->setText(m_locale.toString(QDate::currentDate(), m_model->shortDateFormat()));
    shortDateItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_shortDateAction);
    connect(m_model, &DatetimeModel::shortDateFormatChanged, this, [this](const QString &text) { m_shortDateAction->setText(m_locale.toString(QDate::currentDate(), text)); } );

    // long date
    DStandardItem *longDateItem = new DStandardItem;
    longDateItem->setText(tr("Long date"));
    m_longDateAction = new DViewItemAction;
    m_longDateAction->setText(m_locale.toString(QDate::currentDate(), m_model->longDateFormat()));
    longDateItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_longDateAction);
    connect(m_model, &DatetimeModel::longDateFormatChanged, this, [this](const QString &text) { m_longDateAction->setText(m_locale.toString(QDate::currentDate(), text)); } );

    // short time
    DStandardItem *shortTimeItem = new DStandardItem;
    shortTimeItem->setText(tr("Short time"));
    m_shortTimeAction = new DViewItemAction;
    m_shortTimeAction->setText(m_locale.toString(QTime::currentTime(), m_model->shortTimeFormat()));
    shortTimeItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_shortTimeAction);
    connect(m_model, &DatetimeModel::shortTimeFormatChanged, this, [this](const QString &text) { m_shortTimeAction->setText(m_locale.toString(QTime::currentTime(), text)); } );

    // long time
    DStandardItem *longTimeItem = new DStandardItem;
    longTimeItem->setText(tr("Long time"));
    m_longTimeAction = new DViewItemAction;
    m_longTimeAction->setText(m_locale.toString(QTime::currentTime(), m_model->longTimeFormat()));
    longTimeItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_longTimeAction);
    connect(m_model, &DatetimeModel::longTimeFormatChanged, this, [this](const QString &text) { m_longTimeAction->setText(m_locale.toString(QTime::currentTime(), text)); } );

    // currency
    DStandardItem *currencyItem = new DStandardItem;
    currencyItem->setText(tr("Currency symbol"));
    m_currencyAction = new DViewItemAction;
    m_currencyAction->setText(m_model->currencyFormat());
    currencyItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_currencyAction);
    connect(m_model, &DatetimeModel::currencyFormatChanged, this, [this](const QString &text) { m_currencyAction->setText(text.toUtf8()); } );

    // number
    DStandardItem *numberItem = new DStandardItem;
    numberItem->setText(tr("Numbers"));
    m_numberAction = new DViewItemAction;
    m_numberAction->setText(m_model->numberFormat());
    numberItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_numberAction);
    connect(m_model, &DatetimeModel::numberFormatChanged, this, [this](const QString &text) { m_numberAction->setText(text.toUtf8()); } );

    // paper
    DStandardItem *paperItem = new DStandardItem;
    paperItem->setText(tr("Paper"));
    m_paperAction = new DViewItemAction;
    m_paperAction->setText(m_model->paperFormat());
    paperItem->setActionList(Qt::RightEdge, DViewItemActionList() << m_paperAction);
    connect(m_model, &DatetimeModel::paperFormatChanged, this, [this](const QString &text) { m_paperAction->setText(text.toUtf8()); } );

    viewModel->appendRow(dayItem);
    viewModel->appendRow(shortDateItem);
    viewModel->appendRow(longDateItem);
    viewModel->appendRow(shortTimeItem);
    viewModel->appendRow(longTimeItem);
    viewModel->appendRow(currencyItem);
    viewModel->appendRow(numberItem);
    viewModel->appendRow(paperItem);
}

void RegionModule::initFormatModificationModule()
{
    m_formatModificationModule = new ItemModule("", tr(""), [this](ModuleObject *){
        QWidget *widget = new QWidget;
        QHBoxLayout *hlayout = new QHBoxLayout(widget);
        hlayout->setSpacing(0);
        hlayout->setMargin(0);
        DCommandLinkButton *button = new DCommandLinkButton(tr("custom format"));
        connect(button, &QPushButton::clicked, this, [this](){
            CustomRegionFormatDialog dlg;
            connect(&dlg, &CustomRegionFormatDialog::customFormatSaved, this, [this](const RegionFormat &regionFormat){
                updateRegionFormat(regionFormat);
            });
            dlg.initRegionFormat(m_locale, m_regionFormat);
            dlg.exec();
        });
        hlayout->addStretch(0);
        hlayout->addWidget(button);
        return widget;
    });
    appendChild(m_formatModificationModule);
}

void RegionModule::updateRegionFormat(const RegionFormat &regionFormat)
{
    m_regionFormat = regionFormat;

    m_dayAction->setText(regionFormat.firstDayOfWeekFormat);
    m_shortDateAction->setText(m_locale.toString(QDate::currentDate(), regionFormat.shortDateFormat));
    m_longDateAction->setText(m_locale.toString(QDate::currentDate(), regionFormat.longDateFormat));
    m_shortTimeAction->setText(m_locale.toString(QTime::currentTime(), regionFormat.shortTimeFormat));
    m_longTimeAction->setText(m_locale.toString(QTime::currentTime(), regionFormat.longTimeFormat));
    m_currencyAction->setText(regionFormat.currencyFormat);
    m_numberAction->setText(regionFormat.numberFormat);
    m_paperAction->setText(regionFormat.paperFormat);

    m_work->setConfigValue(firstDayOfWeek_key, regionFormat.firstDayOfWeekFormat);
    m_work->setConfigValue(shortDateFormat_key, regionFormat.shortDateFormat);
    m_work->setConfigValue(longDateFormat_key, regionFormat.longDateFormat);
    m_work->setConfigValue(shortTimeFormat_key, regionFormat.shortTimeFormat);
    m_work->setConfigValue(longTimeFormat_key, regionFormat.longTimeFormat);
    m_work->setConfigValue(currencyFormat_key, regionFormat.currencyFormat);
    m_work->setConfigValue(numberFormat_key, regionFormat.numberFormat);
    m_work->setConfigValue(paperFormat_key, regionFormat.paperFormat);
}

void RegionModule::onLangRegionClicked()
{
    RegionFormatDialog dlg(this->m_model);
    qRegisterMetaType<RegionAvailableData>("RegionFormat");
    connect(&dlg, &RegionFormatDialog::regionFormatSaved, this, [this](const QString &langRegion, const QLocale &locale){
        m_langRegion = langRegion;
        m_locale = locale;
        m_langRegionLabel->setText(langRegion);
        m_work->setConfigValue(languageRegion_key, langRegion);
        m_work->setConfigValue(localeName_key, locale.name());
        updateRegionFormat(RegionProxy::regionFormat(m_locale));
    });
    dlg.exec();
}
