// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "resolutionwidget.h"
#include "modules/display/displaymodel.h"
#include "window/dconfigwatcher.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace dcc::display;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

fillModeCombox::fillModeCombox(QWidget *parent)
    : QComboBox(parent)
{
    connect(this, QOverload<int>::of(&QComboBox::highlighted), this, &fillModeCombox::OnHighlighted);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [=](DGuiApplicationHelper::ColorType themeType){
        //在切换主题的时候，combox无法获取当前是hidepopup还是showpopup, 所以需要设置为hidepopup来达到更新图标的目的
        Q_UNUSED(themeType)
        hidePopup();
    });

}

void fillModeCombox::OnHighlighted(int index)
{
    if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        for(int i = 0; i < this->count(); i++) {
            if(i == index)
                this->setItemIcon(i, QPixmap(this->itemData(i, LightHighlightIconRole).toString()));
            else
                this->setItemIcon(i, QPixmap(this->itemData(i, LightItemIconRole).toString()));
        }
    }
    else if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        for(int i = 0; i < this->count(); i++) {
            if(i == index)
                this->setItemIcon(i, QPixmap(this->itemData(i, DarkHighlightIconRole).toString()));
            else
                this->setItemIcon(i, QPixmap(this->itemData(i, DarkItemIconRole).toString()));
        }
    }
}

void fillModeCombox::showPopup()
{
    QComboBox::showPopup();
    setItemRoleIcon();
}

void fillModeCombox::hidePopup()
{
    QComboBox::hidePopup();
    setDefaultRoleIcon();
}

void fillModeCombox:: setDefaultRoleIcon()
{
    //获取当前主题
    if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, LightDefaultIconRole).toString()));
        }
    } else if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, DarkDefaultIconRole).toString()));
        }
    }
}

void fillModeCombox:: setItemRoleIcon()
{
    //获取当前主题
    if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, LightItemIconRole).toString()));
        }
    } else if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, DarkItemIconRole).toString()));
        }
    }
}

void fillModeCombox:: setHoverRoleIcon()
{qInfo()<<this->itemData(0, LightHoverIconRole).toString()<<this->itemData(0, DarkHoverIconRole).toString();
    qInfo()<<QPixmap(this->itemData(0, LightHoverIconRole).toString())<<QPixmap(this->itemData(0, DarkHoverIconRole).toString());
    //获取当前主题
    if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, LightHoverIconRole).toString()));
        }
    } else if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        for(int i = 0; i < this->count(); i++) {
            this->setItemIcon(i, QPixmap(this->itemData(i, DarkHoverIconRole).toString()));
        }
    }
}

bool fillModeCombox::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::HoverEnter:
        setHoverRoleIcon();
        break;
    case QEvent::HoverLeave:
        setDefaultRoleIcon();
        break;
    default:
        break;
    }
    return QComboBox::event(e);
}

ResolutionWidget::ResolutionWidget(int comboxWidth, QWidget *parent)
    : SettingsItem(parent)
    , m_resolutionLayout(new QHBoxLayout(this))
    , m_resizeDesktopLayout(new QHBoxLayout(this))
    , m_contentLayout(new QVBoxLayout(this))
    , m_resolutionCombox(new QComboBox(this))
    , m_resizeDesktopCombox(new fillModeCombox(this))
    , m_resizeDesktopItem(new SettingsItem)
    , m_model(nullptr)
    , m_monitor(nullptr)
    , m_resoItemModel(new QStandardItemModel)
    , m_resizeItemModel(new QStandardItemModel)
{
    //~ contents_path /display/Resolution
    m_resolutionLabel = new QLabel(tr("Resolution"), this);
    //~ contents_path /display/Resize Desktop
    m_resizeDesktopLabel = new QLabel(tr("Resize Desktop"), this);

    setMinimumHeight(48);
    SettingsItem *resolutionItem = new SettingsItem;
    m_resolutionLayout->setContentsMargins(10, 10, 10, 10);
    m_resolutionLayout->addWidget(m_resolutionLabel);
    m_resolutionLayout->addWidget(m_resolutionCombox);
    m_resolutionCombox->setFocusPolicy(Qt::NoFocus);
    m_resolutionCombox->setMinimumWidth(comboxWidth);
    m_resolutionCombox->setMinimumHeight(36);
    m_resolutionCombox->setModel(m_resoItemModel);
    resolutionItem->setLayout(m_resolutionLayout);
    //"Resize Desktop"
    m_resizeDesktopLayout->setContentsMargins(10, 10, 10, 10);
    m_resizeDesktopLayout->addWidget(m_resizeDesktopLabel);
    m_resizeDesktopLayout->addWidget(m_resizeDesktopCombox);
    m_resizeDesktopCombox->setFocusPolicy(Qt::NoFocus);
    m_resizeDesktopCombox->setMinimumWidth(comboxWidth);
    m_resizeDesktopCombox->setMinimumHeight(36);
    m_resizeDesktopCombox->setModel(m_resizeItemModel);
    m_resizeDesktopItem->setLayout(m_resizeDesktopLayout);
    m_resizeDesktopItem->installEventFilter(this);

    SettingsGroup *grp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    grp->getLayout()->setContentsMargins(0, 0, 0, 0);
    grp->setContentsMargins(0, 0, 0, 0);
    grp->layout()->setMargin(0);
    grp->appendItem(resolutionItem);
    grp->appendItem(m_resizeDesktopItem);

    m_contentLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_contentLayout);
    m_contentLayout->addWidget(grp);
}

ResolutionWidget::~ResolutionWidget()
{
    DConfigWatcher::instance()->erase(DConfigWatcher::display,"desktopDisplay", m_resizeDesktopItem);
}

void ResolutionWidget::setModel(DisplayModel *model, Monitor *monitor)
{
    m_model = model;
    m_resolutionCombox->setEnabled(m_model->resolutionRefreshEnable());
    m_resizeDesktopCombox->setEnabled(m_model->resolutionRefreshEnable());
    connect(m_model, &DisplayModel::monitorListChanged, this, &ResolutionWidget::initResolution);
    connect(m_model, &DisplayModel::displayModeChanged, this, &ResolutionWidget::initResolution);
    connect(m_model, &DisplayModel::resolutionRefreshEnableChanged, m_resolutionCombox, &QComboBox::setEnabled);

    setMonitor(monitor);
}

void ResolutionWidget::setMonitor(Monitor *monitor)
{
    if (monitor == nullptr || m_monitor == monitor) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_monitor != nullptr) {
        disconnect(m_monitor, &Monitor::currentFillModeChanged, this, nullptr);
        disconnect(m_monitor, &Monitor::availableFillModesChanged, this, &ResolutionWidget::OnAvailableFillModesChanged);
        disconnect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
        disconnect(m_monitor, &Monitor::bestModeChanged, this, &ResolutionWidget::initResolution);
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &ResolutionWidget::OnCurrentModeChanged);
    }

    m_monitor = monitor;

    initResolution();
    OnAvailableFillModesChanged(m_monitor->availableFillModes());
    initResizeDesktop();

    connect(m_monitor, &Monitor::availableFillModesChanged, this, &ResolutionWidget::OnAvailableFillModesChanged);
    connect(m_monitor, &Monitor::currentFillModeChanged, this, &ResolutionWidget::initResizeDesktop);
    connect(m_monitor, &Monitor::currentFillModeChanged, this, [this] (QString currFillMode) {
        Q_EMIT requestCurrFillModeChanged(m_monitor,currFillMode);
    });

    connect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
    connect(m_monitor, &Monitor::bestModeChanged, this, &ResolutionWidget::initResolution);
    connect(m_monitor, &Monitor::currentModeChanged, this, &ResolutionWidget::OnCurrentModeChanged);
}

void ResolutionWidget::OnCurrentModeChanged(const Resolution &mode)
{
    // 按主线逻辑，当mode=0时，在x11环境下需要特殊处理
    // 在wayland环境下，直接跳过
    if (qEnvironmentVariable("XDG_SESSION_TYPE").contains("x11")) {
        // 规避mode == 0
        if (mode.id() == 0) {
            return;
        }
    }

    for (int idx = 0; idx < m_resoItemModel->rowCount(); ++idx) {
        auto item = m_resoItemModel->item(idx);
        auto w = item->data(WidthRole).toInt();
        auto h = item->data(HeightRole).toInt();
        if (w == mode.width() && h == mode.height() && m_resolutionCombox->currentIndex() != item->row()) {
            m_resolutionCombox->setCurrentIndex(item->row());
            break;
        }
    }

    // 如果当前设置的分辨率不在列表里面，将分辨率选择框置空
    if (!m_monitor->modeList().contains(mode)) {
        m_resolutionCombox->setCurrentIndex(-1);
    }
}

void ResolutionWidget::setItemIcon()
{

    DStandardItem *defaultItem = new DStandardItem(tr("Default"));
    //深色
    defaultItem->setData("None", FillModeRole);
    defaultItem->setData(":/display/themes/dark/icons/dark/Default.svg", DarkItemIconRole);
    defaultItem->setData(":/display/themes/dark/icons/dark/Default.svg", DarkDefaultIconRole);
    defaultItem->setData(":/display/themes/dark/icons/white/Default.svg", DarkHighlightIconRole);
    defaultItem->setData(":/display/themes/dark/icons/hover/Default.svg", DarkHoverIconRole);
    //浅色
    defaultItem->setData(":/display/themes/light/icon/black/Default.svg", LightItemIconRole);
    defaultItem->setData(":/display/themes/light/icon/light/Default.svg", LightDefaultIconRole);
    defaultItem->setData(":/display/themes/light/icon/white/Default.svg", LightHighlightIconRole);
    defaultItem->setData(":/display/themes/light/icon/hover/Default.svg", LightHoverIconRole);

    m_mapFillModeItems["None"] = defaultItem; /*默认*/


    DStandardItem *fitItem = new DStandardItem(tr("Fit"));
    //深色
    fitItem->setData("Full aspect", FillModeRole);
    fitItem->setData(":/display/themes/dark/icons/dark/Fit.svg", DarkItemIconRole);
    fitItem->setData(":/display/themes/dark/icons/dark/Fit.svg", DarkDefaultIconRole);
    fitItem->setData(":/display/themes/dark/icons/white/Fit.svg", DarkHighlightIconRole);
    fitItem->setData(":/display/themes/dark/icons/hover/Fit.svg", DarkHoverIconRole);
    //浅色
    fitItem->setData(":/display/themes/light/icon/black/Fit.svg", LightItemIconRole);
    fitItem->setData(":/display/themes/light/icon/light/Fit.svg", LightDefaultIconRole);
    fitItem->setData(":/display/themes/light/icon/white/Fit.svg", LightHighlightIconRole);
    fitItem->setData(":/display/themes/light/icon/hover/Fit.svg", LightHoverIconRole);

    m_mapFillModeItems["Full aspect"] = fitItem; /*适应*/


    DStandardItem *stretchItem = new DStandardItem(tr("Stretch"));
    //深色
    stretchItem->setData("Full", FillModeRole);
    stretchItem->setData(":/display/themes/dark/icons/dark/Stretch.svg", DarkItemIconRole);
    stretchItem->setData(":/display/themes/dark/icons/dark/Stretch.svg", DarkDefaultIconRole);
    stretchItem->setData(":/display/themes/dark/icons/white/Stretch.svg", DarkHighlightIconRole);
    stretchItem->setData(":/display/themes/dark/icons/hover/Stretch.svg", DarkHoverIconRole);
    //浅色
    stretchItem->setData(":/display/themes/light/icon/black/Stretch.svg", LightItemIconRole);
    stretchItem->setData(":/display/themes/light/icon/light/Stretch.svg", LightDefaultIconRole);
    stretchItem->setData(":/display/themes/light/icon/white/Stretch.svg", LightHighlightIconRole);
    stretchItem->setData(":/display/themes/light/icon/hover/Stretch.svg", LightHoverIconRole);

    m_mapFillModeItems["Full"] = stretchItem;/*铺满*/


    DStandardItem *centerItem = new DStandardItem(tr("Center"));
    //深色
    centerItem->setData("Center", FillModeRole);
    centerItem->setData(":/display/themes/dark/icons/dark/Center.svg", DarkItemIconRole);
    centerItem->setData(":/display/themes/dark/icons/dark/Center.svg", DarkDefaultIconRole);
    centerItem->setData(":/display/themes/dark/icons/white/Center.svg", DarkHighlightIconRole);
    centerItem->setData(":/display/themes/dark/icons/hover/Center.svg", DarkHoverIconRole);
    //浅色
    centerItem->setData(":/display/themes/light/icon/black/Center.svg", LightItemIconRole);
    centerItem->setData(":/display/themes/light/icon/light/Center.svg", LightDefaultIconRole);
    centerItem->setData(":/display/themes/light/icon/white/Center.svg", LightHighlightIconRole);
    centerItem->setData(":/display/themes/light/icon/hover/Center.svg", LightHoverIconRole);

    m_mapFillModeItems["Center"] = centerItem;/*居中*/
}

void ResolutionWidget::initResizeDesktop()
{

    if (m_monitor == nullptr) {
        return;
    }
    // 先断开信号，设置数据再连接信号
    disconnect(m_resizeDesktopCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);

    QStringList lstFillMode = m_monitor->availableFillModes();
    //获取最新的铺面方式
    QString fillMode = m_monitor->currentFillMode();
    if(fillMode.isEmpty())
        fillMode = "None";
    int index = lstFillMode.indexOf(fillMode);
    if(index >= 0)
        m_resizeDesktopCombox->setCurrentIndex(index);
    //用户手动选择铺满方式时发送信号
    connect(m_resizeDesktopCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        Q_EMIT requestSetFillMode(m_monitor, this->m_resizeDesktopCombox->itemData(idx,FillModeRole).toString());
    });
}

void ResolutionWidget::OnAvailableFillModesChanged(const QStringList &lstFillMode)
{
    disconnect(m_resizeDesktopCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);

    m_resizeItemModel->clear();
    setItemIcon();
    for(auto str : lstFillMode) {
        if(m_mapFillModeItems.keys().contains(str))
            m_resizeItemModel->appendRow(m_mapFillModeItems[str]);
    }
    m_resizeDesktopCombox->setDefaultRoleIcon();

    updateResizeDesktopVisible();
}

void ResolutionWidget::setResizeDesktopVisible(bool visible)
{
    if (m_model->displayMode() == MERGE_MODE && visible) {
        visible = m_model->allSupportFillModes();
    }

    m_resizeDesktopItem->setVisible(visible);
    if (!visible) {
        DConfigWatcher::instance()->erase(DConfigWatcher::display,"desktopDisplay", m_resizeDesktopItem);
    } else {
        DConfigWatcher::instance()->bind(DConfigWatcher::display,"desktopDisplay", m_resizeDesktopItem);
    }
}

bool ResolutionWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_resizeDesktopItem) {
        switch (event->type()) {
        case QEvent::Hide:
            setMinimumHeight(48);
            Q_EMIT requestResizeDesktopVisibleChanged(false);
            break;
        case QEvent::Show:
            setMinimumHeight(48 * 2);
            Q_EMIT requestResizeDesktopVisibleChanged(true);
            break;
        default:
            break;
        }
    }
    return SettingsItem::eventFilter(obj, event);
}

void ResolutionWidget::updateResizeDesktopVisible()
{
    //推荐分辨率下隐藏铺满方式
    if (m_resolutionCombox->currentText().contains(tr("Recommended"))) {
        setResizeDesktopVisible(false);
    } else {
        setResizeDesktopVisible(!m_monitor->availableFillModes().isEmpty());
    }
}

void ResolutionWidget::initResolution()
{
    if (m_monitor == nullptr) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_resoItemModel != nullptr) {
        disconnect(m_resolutionCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);
        m_resoItemModel->clear();
    }

    auto modeList = m_monitor->modeList();
    auto curMode = m_monitor->currentMode();

    Resolution preMode;
    if (qgetenv("WAYLAND_DISPLAY").isEmpty() ) {
        for (auto mode : modeList) {
            if (Monitor::isSameResolution(preMode, mode)) {
                continue;
            }

            preMode = mode;
            if (m_model->displayMode() == MERGE_MODE) {
                bool isComm = true;
                for (auto monitor : m_model->monitorList()) {
                    if (!monitor->hasResolution(mode)) {
                        isComm = false;
                        break;
                    }
                }

                if (!isComm) {
                    continue;
                }
            }

            auto *item = new DStandardItem;
            item->setData(QVariant(mode.id()), IdRole);
            item->setData(QVariant(mode.rate()), RateRole);
            item->setData(QVariant(mode.width()), WidthRole);
            item->setData(QVariant(mode.height()), HeightRole);

            auto res = QString::number(mode.width()) + "×" + QString::number(mode.height());
            if (Monitor::isSameResolution(mode, m_monitor->bestMode())) {
                item->setText(res + QString(" (%1)").arg(tr("Recommended")));
                m_resoItemModel->insertRow(0, item);
            } else {
                item->setText(res);
                m_resoItemModel->appendRow(item);
            }

            if (Monitor::isSameResolution(curMode, mode)) {
                m_resolutionCombox->setCurrentIndex(item->row());
            }
        }
    } else {
        bool first = true;
        for (auto mode : modeList) {
            if (Monitor::isSameResolution(preMode, mode)) {
                continue;
            }

            preMode = mode;
            if (m_model->displayMode() == MERGE_MODE) {
                bool isComm = true;
                for (auto monitor : m_model->monitorList()) {
                    if (!monitor->hasResolution(mode)) {
                        isComm = false;
                        break;
                    }
                }

                if (!isComm) {
                    continue;
                }
            }

            auto *item = new DStandardItem;
            item->setData(QVariant(mode.id()), IdRole);
            item->setData(QVariant(mode.rate()), RateRole);
            item->setData(QVariant(mode.width()), WidthRole);
            item->setData(QVariant(mode.height()), HeightRole);

            auto res = QString::number(mode.width()) + "×" + QString::number(mode.height());
            if (first) {
                first = false;
                item->setText(res + QString(" (%1)").arg(tr("Recommended")));
                m_resoItemModel->insertRow(0, item);
            } else {
                item->setText(res);
                m_resoItemModel->appendRow(item);
            }

            if (Monitor::isSameResolution(curMode, mode)) {
                m_resolutionCombox->setCurrentIndex(item->row());
            }
        }
    }

    // 如果当前设置的分辨率不在列表里面，将分辨率选择框置空
    if (!modeList.contains(curMode)) {
        m_resolutionCombox->setCurrentIndex(-1);
    }

    //推荐分辨率下隐藏铺满方式并改变高度
    updateResizeDesktopVisible();

    connect(m_resolutionCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        // 不处理空白选择项
        if (idx == -1) {
            return;
        }

        auto item = m_resoItemModel->item(idx);
        auto r = item->data(IdRole).toInt();
        auto w = item->data(WidthRole).toInt();
        auto h = item->data(HeightRole).toInt();

        //推荐分辨率下隐藏铺满方式并改变高度
        updateResizeDesktopVisible();

        // 选中分辨率和当前分别率相同
        if (m_monitor->currentMode().width() == w && m_monitor->currentMode().height() == h) {
            return;
        }

        // 优先设置最好模式
        if (m_monitor->bestMode().width() == w && m_monitor->bestMode().height() == h) {
            Q_EMIT requestSetResolution(m_monitor, m_monitor->bestMode().id());
            return;
        }

        for (auto mode : m_monitor->modeList()) {
            if (mode.width() == w && mode.height() == h) {
                Q_EMIT requestSetResolution(m_monitor, mode.id());
                return;
            }
        }
        Q_EMIT requestSetResolution(m_monitor, r);
    });
}
