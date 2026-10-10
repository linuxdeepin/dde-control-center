// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "monitoritemdelegate.h"

#include <QDebug>
#include <QPainter>
#include <QVariant>
#include <QPalette>
#include <QIcon>
#include <QListView>
#include <QTimer>

#include <DStyleOption>
#include <DPalette>
#include <DPaletteHelper>

const int STANDARD_WIDGET = 36;
const int EXPAND_WIDGET = 84;

MonitorItemDelegate::MonitorItemDelegate(QAbstractItemView *parent)
    : QStyledItemDelegate(parent)
    , m_view(parent)
    , m_standardHeight(STANDARD_WIDGET)
    , m_expandHeight(EXPAND_WIDGET)
    , m_bottomSpacing(10)
{
}

void MonitorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    DStyleOptionBackgroundGroup boption;
    boption.init(m_view);
    boption.QStyleOption::operator=(option);
    boption.state.setFlag(QStyle::State_Active);

    ItemSpacing itemSpacing = getItemSpacing(index);
    if (itemSpacing.top != 0 || itemSpacing.bottom != 0) {
        boption.rect.adjust(0, itemSpacing.top, 0, -itemSpacing.bottom);
    }

    QColor bgColor, textColor;
    if (m_view->currentIndex() == index) {
        if (option.rect.height() > 50) {
            textColor = boption.dpalette.brightText().color();
            bgColor = boption.dpalette.brightText().color();
            bgColor.setAlphaF(0.1);
        } else {
            textColor = boption.dpalette.highlightedText().color();
            bgColor = boption.dpalette.highlight().color();
        }
    } else {
        textColor = boption.dpalette.brightText().color();
        bgColor = boption.dpalette.brightText().color();
        bgColor.setAlphaF(0.05);
    }

    if (textColor.isValid()) {
        boption.palette.setBrush(QPalette::BrightText, textColor);
        boption.palette.setBrush(QPalette::Text, textColor);
        boption.palette.setBrush(QPalette::Highlight, textColor);
        QWidget *w = m_view->indexWidget(index);
        if (w) {
            w->setPalette(boption.palette);
        }
    }

    if (bgColor.isValid()) {
        boption.dpalette.setBrush(DPalette::ItemBackground, bgColor);
        boption.directions = Qt::Vertical;
        boption.position = DStyleOptionBackgroundGroup::ItemBackgroundPosition(itemSpacing.viewItemPosition);
        m_view->style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(DStyle::PE_ItemBackground), &boption, painter, option.widget);
    }
}

QWidget *MonitorItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    if (!index.isValid()) {
        return nullptr;
    }

    QStandardItem* item = qobject_cast<QStandardItemModel*>(m_view->model())->itemFromIndex(index);
    MonitorItemWidget *widget = nullptr;
    if (item) {
        widget = new MonitorItemWidget(dynamic_cast<MonitorItem *>(item), parent);
    }
    return widget;
}

QSize MonitorItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
    ItemSpacing itemSpacing = getItemSpacing(index);
    return QSize(-1, itemSpacing.top + itemSpacing.height + itemSpacing.bottom);
}

void MonitorItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!editor)
        return;

    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    ItemSpacing itemSpacing = getItemSpacing(index);
    if (itemSpacing.top != 0 || itemSpacing.bottom != 0) {
        QRect geom = editor->geometry();
        geom.adjust(0, itemSpacing.top, 0, -itemSpacing.bottom);
        editor->setGeometry(geom);
    }
}

void MonitorItemDelegate::destroyEditor(QWidget *editor, const QModelIndex &) const
{
    delete editor;
}

ItemSpacing MonitorItemDelegate::getItemSpacing(const QModelIndex &index) const
{
    ItemSpacing spacing;
    spacing.top = 0;
    QStandardItem *item = qobject_cast<QStandardItemModel*>(m_view->model())->itemFromIndex(index);
    spacing.height = item ? item->sizeHint().height() : STANDARD_WIDGET;
    spacing.bottom = index.row() == (m_view->model()->rowCount()-1) ? 0 : m_bottomSpacing;
    spacing.viewItemPosition = QStyleOptionViewItem::OnlyOne; // 显示样式
    return spacing;
}

MultiscreenOptionItemDelegate::MultiscreenOptionItemDelegate(QAbstractItemView *parent)
    : MonitorItemDelegate(parent)
    , m_view(parent)
{
}

QWidget *MultiscreenOptionItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    if (!index.isValid()) {
        return nullptr;
    }

    QStandardItem* item = qobject_cast<QStandardItemModel*>(m_view->model())->itemFromIndex(index);
    MultiscreenOptionItemWidget *widget = nullptr;
    if (item) {
        widget = new MultiscreenOptionItemWidget(dynamic_cast<MultiscreenOptionItem *>(item), parent);
    }
    return widget;
}

MonitorItem::MonitorItem(Monitor *monitor)
    : QStandardItem()
    , m_monitor(monitor)
    , m_connectingState(false)
    , m_icon(QIcon())
    , m_name(QString())
    , m_itemState(MonitorItemState::NoState)
{
    if (!m_monitor) {
        qWarning() << "Monitor pointer is nullptr !";
        return;
    }

    m_icon = QIcon::fromTheme(QString("monitor"));
    m_name = monitor->name();
    onNdSinkStateChanged(m_monitor->state());

    connect(m_monitor, &Monitor::NameChanged, this, &MonitorItem::nameChanged);
    connect(m_monitor, &Monitor::stateChanged, this, &MonitorItem::onNdSinkStateChanged);
    connect(this, &MonitorItem::itemStateChanged, m_monitor, &Monitor::stateChangeFinished);
}

MonitorItem::~MonitorItem()
{
}

void MonitorItem::onNdSinkStateChanged(const Monitor::NdSinkState state)
{
    switch (state) {
    case Monitor::ND_SINK_STATE_DISCONNECTED:
    case Monitor::ND_SINK_STATE_ERROR:
        m_connectingState = false;
        setSizeHint(QSize(-1, STANDARD_WIDGET));
        updateItemState(MonitorItemState::NoState);
        break;
    default:
        m_connectingState = true;
        setSizeHint(QSize(-1, EXPAND_WIDGET));
        updateItemState(MonitorItemState::Connecting);
        break;
    }
}

void MonitorItem::updateIcon(const QIcon &icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        Q_EMIT iconChanged(m_icon);
    }
}

void MonitorItem::updateName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(m_name);
    }
}

void MonitorItem::updateItemState(const MonitorItemState state)
{
    if (m_itemState != state) {
        m_itemState = state;
        Q_EMIT itemStateChanged(m_itemState);
    }
}

void MonitorItem::connMonitor()
{
    if (m_monitor) {
        m_monitor->connMonitor();
    }
}
void MonitorItem::disconnectMonitor()
{
    if (m_monitor) {
        m_monitor->disconnectMonitor();
    }
}

MultiscreenOptionItem::MultiscreenOptionItem(DisplayMode displayMode, const QString &name, const QString &screen)
    : QStandardItem()
    , m_displayMode(displayMode)
    , m_icon(QIcon())
    , m_name(name)
    , m_screen(screen)
{
    switch (m_displayMode) {
    case MergeMode:
        m_icon = QIcon::fromTheme(QString("duplicate"));
        break;
    case ExtendMode:
        m_icon = QIcon::fromTheme(QString("extend"));
        break;
    case SingleMode:
        m_icon = QIcon::fromTheme(QString("monitor"));
        break;
    }
    setSizeHint(QSize(-1, STANDARD_WIDGET));
}

MultiscreenOptionItem::~MultiscreenOptionItem()
{
}

void MultiscreenOptionItem::updateIcon(const QIcon &icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        Q_EMIT iconChanged(m_icon);
    }
}

void MultiscreenOptionItem::updateName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(m_name);
    }
}

MonitorItemWidget::MonitorItemWidget(MonitorItem *item, QWidget *parent)
    : QWidget(parent)
    , m_item(item)
    , m_mainLayout(new QVBoxLayout(this))
    , m_nameBox(nullptr)
    , m_icon(nullptr)
    , m_nameLabel(nullptr)
    , m_stateBox(nullptr)
    , m_spinner(nullptr)
    , m_tipLabel(nullptr)
    , m_disconnectBtn(nullptr)
{
    if (!m_item) {
        QLabel *err = new QLabel(this);
        err->setText("Unknown Item");
        m_mainLayout->addWidget(err, 1);
        return;
    }

    initUI();
    setAccessibleName(m_item->name());
    updateItemState(m_item->itemState());

    if (parent)
        setForegroundRole(parent->foregroundRole());

    connect(m_item, &MonitorItem::iconChanged, this, &MonitorItemWidget::updateIcon);
    connect(m_item, &MonitorItem::nameChanged, this, &MonitorItemWidget::updateName);
    connect(m_item, &MonitorItem::itemStateChanged, this, &MonitorItemWidget::updateItemState);
    connect(m_disconnectBtn, &QPushButton::clicked, m_item, &MonitorItem::disconnectMonitor);
}

MonitorItemWidget::~MonitorItemWidget()
{
}

void MonitorItemWidget::initUI()
{
    m_nameBox = new DFrame(this);
    m_nameBox->setFrameShape(QFrame::NoFrame);
    m_nameBox->setFixedHeight(STANDARD_WIDGET);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout->setSpacing(0);

    m_icon = new CommonIconButton(this);
    m_icon->setFixedSize(16, 16);
    m_icon->setIcon(m_item->icon());

    m_nameLabel = new DLabel(this);
    m_nameLabel->setText(m_item->name());
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    nameLayout->addWidget(m_icon, 0);
    nameLayout->addSpacing(10);
    nameLayout->addWidget(m_nameLabel, 1);
    m_nameBox->setLayout(nameLayout);

    m_stateBox = new DFrame(this);
    m_stateBox->setFrameShape(QFrame::NoFrame);
    m_stateBox->setFixedHeight(EXPAND_WIDGET - STANDARD_WIDGET);

    QHBoxLayout *stateLayout = new QHBoxLayout();
    stateLayout->setContentsMargins(0, 3, 0, 0);
    stateLayout->setSpacing(0);

    QHBoxLayout *tipLayout = new QHBoxLayout();
    tipLayout->setContentsMargins(0, 8, 0, 0);
    tipLayout->setSpacing(0);
    m_spinner = new DSpinner(this);
    m_spinner->setFixedSize(16, 16);
    m_spinner->stop();

    m_tipLabel = new TipLabel(this);
    m_tipLabel->setText(tr("Connecting"));

    tipLayout->addWidget(m_spinner);
    tipLayout->addSpacing(10);
    tipLayout->addWidget(m_tipLabel);

    m_disconnectBtn = new CancelButton(this);
    m_disconnectBtn->setObjectName("DisconnectBtn");
    m_disconnectBtn->setText(tr("Cancel"));
    m_disconnectBtn->setFixedSize(98, 36);

    stateLayout->addLayout(tipLayout);
    stateLayout->addWidget(m_disconnectBtn, 0, Qt::AlignRight | Qt::AlignTop);
    m_stateBox->setLayout(stateLayout);

    m_mainLayout->setContentsMargins(2, 0, 2, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_nameBox);
    m_mainLayout->addWidget(m_stateBox);
}

void MonitorItemWidget::updateIcon(const QIcon &icon)
{
    m_icon->setIcon(icon);
}

void MonitorItemWidget::updateName(const QString &name)
{
    m_nameLabel->setText(name);
}

void MonitorItemWidget::updateItemState(const MonitorItemState state)
{
    switch (state) {
    case MonitorItemState::NoState:
        m_stateBox->setVisible(false);
        m_spinner->stop();
        break;
    case MonitorItemState::Connecting:
        m_stateBox->setVisible(true);
        m_spinner->start();
        break;
    default:
        m_stateBox->setVisible(false);
        m_spinner->stop();
        break;
    }
}

bool MonitorItemWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::PaletteChange: {
        QLayout *layout = this->layout();
        for (int i = 0; i < layout->count(); i++) {
            QWidget *widget = layout->itemAt(i)->widget();
            if (widget) {
                widget->setPalette(palette());
            }
        }
    } break;
    default:
        break;
    }
    return QWidget::event(e);
}

MultiscreenOptionItemWidget::MultiscreenOptionItemWidget(MultiscreenOptionItem *item, QWidget *parent)
    : QWidget(parent)
    , m_item(item)
    , m_mainLayout(new QHBoxLayout(this))
    , m_nameBox(nullptr)
    , m_icon(nullptr)
    , m_nameLabel(nullptr)
    , m_selectIcon(nullptr)
{
    if (!m_item) {
        QLabel *err = new QLabel(this);
        err->setText("Unknown Item");
        m_mainLayout->addWidget(err, 1);
        return;
    }

    initUI();
    setAccessibleName(m_item->name());

    if (parent)
        setForegroundRole(parent->foregroundRole());

    connect(m_item, &MultiscreenOptionItem::iconChanged, this, &MultiscreenOptionItemWidget::updateIcon);
    connect(m_item, &MultiscreenOptionItem::nameChanged, this, &MultiscreenOptionItemWidget::updateName);
    connect(m_item, &MultiscreenOptionItem::selected, m_selectIcon, &MultiscreenOptionItemWidget::setVisible);
}

MultiscreenOptionItemWidget::~MultiscreenOptionItemWidget()
{
}

void MultiscreenOptionItemWidget::initUI()
{
    m_nameBox = new DFrame(this);
    m_nameBox->setFrameShape(QFrame::NoFrame);
    m_nameBox->setFixedHeight(STANDARD_WIDGET);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout->setSpacing(0);

    m_icon = new CommonIconButton(this);
    m_icon->setFixedSize(16, 16);
    m_icon->setIcon(m_item->icon());

    m_nameLabel = new DLabel(this);
    m_nameLabel->setText(m_item->name());
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    nameLayout->addWidget(m_icon, 0);
    nameLayout->addSpacing(10);
    nameLayout->addWidget(m_nameLabel, 1);
    m_nameBox->setLayout(nameLayout);

    m_mainLayout->setContentsMargins(2, 0, 2, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_nameBox);

    m_selectIcon = new CommonIconButton(this);
    m_selectIcon->setFixedSize(16, 16);
    m_selectIcon->setIcon(QIcon::fromTheme("select"));
    m_selectIcon->setVisible(false);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_selectIcon);
}

void MultiscreenOptionItemWidget::updateIcon(const QIcon &icon)
{
    m_icon->setIcon(icon);
}

void MultiscreenOptionItemWidget::updateName(const QString &name)
{
    m_nameLabel->setText(name);
}

bool MultiscreenOptionItemWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::PaletteChange: {
        QLayout *layout = this->layout();
        for (int i = 0; i < layout->count(); i++) {
            QWidget *widget = layout->itemAt(i)->widget();
            if (widget) {
                widget->setPalette(palette());
            }
        }
    } break;
    default:
        break;
    }
    return QWidget::event(e);
}

void MultiscreenOptionItemWidget::updateItemState(const MonitorItemState state)
{
    Q_UNUSED(state)
}
