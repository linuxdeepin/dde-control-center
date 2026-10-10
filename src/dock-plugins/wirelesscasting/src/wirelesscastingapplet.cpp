// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wirelesscastingapplet.h"
#include "wirelesscastingmodel.h"
#include "constants.h"

#include <qnamespace.h>

#include <DPaletteHelper>
#include <DSpinner>
#include <DIcon>
#include <DGuiApplicationHelper>

#include <QBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QScroller>
#include <QTextLayout>
#include <QTextLine>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

namespace dde {
namespace wirelesscasting {

WirelessCastingApplet::WirelessCastingApplet(WirelessCastingModel *model, DisplayModel *displayMode, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_displayModel(displayMode)
    , m_contentWidget(new QWidget(this))
    , m_scrollArea(new QScrollArea(this))
#ifdef WIRELESS_CASTING_ENABLED
    , m_refresh(new CommonIconButton(this))
    , m_wirelesscastingWidget(new QWidget(this))
    , m_wirelesscastingTitle(new QWidget(this))
    , m_monitorsListView(new MonitorListView(this))
    , m_monitorsModel(new QStandardItemModel(m_monitorsListView))
    , m_statePanel(new StatePanel(m_model, this))
#endif
    , m_multiscreenOptionsWidget(new QWidget(this))
    , m_multiscreenOptionsTitle(new QWidget(this))
    , m_multiscreenOptionsListView(new MonitorListView(this))
    , m_multiscreenOptionsModel(new QStandardItemModel(m_multiscreenOptionsListView))
    , m_showOnDock(true)
    , m_displaySetting(new JumpSettingButton(this))
    , m_lastConnMonitor(nullptr)
    , m_cfgShowCasting(true)
    , m_cfgEnabled(true)
    , m_minHeight(-1)
{
    initUI();
    initMonitors();
#ifdef WIRELESS_CASTING_ENABLED
    qRegisterMetaType<WirelessCastingModel::CastingState>("WirelessCastingModel::CastingState");
    connect(m_model, &WirelessCastingModel::stateChanged, this, &WirelessCastingApplet::onStateChanged);
    connect(m_model, &WirelessCastingModel::stateChangeFinished, this, &WirelessCastingApplet::resizeApplet);
    connect(m_statePanel, &StatePanel::disconnMonitor, this, [this]() {
        m_model->disconnectMonitor();
        resizeApplet();
    });
    connect(m_model, &WirelessCastingModel::stateChanged, this, [this](WirelessCastingModel::CastingState state) {
        Q_EMIT castingChanged(state == WirelessCastingModel::Connected);
        resizeApplet();
    });
    onStateChanged(m_model->state());
#endif
}

void WirelessCastingApplet::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setAlignment(Qt::AlignCenter);
    centralLayout->setContentsMargins(0, 10, 0, 10);
    centralLayout->setSpacing(0);

#ifdef WIRELESS_CASTING_ENABLED
    // 无线投屏
    m_wirelesscastingTitle->setFixedHeight(24);
    QLabel *wirelesscastingTitle = new QLabel(tr("Wireless Casting"), this);
    DFontSizeManager::instance()->bind(wirelesscastingTitle, DFontSizeManager::T9);
    m_refresh->setAccessibleName("refresh");
    m_refresh->setFixedSize(16, 16);
    m_refresh->setIcon(QIcon::fromTheme("refresh"));
    m_refresh->setFocusPolicy(Qt::NoFocus);
    m_refresh->setClickable(true);
    m_refresh->setRotatable(true);
    m_refresh->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    QHBoxLayout *wirelesscastingTitleLayout = new QHBoxLayout(m_wirelesscastingTitle);
    wirelesscastingTitleLayout->setSpacing(0);
    wirelesscastingTitleLayout->setContentsMargins(10, 0, 0, 0);
    wirelesscastingTitleLayout->addWidget(wirelesscastingTitle);
    wirelesscastingTitleLayout->addWidget(m_refresh);

    m_monitorsListView->setModel(m_monitorsModel);
    m_monitorsListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_monitorsListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_monitorsListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout *wirelesscastingLayout = new QVBoxLayout(m_wirelesscastingWidget);
    wirelesscastingLayout->setSpacing(5);
    wirelesscastingLayout->setContentsMargins(0, 0, 0, 0);
    wirelesscastingLayout->addWidget(m_wirelesscastingTitle);
    wirelesscastingLayout->addWidget(m_statePanel);
    wirelesscastingLayout->addWidget(m_monitorsListView);

    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(10, 0, 10, 0);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(m_wirelesscastingWidget);
    m_wirelesscastingWidget->setVisible(false);
#else
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(10, 0, 10, 0);
    contentLayout->setSpacing(10);
    m_multiscreenOptionsTitle->setFixedHeight(24);
#endif
    // 多屏选项
    QLabel *multiscreenOptionsTitle = new QLabel(tr("Multiple Display options"), this);
    DFontSizeManager::instance()->bind(multiscreenOptionsTitle, DFontSizeManager::T9);
    m_multiscreenOptionsWidget->setVisible(false);
    m_multiscreenOptionsListView->setItemDelegate(new MultiscreenOptionItemDelegate(m_multiscreenOptionsListView));
    m_multiscreenOptionsListView->setObjectName("MultiscreenOptionsListView");
    m_multiscreenOptionsListView->setModel(m_multiscreenOptionsModel);
    m_multiscreenOptionsListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_multiscreenOptionsListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_multiscreenOptionsListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout *multiscreenOptionsLayout = new QVBoxLayout(m_multiscreenOptionsWidget);
    multiscreenOptionsLayout->setSpacing(0);
    multiscreenOptionsLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *multiscreenOptionsTitleLayout = new QHBoxLayout(m_multiscreenOptionsTitle);
    multiscreenOptionsTitleLayout->setSpacing(0);
    multiscreenOptionsTitleLayout->setContentsMargins(10, 0, 0, 0);
    multiscreenOptionsTitleLayout->addWidget(multiscreenOptionsTitle);
    multiscreenOptionsLayout->addWidget(m_multiscreenOptionsTitle);
    multiscreenOptionsLayout->addSpacing(5);
    multiscreenOptionsLayout->addWidget(m_multiscreenOptionsListView);
    contentLayout->addWidget(m_multiscreenOptionsWidget);
    contentLayout->addStretch();

    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidget(m_contentWidget);
    m_contentWidget->setAttribute(Qt::WA_TranslucentBackground);
    QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_scrollArea);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    centralLayout->addWidget(m_scrollArea);

    // 跳转按键
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignVCenter);
    btnLayout->setContentsMargins(10, 0, 10, 0);
    btnLayout->setSpacing(0);
    m_displaySetting->setIcon(QIcon::fromTheme("open-display-settings"));
    m_displaySetting->setDescription(tr("Display settings"));
    m_displaySetting->setDccModule("display");
    btnLayout->addWidget(m_displaySetting);
    centralLayout->addLayout(btnLayout);

    setFixedWidth(Dock::DOCK_POPUP_WIDGET_WIDTH);

    connect(m_displaySetting, &JumpSettingButton::showPageRequestWasSended, this, &WirelessCastingApplet::requestHideApplet);
#ifdef WIRELESS_CASTING
    connect(m_refresh, &CommonIconButton::clicked, m_model, &WirelessCastingModel::refresh);
    connect(m_monitorsListView, &MonitorListView::clicked, this, [this](const QModelIndex &idx) {
        auto item = dynamic_cast<MonitorItem *>(m_monitorsModel->itemFromIndex(idx));
        if (item) {
            if (nullptr != m_lastConnMonitor) {
                if (item->monitor() == m_lastConnMonitor && item->connectingState())
                    return;
                m_lastConnMonitor->disconnectMonitor();
            }
            item->connMonitor();
            m_lastConnMonitor = item->monitor();
            resizeApplet();
        }
    });
#endif
    connect(m_multiscreenOptionsListView, &MonitorListView::clicked, this, [this](const QModelIndex &idx) {
        auto item = dynamic_cast<MultiscreenOptionItem *>(m_multiscreenOptionsModel->itemFromIndex(idx));
        if (item) {
            m_displayModel->switchMode(item->displayMode(), item->screen());
        }
    });

    hide();
}

void WirelessCastingApplet::initMonitors()
{
#ifdef WIRELESS_CASTING_ENABLED
    auto monitors = m_model->monitors();
    // 遍历所有设备
    for (auto it = monitors.begin(); it != monitors.end(); ++it) {
        MonitorItem *item = new MonitorItem(it.value());
        m_monitorsModel->appendRow(item);
        m_monitors[it.key()] = MonitorMapItem{ item, it.value() };
        m_monitorsListView->update();
    }
    connect(m_model, &WirelessCastingModel::addMonitor, this, &WirelessCastingApplet::onAddMonitor);
    connect(m_model, &WirelessCastingModel::removeMonitor, this, &WirelessCastingApplet::onRemoveMonitor);
#endif
    auto updateScreens = [this] {
        m_displayModel->setCurrentMode(QString());
        m_multiscreenOptionsModel->clear();
        auto screens = m_displayModel->screens();
        m_model->setMultiscreensFlag(screens.size() > 1);
        m_multiscreenOptionsWidget->setVisible(m_model->multiscreensFlag());
        if (m_model->multiscreensFlag()) {
            m_multiscreenOptionsModel->appendRow(new MultiscreenOptionItem(DisplayMode::MergeMode, tr("Duplicate"), ""));
            m_multiscreenOptionsModel->appendRow(new MultiscreenOptionItem(DisplayMode::ExtendMode, tr("Extend"), ""));

            for (int i = 0; i < screens.size(); ++i) {
                auto name = screens[i];
                MultiscreenOptionItem *item = new MultiscreenOptionItem(DisplayMode::SingleMode, tr("Only on %1").arg(name), name);
                QString iconPath = "onlyone-" + QString((i % 2) == 0 ? "f" : "s");
                item->updateIcon(QIcon::fromTheme(iconPath));
                m_multiscreenOptionsModel->appendRow(item);
            }
            m_multiscreenOptionsListView->update();
        }
    };

    auto updateSelectState = [this] {
        for (int row = 0; row < m_multiscreenOptionsModel->rowCount(); ++row) {
            QModelIndex index = m_multiscreenOptionsModel->index(row, 0);
            auto item = dynamic_cast<MultiscreenOptionItem *>(m_multiscreenOptionsModel->itemFromIndex(index));
            if (item) {
                item->selected(false);
                if (item->displayMode() == m_displayModel->displayMode()) {
                    switch (m_displayModel->displayMode()) {
                    case DisplayMode::MergeMode:
                    case DisplayMode::ExtendMode:
                        item->selected(true);
                        m_displayModel->setCurrentMode(item->name());
                        break;
                    case DisplayMode::SingleMode: {
                        if (item->screen() == m_displayModel->primaryScreen()) {
                            item->selected(true);
                            m_displayModel->setCurrentMode(item->name());
                        }
                        break;
                    }
                    }
                }
            }
        }
    };
    connect(m_displayModel, &DisplayModel::monitorsChanged, this, [this, updateScreens, updateSelectState] {
        updateScreens();
        updateSelectState();
        resizeApplet();
    });
    connect(m_displayModel, &DisplayModel::displayModeChanged, this, updateSelectState);
    connect(m_displayModel, &DisplayModel::primaryScreenChanged, this, updateSelectState);
    updateSelectState();

    resizeApplet();
}

void WirelessCastingApplet::onStateChanged(WirelessCastingModel::CastingState state)
{
#ifdef WIRELESS_CASTING_ENABLED
    setEnabled(m_cfgEnabled);
    if ((WirelessCastingModel::NoWirelessDevice == state || WirelessCastingModel::NotSupportP2P == state) && !m_model->multiscreensFlag()) {
        m_wirelesscastingWidget->setVisible(false);
        hide();
        return;
    }

    m_monitorsListView->setVisible(WirelessCastingModel::List == state);
    m_wirelesscastingWidget->setVisible((WirelessCastingModel::NotSupportP2P != state) && (WirelessCastingModel::NoWirelessDevice != state));

    switch (state) {
    case WirelessCastingModel::NoMonitor:
    case WirelessCastingModel::List:
        m_refresh->show();
        break;
    case WirelessCastingModel::Connected:
        m_lastConnMonitor = nullptr;
    default:
        m_refresh->hide();
        break;
    }

    resizeApplet();
#else
    Q_UNUSED(state)
#endif
}

void WirelessCastingApplet::onAddMonitor(const QString &path, Monitor *monitor)
{
    Q_UNUSED(path)
    Q_UNUSED(monitor)
#ifdef WIRELESS_CASTING_ENABLED
    MonitorItem *item = new MonitorItem(monitor);
    m_monitorsModel->appendRow(item);
    m_monitors[path] = MonitorMapItem{ item, monitor };
    resizeApplet();
#endif
}

void WirelessCastingApplet::onRemoveMonitor(const QString &path)
{
    Q_UNUSED(path)
#ifdef WIRELESS_CASTING_ENABLED
    m_monitorsModel->removeRow(m_monitorsModel->indexFromItem(m_monitors[path].item).row());
    if (m_lastConnMonitor == m_monitors.value(path).monitor)
        m_lastConnMonitor = nullptr;
    m_monitors.remove(path);
    resizeApplet();
#endif
}

bool WirelessCastingApplet::casting() const
{
    return m_model->state() == WirelessCastingModel::Connected;
}

void WirelessCastingApplet::setMinHeight(int minHeight)
{
    m_minHeight = minHeight;
    resizeApplet();
}

void WirelessCastingApplet::onContainerChanged(int container)
{
    m_showOnDock = Dock::APPLET_CONTAINER_DOCK == container;
    layout()->setContentsMargins(0, (m_showOnDock ? 10 : 0), 0, 10);
    update();
}

void WirelessCastingApplet::resizeApplet()
{
    int height = 0;
#ifdef WIRELESS_CASTING_ENABLED
    // 列表高度
    if (m_monitorsListView->isVisibleTo(m_contentWidget)) {
        for (int row = 0; row < m_monitorsModel->rowCount(); ++row) {
            QModelIndex index = m_monitorsModel->index(row, 0);
            auto item = dynamic_cast<MonitorItem *>(m_monitorsModel->itemFromIndex(index));
            if (item) {
                height += item->sizeHint().height() + m_monitorsListView->getItemSpacing();
            }
        }
        height -= m_monitorsListView->getItemSpacing();
        m_monitorsListView->setFixedHeight(height);
    }
#endif
    if (m_multiscreenOptionsListView->isVisibleTo(m_contentWidget)) {
        int multiscreenOptionsHeight = m_multiscreenOptionsModel->rowCount() * (m_multiscreenOptionsListView->getStandardItemHeight() + m_multiscreenOptionsListView->getItemSpacing()) - m_multiscreenOptionsListView->getItemSpacing();
        m_multiscreenOptionsListView->setMinimumHeight(multiscreenOptionsHeight);
        height += multiscreenOptionsHeight;
    }

    // 标题栏 + 跳转按键 + 4个间距
    const int fixHeight = m_displaySetting->height() + (10 * (m_showOnDock ? 3 : 2));
    height += fixHeight;
#ifdef WIRELESS_CASTING_ENABLED
    // 状态面板高度
    const int statePanelHeight = m_statePanel->isVisibleTo(m_contentWidget) ?
                        ((m_model->multiscreensFlag() && (m_model->state() == WirelessCastingModel::NoMonitor
                        || m_model->state() == WirelessCastingModel::DisabledWirelessDevice || m_model->state() == WirelessCastingModel::WarningInfo))
                        ? m_statePanel->infoHeight() : (m_minHeight - fixHeight - m_wirelesscastingTitle->height() - 5)) : 0;

    m_statePanel->setFixedHeight(statePanelHeight);
    height += m_statePanel->isVisibleTo(m_contentWidget) ? statePanelHeight : 0;
#endif
    // 如果比允许的最小高度还小，则以最小高度为准
    height = qMax(m_minHeight, height);
#ifdef WIRELESS_CASTING_ENABLED
    // 自定义最大高度为显示10个item的高度
    const int maxHeight = fixHeight + (m_wirelesscastingWidget->isVisibleTo(this) ? m_wirelesscastingWidget->height() : 0)
                        + (10 * (m_monitorsListView->getStandardItemHeight() + m_monitorsListView->getItemSpacing())
                        - m_monitorsListView->getItemSpacing());

    // 如果比自定义最大高度还大，则以自定义最大高度为准
    height = qMin(maxHeight, height);
#endif
    // 如果比允许的最大高度还大，则以最大高度为准
    height = qMin(Dock::DOCK_POPUP_WIDGET_MAX_HEIGHT, height);

    // 滚动区高度
    m_scrollArea->setFixedHeight(m_contentWidget->isVisibleTo(this) ? (height - fixHeight) : 0);
    resize(width(), height);
}

StatePanel::StatePanel(WirelessCastingModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_infoWidget(new QWidget(this))
    , m_infoLab(new DLabel(this))
    , m_noDeviceBox(new QWidget(this))
    , m_iconNoDevice(new DLabel(this))
    , m_noDevice(new DLabel(this))
    , m_connectedDeviceBox(new QWidget(this))
    , m_iconConnected(new DLabel(this))
    , m_connected(new DLabel(this))
    , m_disconnMonitor(new CancelButton)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 提示信息
    QVBoxLayout *infoLayout = new QVBoxLayout(m_infoWidget);
    infoLayout->setSpacing(0);
    infoLayout->setContentsMargins(10, 0, 20, 0);
    m_infoLab->setWordWrap(true);
    m_infoLab->setAlignment(Qt::AlignLeft);
    infoLayout->addWidget(m_infoLab);
    DFontSizeManager::instance()->bind(m_infoLab, DFontSizeManager::T8, QFont::Medium);
    layout->addWidget(m_infoWidget);

    // 没有设备提示框
    m_iconNoDevice->setFixedSize(128, 128);
    m_noDevice->setText(tr("No available casting wireless monitors found"));
    m_noDevice->setWordWrap(true);
    m_noDevice->setAlignment(Qt::AlignCenter);
    m_noDevice->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(m_noDevice, DFontSizeManager::T8);
    QVBoxLayout *noDevice = new QVBoxLayout(m_noDeviceBox);
    noDevice->setContentsMargins(0, 0, 0, 30);
    noDevice->setSpacing(0);
    noDevice->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    noDevice->addWidget(m_iconNoDevice, 0, Qt::AlignTop | Qt::AlignHCenter);
    noDevice->addWidget(m_noDevice);
    layout->addWidget(m_noDeviceBox, 0, Qt::AlignCenter);

    // 设备已连接提示框
    m_iconConnected->setFixedSize(128, 128);
    m_connected->setWordWrap(true);
    m_connected->setAlignment(Qt::AlignCenter);
    m_disconnMonitor->setText(tr("Disconnect"));
    m_disconnMonitor->setObjectName("DisconnMonitor");
    m_disconnMonitor->setFixedSize(130, 36);
    QVBoxLayout *connected = new QVBoxLayout(m_connectedDeviceBox);
    connected->setContentsMargins(0, 0, 0, 0);
    connected->setSpacing(0);
    connected->addWidget(m_iconConnected);
    connected->addWidget(m_connected);
    connected->setSpacing(15);
    connected->addWidget(m_disconnMonitor);
    layout->addWidget(m_connectedDeviceBox, 0, Qt::AlignCenter);

    setState(m_model->state());
    connect(m_disconnMonitor, &QPushButton::clicked, this, &StatePanel::disconnMonitor);
    connect(m_model, &WirelessCastingModel::stateChanged, this, &StatePanel::setState);
    connect(m_model, &WirelessCastingModel::multiscreensFlagChanged, this, [this] {
        setState(m_model->state());
    });

    auto updateThemeType = [this] (DGuiApplicationHelper::ColorType type) {
        if (type == DGuiApplicationHelper::ColorType::LightType) {
            m_iconConnected->setPixmap(DIcon::loadNxPixmap(":/icons/deepin/builtin/light/icons/success_128px.svg"));
            m_iconNoDevice->setPixmap(DIcon::loadNxPixmap(":/icons/deepin/builtin/light/icons/none_128px.svg"));
        } else {
            m_iconConnected->setPixmap(DIcon::loadNxPixmap(":/icons/deepin/builtin/dark/icons/success_128px.svg"));
            m_iconNoDevice->setPixmap(DIcon::loadNxPixmap(":/icons/deepin/builtin/dark/icons/none_128px.svg"));
        }
    };
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, updateThemeType);
    updateThemeType(DGuiApplicationHelper::instance()->themeType());
}

StatePanel::~StatePanel() = default;

void StatePanel::setState(WirelessCastingModel::CastingState state)
{
    setVisible(WirelessCastingModel::NotSupportP2P != state && WirelessCastingModel::NoWirelessDevice != state && WirelessCastingModel::List != state);
    switch (state) {
    case WirelessCastingModel::Connected:
        m_connected->setText(tr("Successfully cast the screen to %1").arg(m_model->curMonitorName()));
        m_connectedDeviceBox->setVisible(true);
        m_noDeviceBox->setVisible(false);
        m_infoWidget->setVisible(false);
        break;
    case WirelessCastingModel::NoMonitor:
        m_connectedDeviceBox->setVisible(false);
        m_noDeviceBox->setVisible(!m_model->multiscreensFlag());
        m_infoWidget->setVisible(m_model->multiscreensFlag());
        break;
    case WirelessCastingModel::WarningInfo:
    case WirelessCastingModel::NotSupportP2P:
    case WirelessCastingModel::DisabledWirelessDevice:
        m_infoWidget->setVisible(true);
        m_noDeviceBox->setVisible(false);
        m_connectedDeviceBox->setVisible(false);
        break;
    default:
        break;
    }

    if (WirelessCastingModel::WarningInfo == state)
        m_infoLab->setText(tr("The wireless card doesn't support this feature, and cannot cast the screen to a wireless monitor"));
    else if (WirelessCastingModel::DisabledWirelessDevice == state)
        m_infoLab->setText(tr("You need to enable wireless network to cast the screen to a wireless monitor"));
    else if (WirelessCastingModel::NoMonitor == state)
        m_infoLab->setText(tr("No available casting wireless monitors found"));
}

int StatePanel::infoHeight()
{
    auto getLine = [](QPainter *painter, const QRectF &rect, int lineHeight, QTextLayout *layout, Qt::TextElideMode mode)
    {
        int lineCount = 0;
        qreal height = 0;
        QString text = layout->text();
        QTextOption text_option = layout->textOption();

        if (painter) {
            text_option.setTextDirection(painter->layoutDirection());
        }

        layout->beginLayout();

        QTextLine line = layout->createLine();
        QPointF offset = rect.topLeft();

        while (line.isValid()) {
            ++lineCount;
            height += lineHeight;

            if (height + lineHeight > rect.height()) {
                QString end_str = text.mid(line.textStart());

                if (painter)
                    end_str = painter->fontMetrics().elidedText(end_str, mode, qRound(rect.width() - 1));

                layout->endLayout();
                layout->setText(end_str);

                text_option.setWrapMode(QTextOption::NoWrap);
                layout->beginLayout();
                line = layout->createLine();
                line.setLineWidth(rect.width() - 1);
                text = end_str;
            } else {
                line.setLineWidth(rect.width());
            }

            line.setPosition(offset);

            if (painter) {
                line.draw(painter, QPointF(0, 0));
            }

            offset.setY(offset.y() + lineHeight);

            if (height + lineHeight > rect.height())
                break;

            line = layout->createLine();
        }
        layout->setTextOption(text_option);
        layout->endLayout();
        return lineCount;
    };

    QTextLayout layout(m_infoLab->text(), m_infoLab->font());
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout.setTextOption(option);
    QFontMetrics fm(m_infoLab->font());
    int line = getLine(nullptr, QRectF(QPointF(0, 0), QSizeF(330, INT_MAX)), fm.boundingRect(m_infoLab->text()).height(), &layout, Qt::ElideNone);
    return fm.boundingRect(m_infoLab->text()).height() * line;
}

} // namespace wirelesscasting
} // namespace dde
