// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "permissioninfowidget.h"
#include "privacymodule.h"
#include "modules/privacy/applicationitem.h"
#include "modules/privacy/privacysecuritymodel.h"

#include <DLabel>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSwitchButton>
#include <DTreeView>
#include <DStyledItemDelegate>
#include <DPalette>

#include <QVBoxLayout>
#include <QTreeView>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QPaintEvent>

DWIDGET_USE_NAMESPACE

class AppTreeView : public QTreeView
{
public:
    explicit AppTreeView(QWidget *parent = nullptr)
        : QTreeView(parent) {}

protected:
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        opt.rect.adjust(0, 1, 0, -1);

        DStyleOptionBackgroundGroup boption;
        boption.init(this);
        boption.QStyleOption::operator=(opt);

        QAbstractItemModel *m = model();
        int row = index.row();
        bool expanded = isExpanded(index);
        const QModelIndex parentIndex = m->parent(index);
        if (!parentIndex.isValid()) { // 一级
            if (row == 0) {
                if (!expanded && m->rowCount(parentIndex) == 1) {
                    opt.viewItemPosition = QStyleOptionViewItem::OnlyOne;
                } else {
                    opt.viewItemPosition = QStyleOptionViewItem::Beginning;
                }
            } else if (row == m->rowCount(parentIndex) - 1) {
                if (expanded) {
                    opt.viewItemPosition = QStyleOptionViewItem::Middle;
                } else {
                    opt.viewItemPosition = QStyleOptionViewItem::End;
                }
            } else {
                opt.viewItemPosition = QStyleOptionViewItem::Middle;
            }
        } else { // 二级
            if (row == m->rowCount(parentIndex) - 1 && parentIndex.row() == m->rowCount(m->parent(parentIndex)) - 1) {
                opt.viewItemPosition = QStyleOptionViewItem::End;
            } else {
                opt.viewItemPosition = QStyleOptionViewItem::Middle;
            }
        }
        boption.directions = Qt::Vertical;
        boption.position = DStyleOptionBackgroundGroup::ItemBackgroundPosition(opt.viewItemPosition);
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(DStyle::PE_ItemBackground), &boption, painter, opt.widget);
        QTreeView::drawRow(painter, opt, index);
    }
};

// 标识用唯一id拼接行号实现，0xlliiiiiiii i占位为唯一id，l占位为级别
#define ITEM_ID_MARK 0x0000FFFFFFFF
#define ITEM_LEVEL_MARK 0xFFFF00000000
#define ITEM_LEVEL_1 0x0000000000 // 一级
#define ITEM_LEVEL_2 0x0100000000 // 二级
static quint64 ToLevel(quintptr internalId)
{
    return internalId & ITEM_LEVEL_MARK;
}
static unsigned ToUniqueID(quintptr internalId)
{
    return internalId & ITEM_ID_MARK;
}

static quintptr ToInternalId(unsigned id, quint64 level = ITEM_LEVEL_1)
{
    return id | level;
}

AppPermissionItemModel::AppPermissionItemModel(PrivacySecurityModel *model, QList<int> premission, QObject *parent)
    : QAbstractItemModel(parent)
    , m_model(model)
    , m_premission(premission)
{
    connect(m_model, &PrivacySecurityModel::itemAboutToBeAdded, this, [this](int pos) {
        beginInsertRows(QModelIndex(), pos, pos);
    });
    connect(m_model, &PrivacySecurityModel::itemAdded, this, [this]() {
        endInsertRows();
    });
    connect(m_model, &PrivacySecurityModel::itemAboutToBeRemoved, this, [this](int pos) {
        beginRemoveRows(QModelIndex(), pos, pos);
    });
    connect(m_model, &PrivacySecurityModel::itemRemoved, this, [this]() {
        endRemoveRows();
    });
    connect(m_model, &PrivacySecurityModel::itemDataUpdate, this, [this]() {
        beginResetModel();
        endResetModel();
    });
    connect(m_model, &PrivacySecurityModel::itemDataChanged, this, [this](ApplicationItem *appItem) {
        const QModelIndex i = index(appItem);
        Q_EMIT dataChanged(i, i);
        int row = rowCount(i);
        if (row != 0) {
            QModelIndex topLeft = index(0, 0, i);
            QModelIndex bottomRight = index(row - 1, 0, i);
            Q_EMIT dataChanged(topLeft, bottomRight);
        }
    });
}

QVariant AppPermissionItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    quint64 level = ToLevel(index.internalId());
    unsigned uniqueID = ToUniqueID(index.internalId());
    ApplicationItem *item = m_model->applictionItem(uniqueID);
    if (!item)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (level == ITEM_LEVEL_1)
            return item->name();
        else {
            switch (m_premission.at(index.row())) {
            case PermissionType::DocumentsFoldersPermission:
                return tr("\"Documents\" folder");
            case PermissionType::PicturesFoldersPermission:
                return tr("\"Pictures\" folder");
            }
        }
        break;
    case Qt::DecorationRole:
        if (level == ITEM_LEVEL_1)
            return item->icon();
        break;
    case Qt::CheckStateRole:
        if (m_premission.size() == 1) {
            return item->isPremissionEnabled(m_premission.first()) ? Qt::Checked : Qt::Unchecked;
        } else if (level == ITEM_LEVEL_2) {
            return item->isPremissionEnabled(m_premission.at(index.row())) ? Qt::Checked : Qt::Unchecked;
        }
        break;
    case Qt::TextAlignmentRole:
        return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignLeft));
    case Qt::SizeHintRole:
        return QSize(40, 40);
    default:
        break;
    }
    return QVariant();
}

bool AppPermissionItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::CheckStateRole) {
        quint64 level = ToLevel(index.internalId());
        unsigned uniqueID = ToUniqueID(index.internalId());
        ApplicationItem *item = m_model->applictionItem(uniqueID);
        if (!item)
            return false;

        item->setPremissionEnabled(m_premission.at(level == ITEM_LEVEL_1 ? 0 : index.row()), value == Qt::Checked);
        return true;
    }
    return false;
}

QModelIndex AppPermissionItemModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!hasIndex(row, column, parentIndex)) {
        return QModelIndex();
    }
    if (parentIndex == QModelIndex()) { // 一级
        return createIndex(row, column, ToInternalId(m_model->applictionItems().at(row)->getUniqueID()));
    } else {
        unsigned uniqueID = ToUniqueID(parentIndex.internalId());
        int i = m_model->getAppIndex(uniqueID);
        if (i != -1)
            return createIndex(row, column, ToInternalId(uniqueID, ITEM_LEVEL_2));
    }
    return QModelIndex();
}

QModelIndex AppPermissionItemModel::index(ApplicationItem *item) const
{
    int row = m_model->applictionItems().indexOf(item);
    return row == -1 ? QModelIndex() : createIndex(row, 0, ToInternalId(item->getUniqueID()));
}

QModelIndex AppPermissionItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    quint64 level = ToLevel(index.internalId());
    unsigned uniqueID = ToUniqueID(index.internalId());
    if (level != ITEM_LEVEL_1) {
        int i = m_model->getAppIndex(uniqueID);
        if (i != -1)
            return createIndex(i, 0, ToInternalId(uniqueID));
    }
    return QModelIndex();
}

int AppPermissionItemModel::rowCount(const QModelIndex &parent) const
{
    quint64 level = ToLevel(parent.internalId());
    if (parent == QModelIndex()) { // 0级
        return m_model->applictionItems().size();
    } else if (level == ITEM_LEVEL_1) { // 一级
        return m_premission.size() == 1 ? 0 : m_premission.size();
    }
    return 0;
}

int AppPermissionItemModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

Qt::ItemFlags AppPermissionItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}
//////////////////////////////
PermissionInfoWidget::PermissionInfoWidget(const PrivacyPage &page, PrivacySecurityModel *model, QWidget *parent)
    : QWidget(parent)
    , m_privacyPage(page)
    , m_model(model)
    , m_btnEnabled(new DSwitchButton())
    , m_appView(new AppTreeView())
    , m_noneWidget(new QWidget())
{
    initUI();
    initConnect();
}

PermissionInfoWidget::~PermissionInfoWidget()
{
}

void PermissionInfoWidget::mainPremissionEnabledChanged(int premission, bool enabled)
{
    if (premission == m_privacyPage.mainPremission) {
        m_btnEnabled->blockSignals(true);
        m_btnEnabled->setChecked(enabled);
        m_btnEnabled->blockSignals(false);
        m_appView->setEnabled(enabled);
    }
}

void PermissionInfoWidget::setMainPremissionEnabled(bool enabled)
{
    m_model->setPremissionEnabled(m_privacyPage.mainPremission, enabled);
}

void PermissionInfoWidget::onAppSizeChanged()
{
    bool isNone = m_model->applictionItems().isEmpty();
    m_appView->setVisible(!isNone);
    m_noneWidget->setVisible(isNone);
}

void PermissionInfoWidget::initUI()
{
    setAccessibleName("PermissionInfoWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(QMargins(10, 10, 10, 10));

    setLayout(mainLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hLayoutDisturbMode = new QHBoxLayout;
    hLayoutDisturbMode->setContentsMargins(8, 0, 10, 0);
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(m_privacyPage.icon.pixmap(64, 64));
    DLabel *titleLabel = new DLabel(m_privacyPage.displayName);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5, QFont::DemiBold);
    hLayoutDisturbMode->addWidget(iconLabel);
    hLayoutDisturbMode->addWidget(titleLabel, Qt::AlignLeft);
    hLayoutDisturbMode->addWidget(m_btnEnabled, Qt::AlignRight);
    m_btnEnabled->setChecked(m_model->isPremissionEnabled(m_privacyPage.mainPremission));
    mainLayout->addLayout(hLayoutDisturbMode);

    DTipLabel *lblTip = new DTipLabel(m_privacyPage.desc);
    DFontSizeManager::instance()->bind(lblTip, DFontSizeManager::T8);
    lblTip->adjustSize();
    lblTip->setWordWrap(true);
    lblTip->setContentsMargins(10, 5, 10, 5);
    lblTip->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(lblTip);

    m_appView->header()->hide();
    m_appView->setIconSize(QSize(32, 32));
    m_appView->setUniformRowHeights(true);
    m_appView->setEnabled(m_btnEnabled->isChecked());
    m_appView->setFrameStyle(QFrame::NoFrame);
    mainLayout->addWidget(m_appView);
    m_appView->setModel(new AppPermissionItemModel(m_model, m_privacyPage.premission, m_appView));
    //    m_appView->expandAll();

    m_noneWidget->setVisible(false);
    QVBoxLayout *noneLayout = new QVBoxLayout(m_noneWidget);
    QLabel *noneIconLabel = new QLabel();
    noneIconLabel->setPixmap(QIcon::fromTheme("dcc_none").pixmap(72, 72));
    QLabel *noneLabel = new QLabel(m_privacyPage.noneApp);
    noneLayout->addStretch();
    noneLayout->addWidget(noneIconLabel, 0, Qt::AlignCenter);
    noneLayout->addSpacing(10);
    noneLayout->addWidget(noneLabel, 0, Qt::AlignCenter);
    noneLayout->addStretch();
    mainLayout->addWidget(m_noneWidget);
    onAppSizeChanged();
    setDisabled(m_model->updating());
}

void PermissionInfoWidget::initConnect()
{
    m_appView->setExpandsOnDoubleClick(false);
    connect(m_appView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        if (m_appView->isExpanded(index)) {
            m_appView->collapse(index);
        } else {
            m_appView->expand(index);
        }
    });
    connect(m_model, &PrivacySecurityModel::itemDataUpdate, this, &PermissionInfoWidget::setDisabled);
    connect(m_model, &PrivacySecurityModel::premissionEnabledChanged, this, &PermissionInfoWidget::mainPremissionEnabledChanged);
    connect(m_model, &PrivacySecurityModel::itemAdded, this, &PermissionInfoWidget::onAppSizeChanged);
    connect(m_model, &PrivacySecurityModel::itemRemoved, this, &PermissionInfoWidget::onAppSizeChanged);
    connect(m_btnEnabled, &DSwitchButton::checkedChanged, this, &PermissionInfoWidget::setMainPremissionEnabled);
}
