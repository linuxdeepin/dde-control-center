#include "region_setting_dialog.h"

#include <DStandardItem>
#include <DStyledItemDelegate>
#include <DSuggestButton>

#include <QAbstractItemModel>
#include <QDateTime>
#include <QPushButton>
#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

using Dtk::Widget::DStandardItem;
using Dtk::Widget::DStyledItemDelegate;
using Dtk::Widget::DSuggestButton;

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
    center->addWidget(title);

    QLabel *dateTitle = new QLabel(tr("Date"), this);
    center->addWidget(dateTitle);
    center->addWidget(m_date);

    QLabel *timeTitle = new QLabel(tr("Time"), this);
    center->addWidget(timeTitle);
    center->addWidget(m_time);

    QLabel *dateAndTimeTitle = new QLabel(tr("Date And Time"), this);
    center->addWidget(dateAndTimeTitle);
    center->addWidget(m_dateAndTime);

    QLabel *numberTitle = new QLabel(tr("Number"), this);
    center->addWidget(numberTitle);
    center->addWidget(m_number);

    QLabel *moneyTitle = new QLabel(tr("Currency"), this);
    center->addWidget(moneyTitle);
    center->addWidget(m_currency);

    center->addStretch();

    updateShowInfo(std::nullopt);
}

constexpr int SHOW_NUMBER = 123456789;

void RegionFormatShowPage::updateShowInfo(const std::optional<QString> region)
{
    QLocale locale = QLocale(region.value_or(QString()));

    m_date->setText(locale.toString(QDate::currentDate()));
    m_time->setText(locale.toString(QTime::currentTime()));
    m_dateAndTime->setText(locale.toString(QDateTime::currentDateTime()));

    m_currency->setText(locale.currencySymbol());

    m_number->setText(locale.toString(SHOW_NUMBER));
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
        QVBoxLayout *layout = new QVBoxLayout;

        m_edit->setPlaceholderText("Search");
        m_edit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_view->setBackgroundType(DStyledItemDelegate::ClipCornerBackground);
        m_view->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
        DStandardItem *item = new DStandardItem(key.name);
        item->setData(key.id, RegionRole::Key);
        model->appendRow(item);
    }
    return model;
}
