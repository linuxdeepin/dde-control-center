// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "regionformatdialog.h"

#include "datetimemodel.h"

#include <unicode/locid.h>
#include <unicode/unistr.h>

#include <DFrame>
#include <DLabel>
#include <DSearchEdit>
#include <DSuggestButton>
#include <DTitlebar>

#include <QSortFilterProxyModel>

using icu::Locale;
using icu::UnicodeString;

using namespace DCC_NAMESPACE;

RegionFormatDialog::RegionFormatDialog(DatetimeModel *datetimeModel, QWidget *parent)
    : DAbstractDialog(parent)
{
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame); // 无边框
    titleIcon->setBackgroundTransparent(true); // 透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    QLabel *headTitle = new QLabel(tr("Region Format"));
    DFontSizeManager::instance()->bind(headTitle,
                                       DFontSizeManager::T5,
                                       QFont::DemiBold); // 设置label字体
    headTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    headTitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->setSpacing(0);
    mainVLayout->setMargin(0);

    QWidget *topWidget = new QWidget;
    QHBoxLayout *topHLayout = new QHBoxLayout(topWidget);
    topHLayout->setSpacing(0);
    topHLayout->setMargin(0);

    DFrame *leftFrame = new DFrame;
    leftFrame->setFixedSize(344, 520);
    DSearchEdit *searchEdit = new DSearchEdit;
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_regionListView = new DListView;
    m_regionListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_regionListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_regionListView->setFrameShape(QFrame::NoFrame);
    m_regionListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_regionListView->setBackgroundType(DStyledItemDelegate::ClipCornerBackground);
    m_regionListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_regionListView->setModel(m_proxyModel);

    QVBoxLayout *leftVLayout = new QVBoxLayout(leftFrame);
    leftVLayout->setSpacing(0);
    leftVLayout->setMargin(10);
    leftVLayout->addWidget(searchEdit);
    leftVLayout->addSpacing(10);
    leftVLayout->addWidget(m_regionListView);

    DFrame *rightFrame = new DFrame;
    rightFrame->setFixedSize(344, 520);
    DLabel *titleLabel = new DLabel(tr("Default format"));
    DFontSizeManager::instance()->bind(titleLabel,
                                       DFontSizeManager::T5,
                                       QFont::DemiBold); // 设置label字体
    DFrame *formatContentFrame = new DFrame;
    formatContentFrame->setBackgroundRole(DPalette::ItemBackground);
    QVBoxLayout *formatLayout = new QVBoxLayout(formatContentFrame);
    formatLayout->setMargin(0);

    m_dayLabel = addFormatItem(formatContentFrame, tr("First of day"), "Monday");
    m_shortDateLabel = addFormatItem(formatContentFrame, tr("Short date"), "2023.10.01");
    m_longDateLabel = addFormatItem(formatContentFrame, tr("Long date"), "2023.10.01.Mon");
    m_shortTimeLabel = addFormatItem(formatContentFrame, tr("Short time"), "12:00");
    m_longTimeLabel = addFormatItem(formatContentFrame, tr("Long time"), "12:00:00");
    m_currencyLabel = addFormatItem(formatContentFrame, tr("Currency symbol"), "$");
    m_numberLabel = addFormatItem(formatContentFrame, tr("Numbers"), "123456789");
    m_paperLabel = addFormatItem(formatContentFrame, tr("Paper"), "A4");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightFrame);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(10);
    rightLayout->addWidget(titleLabel);
    rightLayout->addSpacing(12);
    rightLayout->addWidget(formatContentFrame);
    rightLayout->addStretch(0);

    QWidget *buttonsWidget = new QWidget;
    QHBoxLayout *bHLayout = new QHBoxLayout(buttonsWidget);
    bHLayout->setSpacing(0);
    bHLayout->setMargin(0);
    QPushButton *cancelBtn = new QPushButton(tr("Cancel"));
    cancelBtn->setFixedSize(200, 36);
    m_saveBtn = new DSuggestButton(tr("Save"));
    m_saveBtn->setFixedSize(200, 36);
    m_saveBtn->setEnabled(false);
    bHLayout->addStretch(0);
    bHLayout->addWidget(cancelBtn);
    bHLayout->addSpacing(10);
    bHLayout->addWidget(m_saveBtn);
    bHLayout->addStretch(0);

    topHLayout->addWidget(leftFrame);
    topHLayout->addSpacing(10);
    topHLayout->addWidget(rightFrame);

    mainVLayout->addWidget(titleIcon);
    mainVLayout->addWidget(headTitle);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(topWidget);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(buttonsWidget);
    mainVLayout->addSpacing(40);

    setLayout(mainVLayout);

    connect(searchEdit, &DLineEdit::textChanged, m_proxyModel, &QSortFilterProxyModel::setFilterWildcard);
    connect(cancelBtn, &QPushButton::clicked, this, &RegionFormatDialog::close);
    connect(m_saveBtn, &QPushButton::clicked, this, &RegionFormatDialog::onSaved);
    connect(m_regionListView, &QListView::clicked, this, &RegionFormatDialog::onRegionSelected);

    initItemModel(datetimeModel);
    m_proxyModel->setSourceModel(m_model);
}

RegionFormatDialog::~RegionFormatDialog() { }

void RegionFormatDialog::setCurrentRegion(const QString &region)
{
    QModelIndex start = m_proxyModel->index(0, 0);
    if (!start.isValid()) {
        qWarning() << "startIndex is invalid when setCurrentRegion called!";
        return;
    }
    QModelIndexList results = m_proxyModel->match(start, Qt::DisplayRole, region);
    if (results.size() > 0) {
        m_regionListView->setCurrentIndex(results.first());
        auto realIndex = m_proxyModel->mapToSource(results.first());
        m_locale = realIndex.data(RegionFormatRole::LocaleRole).toLocale();
        updateRegionFormat(m_locale);
        QStandardItem *selectedItem = m_model->itemFromIndex(realIndex);
        if (selectedItem) {
            selectedItem->setCheckState(Qt::Checked);
            m_lastSelectedIndex = realIndex;
        }
    } else {
        qWarning() << "There is not anything matched in region proxyModel";
    }
}

void RegionFormatDialog::onRegionSelected(const QModelIndex &index)
{
    updateDataModel(m_model, index);
    m_langRegion = index.data(RegionFormatRole::TextRole).toString();
    m_locale = index.data(RegionFormatRole::LocaleRole).toLocale();
    updateRegionFormat(m_locale);
}

void RegionFormatDialog::onSaved()
{
    Q_EMIT regionFormatSaved(m_langRegion, m_locale);
    close();
}

void RegionFormatDialog::initItemModel(DatetimeModel *dateTimeModel)
{
    m_regions = dateTimeModel->regions();
    for (auto locale : m_regions) {
        // This is legacy code that no longer used, the following code doesn't garantee to be compilable.
        // The code here is just for demo purpose.
        auto langAndRegion = DCCLocale::languageAndRegionName(locale.name());
        QString langRegionText = QString("%1 (%2)").arg(langAndRegion.first).arg(langAndRegion.second);
        DStandardItem *item = new DStandardItem;
        item->setData(langAndRegion.first, RegionFormatRole::TextRole);
        item->setData(locale, RegionFormatRole::LocaleRole);
        item->setText(langRegionText);
        item->setSizeHint(QSize(304, 36));
        m_model->appendRow(item);
    }
}

QLabel *RegionFormatDialog::addFormatItem(const QWidget *frame,
                                          const QString &name,
                                          const QString &format)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QLabel *nameLabel = new QLabel(name);
    QLabel *formatLabel = new QLabel(format);
    hLayout->addWidget(nameLabel);
    hLayout->addStretch(0);
    hLayout->addWidget(formatLabel);
    frame->layout()->addWidget(widget);
    return formatLabel;
}

void RegionFormatDialog::updateRegionFormat(const QLocale &locale)
{
    RegionFormat regionFormat = RegionProxy::regionFormat(locale);
    m_dayLabel->setText(locale.standaloneDayName(regionFormat.firstDayOfWeekFormat));
    m_shortDateLabel->setText(locale.toString(QDate::currentDate(), regionFormat.shortDateFormat));
    m_longDateLabel->setText(locale.toString(QDate::currentDate(), regionFormat.longDateFormat));
    m_shortTimeLabel->setText(locale.toString(QTime::currentTime(), regionFormat.shortTimeFormat));
    m_longTimeLabel->setText(locale.toString(QTime::currentTime(), regionFormat.longTimeFormat));
    m_currencyLabel->setText(regionFormat.currencyFormat.toUtf8().data());
    m_numberLabel->setText(regionFormat.numberFormat.toUtf8().data());
    m_paperLabel->setText(regionFormat.paperFormat.toUtf8().data());
}

void RegionFormatDialog::updateDataModel(QStandardItemModel *model, const QModelIndex &index)
{
    if (m_lastSelectedIndex.isValid()) {
        model->itemFromIndex(m_lastSelectedIndex)->setCheckState(Qt::Unchecked);
    }
    auto realIndex = m_proxyModel->mapToSource(index);
    QStandardItem *selectedItem = model->itemFromIndex(realIndex);
    if (selectedItem) {
        selectedItem->setCheckState(Qt::Checked);
        m_lastSelectedIndex = realIndex;
        m_saveBtn->setEnabled(true);
    }
}
