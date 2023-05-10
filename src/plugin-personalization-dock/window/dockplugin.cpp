//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "dockplugin.h"
#include "dockdbusproxy.h"

#include "widgets/itemmodule.h"
#include "widgets/widgetmodule.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/titlelabel.h"

#include <DSlider>
#include <DTipLabel>
#include <DListView>
#include <DStyle>
#include <DGuiApplicationHelper>

#include <QApplication>
#include <QScreen>
#include <QScroller>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QDBusMessage>
#include <QStyle>
#include <QCheckBox>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;

enum DisplayMode
{
    Fashion = 0,   // 时尚模式
    Efficient = 1, // 高效模式
};

enum Position
{
    Top = 0,    // 上
    Right = 1,  // 右
    Bottom = 2, // 下
    Left = 3,   // 左
};

enum HideMode
{
    KeepShowing = 0, // 一直显示
    KeepHidden = 1,  // 一直隐藏
    SmartHide = 2,   // 智能隐藏
};

DockPlugin::DockPlugin(QObject *parent)
    : PluginInterface(parent)
{
}

QString DockPlugin::name() const
{
    return QString("dock");
}

ModuleObject *DockPlugin::module()
{
    return new DockModuleObject();
}

QString DockPlugin::follow() const
{
    // 插入到个性化/桌面模块中，作为三级模块
    return QStringLiteral("personalization/desktop");
}

QString DockPlugin::location() const
{
    // 桌面的第一个模块
    return "0";
}

DockModuleObject::DockModuleObject()
    : PageModule("dock", tr("Dock"), QString(), nullptr), m_screenTitle(new ItemModule("screenTitle", tr("Multiple Displays"))), m_screen(new ItemModule("screen", tr("Show Dock"), this, &DockModuleObject::initScreen))
{
    setNoScroll();
    setNoStretch();
    setContentsMargins(0, 0, 0, 0);

    appendChild(new ItemModule("title", tr("Dock")));
    appendChild(new WidgetModule<ComboxWidget>("mode", tr("Mode"), this, &DockModuleObject::initMode));
    appendChild(new WidgetModule<ComboxWidget>("position", tr("Position"), this, &DockModuleObject::initPosition));
    appendChild(new WidgetModule<ComboxWidget>("status", tr("Status"), this, &DockModuleObject::initStatus));
    appendChild(new WidgetModule<QCheckBox>("recent", tr("Show recent apps in Dock"), this, &DockModuleObject::initShowRecent));
    appendChild(new WidgetModule<TitledSliderItem>("size", tr("Size"), this, &DockModuleObject::initSizeSlider));

    m_screen->setBackground(true);
    appendChild(m_screenTitle);
    appendChild(m_screen);

    m_displayProxy.reset(new QDBusInterface("org.deepin.dde.Display1", "/org/deepin/dde/Display1",
                                            "org.deepin.dde.Display1", QDBusConnection::sessionBus(), this));
    // 当任务栏服务未注册或当前只有一个屏幕或当前有多个屏幕但设置为复制模式时均不显示多屏设置项
    connect(qApp, &QApplication::screenAdded, this, &DockModuleObject::updateScreenVisible);
    connect(qApp, &QApplication::screenRemoved, this, &DockModuleObject::updateScreenVisible);
    QDBusConnection::sessionBus().connect("org.deepin.dde.Display1", "/org/deepin/dde/Display1",
                                          "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
                                          this, SLOT(onDisplayPropertiesChanged(const QDBusMessage &)));
    updateScreenVisible();

    DockDBusProxy::regiestDockItemType();
    // @note 不使用m_dbusProxy的原因在于module函数的调用和m_dbusProxy指针的初始化分别在不同的线程中
    QDBusInterface dockInter("org.deepin.dde.Dock1", "/org/deepin/dde/Dock1", "org.deepin.dde.Dock1", QDBusConnection::sessionBus(), this);
    QDBusPendingReply<DockItemInfos> reply = dockInter.asyncCall(QStringLiteral("plugins"));
    reply.waitForFinished();
    DockItemInfos plugins = reply.value();
    // 当対应服务异常或插件为空时，不显示对应模块信息
    if (reply.error().type() == QDBusError::ErrorType::NoError && plugins.size() > 0)
    {
        appendChild(new WidgetModule<TitleLabel>("pluginTitle", tr("Plugin Area"), this, &DockModuleObject::initPluginTitle));
        appendChild(new WidgetModule<DTipLabel>("pluginTip", tr("Select which icons appear in the Dock"), this, &DockModuleObject::initPluginTips));
        appendChild(new WidgetModule<DListView>("pluginArea", QString(), this, &DockModuleObject::initPluginView));
    }
}

QIcon DockModuleObject::getIcon(const DockItemInfo &dockItemInfo) const
{
    static const QMap<QString, QString> &pluginIconMap = {{"AiAssistant", "dcc_dock_assistant"}, {"show-desktop", "dcc_dock_desktop"}, {"onboard", "dcc_dock_keyboard"}, {"notifications", "dcc_dock_notify"}, {"shutdown", "dcc_dock_power"}, {"multitasking", "dcc_dock_task"}, {"system-monitor", "dcc_dock_systemmonitor"}, {"grand-search", "dcc_dock_grandsearch"}, {"trash", "dcc_dock_trash"}};
    QPixmap pixmap;
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::ColorType::LightType)
        pixmap.loadFromData(dockItemInfo.iconLight);
    else
        pixmap.loadFromData(dockItemInfo.iconDark);

    QIcon icon(pixmap);
    if (icon.isNull())
        icon = QIcon::fromTheme(pluginIconMap.value(dockItemInfo.name));
    if (icon.isNull())
        icon = QIcon::fromTheme("dcc_dock_plug_in");

    return icon;
}

void DockModuleObject::initMode(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    static QMap<QString, int> g_modeMap = {{tr("Fashion mode"), Fashion}, {tr("Efficient mode"), Efficient}};
    widget->setAccessibleName("Mode");
    widget->comboBox()->setAccessibleName("ModeCombox");
    widget->addBackground();
    widget->setTitle(tr("Mode"));
    widget->setComboxOption(QStringList() << tr("Fashion mode") << tr("Efficient mode"));
    widget->setCurrentText(g_modeMap.key(m_dbusProxy->displayMode()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [=](const QString &text)
            { m_dbusProxy->setDisplayMode(g_modeMap.value(text)); });

    connect(m_dbusProxy.get(), &DockDBusProxy::DisplayModeChanged, widget, [=](int value)
            {
        DisplayMode mode = static_cast<DisplayMode>(value);
        if (g_modeMap.key(mode) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_modeMap.key(mode)); });
}

void DockModuleObject::initPosition(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, int> g_positionMap = {{tr("Top"), Top}, {tr("Bottom"), Bottom}, {tr("Left"), Left}, {tr("Right"), Right}};
    widget->setAccessibleName("Location");
    widget->comboBox()->setAccessibleName("LocationCombox");
    widget->addBackground();
    widget->setTitle(tr("Location"));
    widget->setComboxOption(QStringList() << tr("Top") << tr("Bottom") << tr("Left") << tr("Right"));
    widget->setCurrentText(g_positionMap.key(m_dbusProxy->position()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [=](const QString &text)
            { m_dbusProxy->setPosition(g_positionMap.value(text)); });

    connect(m_dbusProxy.get(), &DockDBusProxy::PositionChanged, widget, [=](int position)
            {
        if (g_positionMap.key(position) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_positionMap.key(position)); });
}

void DockModuleObject::initStatus(ComboxWidget *widget)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, int> g_stateMap = {{tr("Keep shown"), KeepShowing}, {tr("Keep hidden"), KeepHidden}, {tr("Smart hide"), SmartHide}};
    widget->setAccessibleName("Status");
    widget->comboBox()->setAccessibleName("StatusCombox");
    widget->addBackground();
    widget->setTitle(tr("Status"));
    widget->setComboxOption(QStringList() << tr("Keep shown") << tr("Keep hidden") << tr("Smart hide"));
    widget->setCurrentText(g_stateMap.key(m_dbusProxy->hideMode()));
    connect(widget, &ComboxWidget::onSelectChanged, m_dbusProxy.get(), [=](const QString &text)
            { m_dbusProxy->setHideMode(g_stateMap.value(text)); });

    connect(m_dbusProxy.get(), &DockDBusProxy::HideModeChanged, widget, [=](int hideMode)
            {
        if (g_stateMap.key(hideMode) == widget->comboBox()->currentText())
            return;

        widget->setCurrentText(g_stateMap.key(hideMode)); });
}

void DockModuleObject::initShowRecent(QCheckBox *checkBox)
{
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    checkBox->blockSignals(true);
    checkBox->setChecked(m_dbusProxy->showRecent());
    checkBox->blockSignals(false);
    checkBox->setText(tr("Show recent apps in Dock"));
    connect(checkBox, &QCheckBox::toggled, checkBox, [this](bool checked)
            {
        m_dbusProxy->blockSignals(true);
        m_dbusProxy->SetShowRecent(checked);
        m_dbusProxy->blockSignals(false); });
    connect(m_dbusProxy.get(), &DockDBusProxy::showRecentChanged, checkBox, [=](bool checked)
            {
        checkBox->blockSignals(true);
        checkBox->setChecked(checked);
        checkBox->blockSignals(false); });
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

    auto updateSliderValue = [=]
    {
        auto displayMode = m_dbusProxy->displayMode();

        slider->slider()->blockSignals(true);
        if (displayMode == DisplayMode::Fashion)
        {
            if (int(m_dbusProxy->windowSizeFashion()) != slider->slider()->value())
                slider->slider()->setValue(int(m_dbusProxy->windowSizeFashion()));
        }
        else if (displayMode == DisplayMode::Efficient)
        {
            if (int(m_dbusProxy->windowSizeEfficient()) != slider->slider()->value())
                slider->slider()->setValue(int(m_dbusProxy->windowSizeEfficient()));
        }
        slider->slider()->blockSignals(false);
    };

    connect(m_dbusProxy.get(), &DockDBusProxy::DisplayModeChanged, slider, [=]
            { updateSliderValue(); });
    connect(m_dbusProxy.get(), &DockDBusProxy::WindowSizeFashionChanged, slider, [=]
            { updateSliderValue(); });
    connect(m_dbusProxy.get(), &DockDBusProxy::WindowSizeEfficientChanged, slider, [=]
            { updateSliderValue(); });
    connect(slider->slider(), &DSlider::sliderMoved, slider->slider(), &DSlider::valueChanged);
    connect(slider->slider(), &DSlider::valueChanged, m_dbusProxy.get(), [=](int value)
            { m_dbusProxy->resizeDock(value, true); });
    connect(slider->slider(), &DSlider::sliderPressed, m_dbusProxy.get(), [=]
            { m_dbusProxy->blockSignals(true); });
    connect(slider->slider(), &DSlider::sliderReleased, m_dbusProxy.get(), [=]
            {
        m_dbusProxy->blockSignals(false);

        // 松开手后通知dock拖拽状态解除
        QMetaObject::invokeMethod(this, [ = ] {
            int offset = slider->slider()->value();
            m_dbusProxy->resizeDock(offset, false);
        }, Qt::QueuedConnection); });

    updateSliderValue();
}

void DockModuleObject::initScreenTitle(TitleLabel *label)
{
    label->setAccessibleName("MultipleDisplays");
    label->setText(tr("Multiple Displays"));

    connect(qApp, &QApplication::screenAdded, label, [=]
            { label->setVisible(qApp->screens().count() > 1); });
    connect(qApp, &QApplication::screenRemoved, label, [=]
            { label->setVisible(qApp->screens().count() > 1); });
}

QWidget *DockModuleObject::initScreen(DCC_NAMESPACE::ModuleObject *module)
{
    Q_UNUSED(module)
    if (m_dbusProxy.isNull())
        m_dbusProxy.reset(new DockDBusProxy);

    const QMap<QString, bool> g_screenSettingMap = {{tr("On screen where the cursor is"), false}, {tr("Only on main screen"), true}};

    QComboBox *widget = new QComboBox();
    widget->setAccessibleName("ShowDock");
    widget->setAccessibleName("ShowDockCombox");
    widget->addItems(QStringList() << tr("On screen where the cursor is") << tr("Only on main screen"));
    widget->setCurrentText(g_screenSettingMap.key(m_dbusProxy->showInPrimary()));
    connect(widget, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_dbusProxy.get(), [=](int index)
            {
        const QString &text = widget->itemText(index);
        m_dbusProxy->setShowInPrimary(g_screenSettingMap.value(text)); });
    connect(qApp, &QApplication::screenAdded, widget, [widget]
            { widget->setVisible(qApp->screens().count() > 1); });
    connect(qApp, &QApplication::screenRemoved, widget, [widget]
            { widget->setVisible(qApp->screens().count() > 1); });

    // 这里不会生效，但实际场景中也不存在有其他可配置的地方，暂时不用处理
    connect(m_dbusProxy.get(), &DockDBusProxy::ShowInPrimaryChanged, widget, [widget, g_screenSettingMap](bool showInPrimary)
            {
        if (widget->currentText() == g_screenSettingMap.key(showInPrimary))
            return;

        widget->blockSignals(true);
        widget->setCurrentText(g_screenSettingMap.key(showInPrimary));
        widget->blockSignals(false); });
    return widget;
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

    QDBusPendingReply<DockItemInfos> reply = m_dbusProxy->plugins();
    DockItemInfos plugins = reply.value();

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

    auto updateItemCheckStatus = [=](const QString &name, bool visible)
    {
        for (int i = 0; i < pluginModel->rowCount(); ++i)
        {
            auto item = static_cast<DStandardItem *>(pluginModel->item(i));
            if (item->text() != name || item->actionList(Qt::Edge::RightEdge).size() < 1)
                continue;

            auto action = item->actionList(Qt::Edge::RightEdge).first();
            auto checkstatus = visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
            auto icon = qobject_cast<DStyle *>(qApp->style())->standardIcon(checkstatus);
            action->setIcon(icon);
            view->update(item->index());
            break;
        }
    };

    for (DockItemInfo dockItem : plugins)
    {
        DStandardItem *item = new DStandardItem(dockItem.displayName);
        item->setFontSize(DFontSizeManager::T8);
        QSize size(16, 16);

        // 插件图标
        auto leftAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
        leftAction->setIcon(getIcon(dockItem));
        item->setActionList(Qt::Edge::LeftEdge, {leftAction});

        auto rightAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
        auto checkstatus = dockItem.visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
        auto checkIcon = qobject_cast<DStyle *>(qApp->style())->standardIcon(checkstatus);
        rightAction->setIcon(checkIcon);
        item->setActionList(Qt::Edge::RightEdge, {rightAction});
        pluginModel->appendRow(item);

        item->setData(dockItem.visible, Dtk::UserRole + 1);

        connect(rightAction, &DViewItemAction::triggered, view, [=]
                {
            bool visible = !item->data(Dtk::UserRole + 1).toBool();
            m_dbusProxy->setItemOnDock(dockItem.settingKey, dockItem.itemKey, visible);
            updateItemCheckStatus(dockItem.displayName, visible);
            item->setData(visible, Dtk::UserRole + 1); });
        // 主题发生变化触发的信号
        connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged, leftAction, [leftAction, this, dockItem]()
                { leftAction->setIcon(getIcon(dockItem)); });
    }
    // 固定大小,防止滚动
    int lineHeight = view->visualRect(view->indexAt(QPoint(0, 0))).height();
    view->setMinimumHeight(lineHeight * plugins.size() + 10);

    connect(m_dbusProxy.get(), &DockDBusProxy::pluginVisibleChanged, view, std::bind(updateItemCheckStatus, std::placeholders::_1, std::placeholders::_2));
}

void DockModuleObject::onDisplayPropertiesChanged(const QDBusMessage &dbusMessage)
{
    QList<QVariant> arguments = dbusMessage.arguments();
    // 参数固定长度
    if (3 != arguments.count())
        return;

    QString interfaceName = arguments.first().toString();
    if (interfaceName != "org.deepin.dde.Display1")
        return;

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    QStringList keys = changedProps.keys();
    if (keys.contains("DisplayMode"))
        updateScreenVisible();
}

void DockModuleObject::updateScreenVisible()
{
    uint displayMode = m_displayProxy->property("DisplayMode").toUInt();
    bool screenIsShow = (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.deepin.dde.Dock1") && QApplication::screens().size() > 1 && displayMode == 2);

    m_screenTitle->setHidden(!screenIsShow);
    m_screen->setHidden(!screenIsShow);
}
