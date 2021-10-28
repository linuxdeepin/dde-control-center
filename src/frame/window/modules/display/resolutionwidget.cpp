/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#include "resolutionwidget.h"
#include "modules/display/displaymodel.h"

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
    , m_resolutionLabel(new QLabel(tr("Resolution"), this))
    , m_resolutionCombox(new QComboBox(this))
    , m_resizeDesktopLabel(new QLabel(tr("Resize Desktop"), this))
    , m_resizeDesktopCombox(new fillModeCombox(this))
    , m_resizeDesktopItem(new SettingsItem)
    , m_model(nullptr)
    , m_monitor(nullptr)
    , m_resoItemModel(new QStandardItemModel)
    , m_resizeItemModel(new QStandardItemModel)
{
    setAccessibleName("ResolutionWidget");
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
    //resizeDesktopItem = new SettingsItem;
    m_resizeDesktopLayout->setContentsMargins(10, 10, 10, 10);
    m_resizeDesktopLayout->addWidget(m_resizeDesktopLabel);
    m_resizeDesktopLayout->addWidget(m_resizeDesktopCombox);
    m_resizeDesktopCombox->setFocusPolicy(Qt::NoFocus);
    m_resizeDesktopCombox->setMinimumWidth(comboxWidth);
    m_resizeDesktopCombox->setMinimumHeight(36);
    m_resizeDesktopCombox->setModel(m_resizeItemModel);
    m_resizeDesktopItem->setLayout(m_resizeDesktopLayout);

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
        disconnect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
        disconnect(m_monitor, &Monitor::bestModeChanged, this, &ResolutionWidget::initResolution);
        disconnect(m_monitor, &Monitor::currentModeChanged, this, nullptr);
        disconnect(m_monitor, &Monitor::currentFillModeChanged, this, &ResolutionWidget::initResizeDesktop);
    }

    m_monitor = monitor;

    initResolution();
    OnAvailableFillModesChanged(m_monitor->availableFillModes());
    initResizeDesktop();

    connect(m_monitor, &Monitor::currentFillModeChanged, this, &ResolutionWidget::initResizeDesktop);

    connect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
    connect(m_monitor, &Monitor::bestModeChanged, this, &ResolutionWidget::initResolution);
    connect(m_monitor, &Monitor::currentModeChanged, this, [=](const Resolution &mode) {
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
    });
}

void ResolutionWidget::setItemIcon()
{
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
        fillMode = "Full aspect";
    int index = lstFillMode.indexOf(fillMode);
    if(index >= 0)
        m_resizeDesktopCombox->setCurrentIndex(index);

    connect(m_monitor, &Monitor::availableFillModesChanged, this, &ResolutionWidget::OnAvailableFillModesChanged);

    //用户手动选择铺满方式时发送信号
    connect(m_resizeDesktopCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        if (m_model->displayMode() == MERGE_MODE) {
            for (auto monitor : m_model->monitorList()) {
                Q_EMIT requestSetFillMode(monitor, this->m_resizeDesktopCombox->itemData(idx,FillModeRole).toString());
            }
        }
        else {
            Q_EMIT requestSetFillMode(m_monitor, this->m_resizeDesktopCombox->itemData(idx,FillModeRole).toString());
        }
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

}

void ResolutionWidget::resolutionWidgetChanged()
{
    //推荐分辨率下隐藏铺满方式
    if (m_resolutionCombox->currentText().contains(tr("Recommended"))) {
        setMinimumHeight(48);
        m_resizeDesktopItem->setVisible(false);
    } else {
        setMinimumHeight(48*2);
        m_resizeDesktopItem->setVisible(true);
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

    //推荐分辨率下隐藏铺满方式并改变高度
    resolutionWidgetChanged();

    connect(m_resolutionCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        auto item = m_resoItemModel->item(idx);
        auto r = item->data(IdRole).toInt();
        auto w = item->data(WidthRole).toInt();
        auto h = item->data(HeightRole).toInt();

        //推荐分辨率下隐藏铺满方式并改变高度
        resolutionWidgetChanged();

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
