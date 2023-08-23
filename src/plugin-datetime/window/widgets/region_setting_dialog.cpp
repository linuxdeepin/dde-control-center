//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "region_setting_dialog.h"

#include <DFontSizeManager>
#include <DStandardItem>
#include <DStyledItemDelegate>
#include <DSuggestButton>
#include <DTitlebar>

#include <QAbstractItemModel>
#include <QDateTime>
#include <QPushButton>
#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

using Dtk::Widget::DFontSizeManager;
using Dtk::Widget::DStandardItem;
using Dtk::Widget::DStyledItemDelegate;
using Dtk::Widget::DSuggestButton;
using Dtk::Widget::DTitlebar;

RegionFormatShowPage::RegionFormatShowPage(QWidget *parent)
    : QWidget(parent)
    , m_date(new QLabel(this))
    , m_time(new QLabel(this))
    , m_dateAndTime(new QLabel(this))
    , m_number(new QLabel(this))
    , m_currency(new QLabel(this))
{
    QVBoxLayout *center = new QVBoxLayout(this);

    QLabel *title = new QLabel(tr("Default Format"), this);
    title->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(title, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(title);

    QLabel *dateTitle = new QLabel(tr("Date"), this);
    DFontSizeManager::instance()->bind(dateTitle, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(dateTitle);
    center->addWidget(m_date);

    center->addSpacing(8);

    QLabel *timeTitle = new QLabel(tr("Time"), this);
    DFontSizeManager::instance()->bind(timeTitle, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(timeTitle);
    center->addWidget(m_time);

    center->addSpacing(8);

    QLabel *dateAndTimeTitle = new QLabel(tr("Date And Time"), this);
    DFontSizeManager::instance()->bind(dateAndTimeTitle, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(dateAndTimeTitle);
    center->addWidget(m_dateAndTime);

    center->addSpacing(8);

    QLabel *numberTitle = new QLabel(tr("Number"), this);
    DFontSizeManager::instance()->bind(numberTitle, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(numberTitle);
    center->addWidget(m_number);

    center->addSpacing(8);

    QLabel *moneyTitle = new QLabel(tr("Currency"), this);
    DFontSizeManager::instance()->bind(moneyTitle, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(moneyTitle);
    center->addWidget(m_currency);

    center->addStretch();

    updateShowInfo(std::nullopt);
}

void RegionFormatShowPage::updateShowInfo(const std::optional<QString> region)
{
    QLocale locale = QLocale(region.value_or(QString()));

    m_date->setText(locale.toString(QDate::currentDate()));
    m_time->setText(locale.toString(QTime::currentTime()));
    m_dateAndTime->setText(locale.toString(QDateTime::currentDateTime()));

    m_currency->setText(locale.currencySymbol());

    m_number->setText(locale.toString(123456789));
}

RegionDialog::RegionDialog(LocaleList regions, QWidget *parent)
    : Dtk::Widget::DAbstractDialog(parent)
    , m_edit(new QLineEdit(this))
    , m_view(new Dtk::Widget::DListView(this))
    , m_regions(regions)
    , m_model(new QSortFilterProxyModel(this))
    , m_lastSelectedIndex(std::nullopt)
    , m_selectedValue(std::nullopt)
    , m_regionFormatShowPage(new RegionFormatShowPage)
    , m_cancelBtn(new QPushButton(tr("Cancel")))
    , m_confirmBtn(new DSuggestButton(tr("Confirm")))
{
    QVBoxLayout *center = new QVBoxLayout;
    QHBoxLayout *top = new QHBoxLayout;

    {
        DTitlebar *titleIcon = new DTitlebar();
        titleIcon->setFrameStyle(QFrame::NoFrame); // 无边框
        titleIcon->setBackgroundTransparent(true); // 透明
        titleIcon->setMenuVisible(false);
        titleIcon->setTitle(tr("Regional Setting"));
        center->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    }

    {
        QVBoxLayout *layout = new QVBoxLayout;

        m_edit->setPlaceholderText(tr("Search"));
        m_edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        m_edit->setMinimumWidth(350);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_view->setBackgroundType(DStyledItemDelegate::ClipCornerBackground);
        m_view->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setMinimumWidth(350);

        auto model = fromListToModel(m_regions);
        m_model->setSourceModel(model);

        m_view->setModel(m_model);
        layout->addWidget(m_edit);
        layout->addWidget(m_view);
        connect(m_edit, &QLineEdit::textChanged, this, &RegionDialog::onFilterChanged);
        connect(m_view, &QListView::clicked, model, [model, this](const QModelIndex &index) {
            if (m_lastSelectedIndex.has_value()) {
                model->itemFromIndex(m_lastSelectedIndex.value())->setCheckState(Qt::Unchecked);
            }
            auto realindex = m_model->mapToSource(index);
            model->itemFromIndex(realindex)->setCheckState(Qt::Checked);
            QString value = qvariant_cast<QString>(model->itemData(realindex)[Qt::DisplayRole]);
            QString key = qvariant_cast<QString>(model->itemData(realindex)[RegionRole::Key]);
            m_selectedValue = { key, value };
            m_regionFormatShowPage->updateShowInfo(key);
            m_lastSelectedIndex = realindex;
            m_confirmBtn->setEnabled(true);
        });

        m_edit->setMaximumWidth(400);
        m_view->setMaximumWidth(400);
        top->addLayout(layout);
    }

    top->addWidget(m_regionFormatShowPage);
    center->addLayout(top);
    QHBoxLayout *buttonGroup = new QHBoxLayout;
    {
        m_confirmBtn->setEnabled(false);
        buttonGroup->addWidget(m_cancelBtn);
        buttonGroup->addWidget(m_confirmBtn);
        connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        connect(m_confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
    center->addLayout(buttonGroup);
    setLayout(center);
}

void RegionDialog::onFilterChanged(const QString &filter)
{
    if (filter.contains("\\")) {
        return;
    }
    auto re = QRegularExpression(filter, QRegularExpression::CaseInsensitiveOption);
    m_model->setFilterRegularExpression(re);
}

QStandardItemModel *RegionDialog::fromListToModel(const LocaleList &regions)
{
    auto model = new QStandardItemModel(this);
    for (const LocaleInfo &key : regions) {
        QString display = QString("%1 (%2)").arg(key.name).arg(key.id);
        DStandardItem *item = new DStandardItem(display);
        item->setData(key.id, RegionRole::Key);
        model->appendRow(item);
    }
    return model;
}
