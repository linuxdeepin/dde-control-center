/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     fanpengcheng <fanpengcheng@uniontech.com>
*
* Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
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

#include "dockplugin.h"
#include "dockdbusproxy.h"

#include "widgets/widgetmodule.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/titlelabel.h"

#include <DSlider>
#include <DTipLabel>
#include <DListView>
#include <DStyle>

#include <QApplication>
#include <QScreen>
#include <QScroller>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QStyle>
#include <QCheckBox>

DWIDGET_USE_NAMESPACE

enum DisplayMode {
    Fashion     = 0,    // 时尚模式
    Efficient   = 1,    // 高效模式
};

enum Position {
    Top         = 0,    // 上
    Right       = 1,    // 右
    Bottom      = 2,    // 下
    Left        = 3,    // 左
};

enum HideMode {
    KeepShowing     = 0,    // 一直显示
    KeepHidden      = 1,    // 一直隐藏
    SmartHide       = 2,    // 智能隐藏
};

DockPlugin::DockPlugin(QObject *parent)
    : PluginInterface(parent)
{

}

QString DockPlugin::name() const
{
    return QString("dock");
}

ModuleObject* DockPlugin::module()
{
    return new DockModuleObject();
}

QString DockPlugin::follow() const
{
    // 插入到个性化模块中，作为二级模块
    return QStringLiteral("personalization");
}

QString DockPlugin::location() const
{
    // 个性化的最后一个模块
    return "4";
}

DockModuleObject::DockModuleObject()
    : PageModule("dock", tr("Dock"), tr("Dock plugin"), nullptr)
{
    appendChild(new WidgetModule<ComboxWidget>("mode", tr("Mode"), this, &DockModuleObject::initMode));
    appendChild(new WidgetModule<ComboxWidget>("position", tr("Position"), this, &DockModuleObject::initPosition));
    appendChild(new WidgetModule<ComboxWidget>("status", tr("Status"), this, &DockModuleObject::initStatus));
    appendChild(new WidgetModule<QCheckBox>("recent", tr("Show Recent App"), this, &DockModuleObject::initShowRecent));
    appendChild(new WidgetModule<TitledSliderItem>("size", tr("Size"), this, &DockModuleObject::initSizeSlider));

    // 当任务栏服务未注册或当前只有一个屏幕或当前有多个屏幕但设置为复制模式时均不显示多屏设置项
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.dde.Dock")
            && QApplication::screens().size() > 1
            && !isCopyMode()) {
        appendChild(new WidgetModule<TitleLabel>("screenTitle", tr("ScreenTitle"), this, &DockModuleObject::initScreenTitle));
        appendChild(new WidgetModule<ComboxWidget>("screen", tr("Screen"), this, &DockModuleObject::initScreen));
    }

    // @note 不使用m_dbusProxy的原因在于module函数的调用和m_dbusProxy指针的初始化分别在不同的线程中
    QDBusInterface dockInter("com.deepin.dde.Dock", "/com/deepin/dde/Dock", "com.deepin.dde.Dock", QDBusConnection::sessionBus(), this);
    QDBusPendingReply<QStringList> reply = dockInter.asyncCall(QStringLiteral("GetLoadedPlugins"));
    reply.waitForFinished();
    QStringList plugins = reply.value();
    // 当対应服务异常或插件为空时，不显示对应模块信息
    if (reply.error().type() == QDBusError::ErrorType::NoError && reply.value().size() > 0) {
        appendChild(new WidgetModule<TitleLabel>("pluginTitle", tr("PluginTitle"), this, &DockModuleObject::initPluginTitle));
        appendChild(new WidgetModule<DTipLabel>("pluginTip", tr("PluginTip"), this, &DockModuleObject::initPluginTips));
        appendChild(new WidgetModule<DListView>("pluginArea", tr("PluginArea"), this, &DockModuleObject::initPluginView));
    }
}

void DockModuleObject::initMode(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    static QMap<QString, int> g_modeMap = {{tr("Fashion mode"), Fashion}
                                           , {tr("Efficient mode"), Efficient}};
    widget->setAccessibleName("Mode");
    widget->comboBox()->setAccessibleName("ModeCombox");
    widget->addBackground();
    widget->setTitle(tr("Mode"));
    widget->setComboxOption(QStringList() << tr("Fashion mode") << tr("Efficient mode"));
    widget->setCurrentText(g_modeMap.key(m_dbusProxy->displayMode()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [ = ] (const QString &text) {
        m_dbusProxy->setDisplayMode(g_modeMap.value(text));
    });

    connect(m_dbusProxy.get(), &DockDBusProxy::DisplayModeChanged, widget, [ = ] (int value) {
        DisplayMode mode = static_cast<DisplayMode>(value);
        if (g_modeMap.key(mode) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_modeMap.key(mode));
    });

}

void DockModuleObject::initPosition(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, int> g_positionMap = {{tr("Top"), Top}
                                               , {tr("Bottom"), Bottom}
                                               , {tr("Left"), Left}
                                               , {tr("Right"), Right}};
    widget->setAccessibleName("Location");
    widget->comboBox()->setAccessibleName("LocationCombox");
    widget->addBackground();
    widget->setTitle(tr("Location"));
    widget->setComboxOption(QStringList() << tr("Top") << tr("Bottom") << tr("Left") << tr("Right"));
    widget->setCurrentText(g_positionMap.key(m_dbusProxy->position()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [ = ] (const QString &text) {
        m_dbusProxy->setPosition(g_positionMap.value(text));
    });

    connect(m_dbusProxy.get(), &DockDBusProxy::PositionChanged, widget, [ = ] (int position) {
        if (g_positionMap.key(position) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_positionMap.key(position));
    });

}

void DockModuleObject::initStatus(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, int> g_stateMap = {{tr("Keep shown"), KeepShowing}
                                            , {tr("Keep hidden"), KeepHidden}
                                            , {tr("Smart hide"), SmartHide}};
    widget->setAccessibleName("Status");
    widget->comboBox()->setAccessibleName("StatusCombox");
    widget->addBackground();
    widget->setTitle(tr("Status"));
    widget->setComboxOption(QStringList() << tr("Keep shown") << tr("Keep hidden") << tr("Smart hide"));
    widget->setCurrentText(g_stateMap.key(m_dbusProxy->hideMode()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [ = ] (const QString &text) {
        m_dbusProxy->setHideMode(g_stateMap.value(text));
    });

    connect(m_dbusProxy.get(), &DockDBusProxy::HideModeChanged, widget, [ = ] (int hideMode) {
        if (g_stateMap.key(hideMode) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_stateMap.key(hideMode));
    });
}

void DockModuleObject::initShowRecent(QCheckBox *checkBox)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    checkBox->blockSignals(true);
    checkBox->setChecked(m_dbusProxy->showRecent());
    checkBox->blockSignals(false);
    checkBox->setText(tr("Show Recent App"));
    connect(checkBox, &QCheckBox::toggled, checkBox, [ this ](bool checked) {
        m_dbusProxy->blockSignals(true);
        m_dbusProxy->SetShowRecent(checked);
        m_dbusProxy->blockSignals(false);
    });
    connect(m_dbusProxy.get(), &DockDBusProxy::showRecentChanged, checkBox, [ = ](bool checked) {
        checkBox->blockSignals(true);
        checkBox->setChecked(checked);
        checkBox->blockSignals(false);
    });
}

void DockModuleObject::initSizeSlider(TitledSliderItem *slider)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    slider->setAccessibleName("Slider");
    slider->addBackground();
    slider->slider()->setRange(40, 100);
    QStringList ranges;
    ranges << tr("Small") << "" << tr("Large");
    slider->setAnnotations(ranges);

    auto updateSliderValue = [ = ] {
        auto displayMode = m_dbusProxy->displayMode();

        slider->slider()->blockSignals(true);
        if (displayMode == DisplayMode::Fashion) {
            if (int(m_dbusProxy->windowSizeFashion()) != slider->slider()->value())
                slider->slider()->setValue(int(m_dbusProxy->windowSizeFashion()));
        } else if (displayMode == DisplayMode::Efficient) {
            if (int(m_dbusProxy->windowSizeEfficient()) != slider->slider()->value())
                slider->slider()->setValue(int(m_dbusProxy->windowSizeEfficient()));
        }
        slider->slider()->blockSignals(false);
    };

    connect(m_dbusProxy.get(), &DockDBusProxy::DisplayModeChanged, slider, [ = ] {updateSliderValue();});
    connect(m_dbusProxy.get(), &DockDBusProxy::WindowSizeFashionChanged, slider, [ = ] {updateSliderValue();});
    connect(m_dbusProxy.get(), &DockDBusProxy::WindowSizeEfficientChanged, slider, [ = ] {updateSliderValue();});
    connect(slider->slider(), &DSlider::sliderMoved, slider->slider(), &DSlider::valueChanged);
    connect(slider->slider(), &DSlider::valueChanged, m_dbusProxy.get(), [ = ] (int value) {
        m_dbusProxy->resizeDock(value, true);
    });
    connect(slider->slider(), &DSlider::sliderPressed, m_dbusProxy.get(), [ = ] {
        m_dbusProxy->blockSignals(true);
    });
    connect(slider->slider(), &DSlider::sliderReleased, m_dbusProxy.get(), [ = ] {
        m_dbusProxy->blockSignals(false);

        // 松开手后通知dock拖拽状态解除
        QMetaObject::invokeMethod(this, [ = ] {
            int offset = slider->slider()->value();
            m_dbusProxy->resizeDock(offset, false);
        }, Qt::QueuedConnection);
    });

    updateSliderValue();
}

void DockModuleObject::initScreenTitle(TitleLabel *label)
{
    label->setAccessibleName("MultipleDisplays");
    label->setText(tr("Multiple Displays"));

    connect(qApp, &QApplication::screenAdded, label, [ = ] {
        label->setVisible(qApp->screens().count() > 1);
    });
    connect(qApp, &QApplication::screenRemoved, label, [ = ] {
        label->setVisible(qApp->screens().count() > 1);
    });
}

void DockModuleObject::initScreen(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, bool> g_screenSettingMap = {{tr("On screen where the cursor is"), false}
                                                     , {tr("Only on main screen"), true}};
    widget->setAccessibleName("ShowDock");
    widget->comboBox()->setAccessibleName("ShowDockCombox");
    widget->setTitle(tr("Show Dock"));
    widget->addBackground();
    widget->setComboxOption(QStringList() << tr("On screen where the cursor is") << tr("Only on main screen"));
    widget->setCurrentText(g_screenSettingMap.key(m_dbusProxy->showInPrimary()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [ = ] (const QString &text) {
        m_dbusProxy->setShowInPrimary(g_screenSettingMap.value(text));
    });
    connect(qApp, &QApplication::screenAdded, widget, [ = ] {
        widget->setVisible(qApp->screens().count() > 1);
    });
    connect(qApp, &QApplication::screenRemoved, widget, [ = ] {
        widget->setVisible(qApp->screens().count() > 1);
    });

    // 这里不会生效，但实际场景中也不存在有其他可配置的地方，暂时不用处理
    connect(m_dbusProxy.get(), &DockDBusProxy::ShowInPrimaryChanged, widget, [ = ] (bool showInPrimary) {
        if (widget->comboBox()->currentText() == g_screenSettingMap.key(showInPrimary))
            return;

        widget->blockSignals(true);
        widget->setCurrentText(g_screenSettingMap.key(showInPrimary));
        widget->blockSignals(false);
    });
}

void DockModuleObject::initPluginTitle(TitleLabel *label)
{
    label->setAccessibleName("PluginArea");
    label->setText(tr("Plugin Area"));
}

void DockModuleObject::initPluginTips(DTipLabel *label)
{
    label->setAccessibleName("PluginTips");
    label->setText(tr("Select which icons appear in the Dock"));
    label->adjustSize();
    label->setWordWrap(true);
    label->setContentsMargins(10, 5, 10, 5);
    label->setAlignment(Qt::AlignLeft);
}

void DockModuleObject::initPluginView(DListView *view)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    QDBusPendingReply<QStringList> reply = m_dbusProxy->GetLoadedPlugins();
    QStringList plugins = reply.value();
    const QMap<QString, QString> &pluginIconMap = {{"AiAssistant",      "dcc_dock_assistant"}
                                                   , {"show-desktop",   "dcc_dock_desktop"}
                                                   , {"onboard",        "dcc_dock_keyboard"}
                                                   , {"notifications",  "dcc_dock_notify"}
                                                   , {"shutdown",       "dcc_dock_power"}
                                                   , {"multitasking",   "dcc_dock_task"}
                                                   , {"datetime",       "dcc_dock_time"}
                                                   , {"system-monitor", "dcc_dock_systemmonitor"}
                                                   , {"grand-search",   "dcc_dock_grandsearch"}
                                                   , {"trash",          "dcc_dock_trash"}};

    view->setAccessibleName("PluginList");
    view->setAccessibleName("pluginList");
    view->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setSelectionMode(QListView::SelectionMode::NoSelection);
    view->setEditTriggers(DListView::NoEditTriggers);
    view->setFrameShape(DListView::NoFrame);
    view->setViewportMargins(0, 0, 0, 0);
    view->setItemSpacing(1);

    QMargins itemMargins(view->itemMargins());
    itemMargins.setLeft(14);
    view->setItemMargins(itemMargins);

    view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QScroller *scroller = QScroller::scroller(view->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    QStandardItemModel *pluginModel = new QStandardItemModel(this);
    view->setModel(pluginModel);

    auto updateItemCheckStatus = [ = ] (const QString &name, bool visible) {
        for (int i = 0; i < pluginModel->rowCount(); ++i) {
            auto item = static_cast<DStandardItem *>(pluginModel->item(i));
            if (item->text() != name || item->actionList(Qt::Edge::RightEdge).size() < 1)
                continue;

            auto action = item->actionList(Qt::Edge::RightEdge).first();
            auto checkstatus = visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked ;
            auto icon = qobject_cast<DStyle *>(qApp->style())->standardIcon(checkstatus);
            action->setIcon(icon);
            view->update(item->index());
            break;
        }
    };

    for (auto name : plugins) {
        DStandardItem *item = new DStandardItem(name);
        item->setFontSize(DFontSizeManager::T8);
        QSize size(16, 16);

        // 插件图标
        auto leftAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
        leftAction->setIcon(QIcon::fromTheme(pluginIconMap.value(m_dbusProxy->getPluginKey(name), "dcc_dock_plug_in")));
        item->setActionList(Qt::Edge::LeftEdge, {leftAction});

        auto rightAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
        bool visible = m_dbusProxy->getPluginVisible(name);
        auto checkstatus = visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked ;
        auto checkIcon = qobject_cast<DStyle *>(qApp->style())->standardIcon(checkstatus);
        rightAction->setIcon(checkIcon);
        item->setActionList(Qt::Edge::RightEdge, {rightAction});
        pluginModel->appendRow(item);

        connect(rightAction, &DViewItemAction::triggered, view, [ = ] {
            bool checked = m_dbusProxy->getPluginVisible(name);
            m_dbusProxy->setPluginVisible(name, !checked);
            updateItemCheckStatus(name, !checked);
        });
    }
    // 固定大小,防止滚动
    int lineHeight = view->visualRect(view->indexAt(QPoint(0, 0))).height();
    view->setMinimumHeight(lineHeight * plugins.size() + 10);

    connect(m_dbusProxy.get(), &DockDBusProxy::pluginVisibleChanged, view, std::bind(updateItemCheckStatus, std::placeholders::_1, std::placeholders::_2));
}

/**判断屏幕是否为复制模式的依据，第一个屏幕的X和Y值是否和其他的屏幕的X和Y值相等
 * 对于复制模式，这两个值肯定是相等的，如果不是复制模式，这两个值肯定不等，目前支持双屏
 * @brief DisplayManager::isCopyMode
 * @return
 */
bool DockModuleObject::isCopyMode()
{
    QList<QScreen *> screens = qApp->screens();
    if (screens.size() < 2)
        return false;

    // 在多个屏幕的情况下，如果所有屏幕的位置的X和Y值都相等，则认为是复制模式
    QRect screenRect = screens[0]->availableGeometry();
    for (int i = 1; i < screens.size(); i++) {
        QRect rect = screens[i]->availableGeometry();
        if (screenRect.x() != rect.x() || screenRect.y() != rect.y())
            return false;
    }

    return true;
}

