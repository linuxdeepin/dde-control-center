#include "dccplugintestwidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <DGuiApplicationHelper>
#include <DPalette>
#include <QPainter>
#include <DApplicationHelper>
#include <QScrollArea>
#include <DTitlebar>
#include <DPaletteHelper>

#include <QLineEdit>
#include <QScroller>
#include <QObject>

#include "dccnetworkmodule.h"

#include <widgets/multiselectlistview.h>
#include <DBackgroundGroup>
#include <QScreen>

using namespace dcc::widgets;

static int WidgetMinimumWidth = 820;
static int WidgetMinimumHeight = 634;

const int second_widget_min_width = 230;
const int third_widget_min_width = 340;
const int widget_total_min_width = 820;
const int four_widget_min_widget = widget_total_min_width + third_widget_min_width + 40;
const int first_widget_min_width = 188;
const qint32 ActionIconSize = 30;//大图标角标大小

const QSize ListViweItemIconSize(84,84);
const QSize ListViweItemSize(170,168);
const QSize ListViweItemIconSize_ListMode(42,42);
const QSize ListViweItemSize_ListMode(168,48);

Q_DECLARE_METATYPE(QMargins)
const QMargins navItemMargin(5, 3, 5, 3);
const QVariant NavItemMargin = QVariant::fromValue(navItemMargin);

DccPluginTestWidget::DccPluginTestWidget(QWidget *parent)
    : DMainWindow(parent)
    , m_topWidget(nullptr)
    , m_translator(new QTranslator)
    , m_isFinalWidget(false)
    , m_isFromSecondAddWidget(false)
{
    QWidget *content = new QWidget(this);
    content->setObjectName("contentwindow");
    QHBoxLayout *contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    m_rightContentLayout = new QHBoxLayout();
    m_rightContentLayout->setContentsMargins(0, 0, 0, 0);

    m_rightView = new DBackgroundGroup(m_rightContentLayout);
    m_rightView->setObjectName("modulepage");
    m_rightView->setItemSpacing(2);
    m_rightView->setContentsMargins(10, 10, 10, 10);

    m_navView = new dcc::widgets::MultiSelectListView(this);
    m_navView->setAccessibleName("Form_mainmenulist");
    m_navView->setFrameShape(QFrame::Shape::NoFrame);
    m_navView->setEditTriggers(QListView::NoEditTriggers);
    m_navView->setResizeMode(QListView::Adjust);
    m_navView->setAutoScroll(true);

    QScroller::grabGesture(m_navView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_navView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    contentLayout->addWidget(m_navView, 1);
    contentLayout->addWidget(m_rightView, 5);

    m_rightView->hide();
    m_rightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout()->setMargin(0);
    setCentralWidget(content);

    //Initialize top page view and model
    m_navModel = new QStandardItemModel(m_navView);
    m_navView->setModel(m_navModel);
    connect(m_navView, &DListView::activated, this, &DccPluginTestWidget::onFirstItemClick);
    connect(m_navView, &DListView::clicked, m_navView, &DListView::activated);

    /*m_searchWidget = new SearchWidget(this);
    m_searchWidget->setMinimumSize(350, 36);
    m_searchWidget->setAccessibleName("SearchModule");
    m_searchWidget->lineEdit()->setAccessibleName("SearchModuleLineEdit");
    GSettingWatcher::instance()->bind("mainwindowSearchEdit", m_searchWidget);*/

    DTitlebar *titlebar = this->titlebar();
    auto widhetlist = titlebar->children();
    for (auto child : widhetlist) {
        if (auto item = qobject_cast<DIconButton *>(child)) {
            item->setAccessibleName("FrameIcom");
        }
    }
    titlebar->setAccessibleName("Mainwindow bar");
    //titlebar->addWidget(m_searchWidget, Qt::AlignCenter);
    //connect(m_searchWidget, &SearchWidget::notifyModuleSearch, this, &MainWindow::onEnterSearchWidget);

    auto menu = titlebar->menu();
    if (!menu) {
        qDebug() << "menu is nullptr, create menu!";
        menu = new QMenu;
    } else {
        qDebug() << "get title bar menu :" << menu;
    }
    menu->setAccessibleName("titlebarmenu");
    titlebar->setMenu(menu);

    auto action = new QAction(tr("Help"));
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [ = ] {
        /*QString helpTitle = m_moduleName;
        if (helpTitle.isEmpty()) {
            helpTitle = "controlcenter";
        }
        const QString dmanInterface = "com.deepin.Manual.Open";
        QDBusInterface *inter = new QDBusInterface(dmanInterface,
                                                   "/com/deepin/Manual/Open",
                                                   dmanInterface,
                                                   QDBusConnection::sessionBus());
        inter->call("OpenTitle", "dde", helpTitle);
        inter->deleteLater();*/
    });

    m_backwardBtn = new DIconButton(this);
    m_backwardBtn->setAccessibleName("backwardbtn");
    m_backwardBtn->setEnabled(false);
    m_backwardBtn->setIcon(QStyle::SP_ArrowBack);
    QWidget *backWidget = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addSpacing(5);
    btnLayout->addWidget(m_backwardBtn);
    backWidget->setLayout(btnLayout);
    titlebar->addWidget(backWidget, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar->setIcon(QIcon::fromTheme("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, [this] {
        //说明：只有"update"模块/"镜像源列表"页面需要从第三级页面返回第二级页面(若其他模块还有需求可以在此处添加处理)
        if (!m_contentStack.isEmpty() && m_contentStack.last().first->name() != "update") {
            popAllWidgets();
        } else {
            popWidget();
        }
        resetNavList(m_contentStack.isEmpty());
    });
    updateViewBackground();
    updateWinsize();

    initNetworkModule();
}

DccPluginTestWidget::~DccPluginTestWidget()
{
    delete m_module;
}

void DccPluginTestWidget::pushWidget(ModuleInterface * const inter, QWidget * const w, FrameProxyInterface::PushType type)
{
    if (!inter || !w)
        return;

    m_lastPushWidget = w;
    switch (type) {
    case Replace:
        replaceThirdWidget(inter, w);
        break;
    case CoverTop://根据当前页面宽度去计算新增的页面放在最后面一层，或者Top页面
        judgeTopWidgetPlace(inter, w);
        break;
    case DirectTop://不需要管页面宽度，直接将新增页面放在Top页面；为解决某些页面使用CoverTop无法全部展示的问题
        pushTopWidget(inter, w);
        break;
    case Normal:
    default:
        pushNormalWidget(inter, w);
        break;
    }

    resetTabOrder();
}

void DccPluginTestWidget::popWidget(ModuleInterface * const inter)
{
    Q_UNUSED(inter)

    popWidget();
    resetNavList(m_contentStack.isEmpty());
}

void DccPluginTestWidget::setModuleVisible(ModuleInterface * const inter, const bool visible)
{
    Q_UNUSED(inter);
    Q_UNUSED(visible);
}

void DccPluginTestWidget::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_UNUSED(module);
    Q_UNUSED(page);
    Q_UNUSED(animation)

    raise();

    // onEnterSearchWidget(module, page);
    // Note: 当直接进入模块界面(二级界面)，先将模块界面显示出来，在加载首界面
    QTimer::singleShot(0, this, [ = ] {
        if (isMinimized() || !isVisible())
            showNormal();

        activateWindow();
    });
}

void DccPluginTestWidget::setModuleSubscriptVisible(const QString &module, bool bIsDisplay)
{
    QPair<DViewItemAction *, DViewItemAction *> m_pair(nullptr, nullptr);
    int index = 0;
    for (const auto &k : m_remindeSubscriptList) {
        if (module == k.m_name) {
            m_pair = k.m_action;
            index = k.m_index;
        }
    }

    if (m_pair.first == nullptr) {
        return;
    }

    if (m_navView->viewMode() == QListView::IconMode) {
        if (m_pair.first->isVisible() != bIsDisplay) {
            m_pair.first->setVisible(bIsDisplay);
            if (bIsDisplay) {
                m_navModel->item(index, 0)->setData(QVariant::fromValue(QMargins(ActionIconSize + 15, 0, 0, 0)), Dtk::MarginsRole);
            } else {
                m_navModel->item(index, 0)->setData(QVariant::fromValue(QMargins(0, 0, 0, 0)), Dtk::MarginsRole);
            }
        }
    } else {
        if (m_pair.second->isVisible() != bIsDisplay) {
            m_pair.second->setVisible(bIsDisplay);
        }
    }
    m_navView->update();
}

void DccPluginTestWidget::setRemoveableDeviceStatus(QString type, bool state)
{
    if (state) {
        if (!m_removeableDeviceList.contains(type)) {
            m_removeableDeviceList.append(type);
        }
    } else {
        if (m_removeableDeviceList.contains(type)) {
            m_removeableDeviceList.removeOne(type);
        }
    }
}

bool DccPluginTestWidget::getRemoveableDeviceStatus(QString type) const
{
    return m_removeableDeviceList.contains(type);
}

void DccPluginTestWidget::setSearchPath(ModuleInterface * const inter) const
{
    Q_UNUSED(inter);
}

void DccPluginTestWidget::onBack()
{
    popWidget();
}

void DccPluginTestWidget::onFirstItemClick(const QModelIndex &index)
{
    m_navView->setFocus();
    popAllWidgets();

    static bool isFirstEnter = true;
    if (isFirstEnter) {
        m_module->initialize();
        isFirstEnter = false;
    }
    setCurrModule(m_module);
    m_module->active();
    m_navView->resetStatus(index);
}

void DccPluginTestWidget::initNetworkModule()
{
    m_module = new DCCNetworkModule;
    m_module->setFrameProxy(this);
    m_translator->load(m_module->translationPath());
    qApp->installTranslator(m_translator);
    DStandardItem *item = new DStandardItem;
    item->setIcon(m_module->icon());
    item->setText(m_module->name());
    m_navModel->appendRow(item);

    resetNavList(true);
}

void DccPluginTestWidget::replaceThirdWidget(ModuleInterface * const inter, QWidget * const w)
{
    if (m_contentStack.count() != 2)
        return;

    if (m_lastThirdPage.second) {
        m_lastThirdPage.first = nullptr;
        m_lastThirdPage.second = nullptr;
    }

    QPair<ModuleInterface *, QWidget *>widget = m_contentStack.pop();
    m_lastThirdPage.first = widget.first;
    m_lastThirdPage.second = widget.second;
    m_lastThirdPage.second->setVisible(false);

    w->setParent(m_lastThirdPage.second);//the replace widget follow the old third widget to delete
    m_lastThirdPage.second->setParent(m_contentStack.top().second);
    pushNormalWidget(inter, w);
}

void DccPluginTestWidget::pushNormalWidget(ModuleInterface * const inter, QWidget * const w)
{
    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
    popAllWidgets(1);
    //Set the newly added page to fill the blank area
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w, m_contentStack.size() == 1 ? 3 : 7);

    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);

        //解决插件二级菜单右侧两角为圆角的问题[正常情况圆角应该在三级菜单的右侧两角和二级菜单的左侧两角]
        m_contentStack.at(1).second->show();
    }

    resetNavList(m_contentStack.empty());
}

void DccPluginTestWidget::popAllWidgets(int place)
{
    if (place == 0 && m_topWidget) {
        m_topWidget->deleteLater();
        m_topWidget = nullptr;
        return;
    }

    for (int pageCount = m_contentStack.count(); pageCount > place; pageCount--) {
        popWidget();
    }
}

void DccPluginTestWidget::judgeTopWidgetPlace(ModuleInterface * const inter, QWidget * const w)
{
    int totalWidth = m_navView->minimumWidth() + 20;//10 is first and second widget space　, other 10 is last widget with right box space
    int contentCount = m_contentStack.count();

    if (m_isFinalWidget) {
        m_isFinalWidget = false;
    }

    for (int count = 0; count < contentCount; count++) {
        totalWidth += m_contentStack.at(count).second->minimumWidth();
    }

    //according current content widgets count to calculate use top widget or right widget
    switch (contentCount) {
    case 1: //from second widget to add top/right widget (like update setting : source list)
        if (totalWidth < widget_total_min_width) {
            m_isFinalWidget = true;
            m_isFromSecondAddWidget = true;//save from pushWidget of CoverTop type to add final(right) widget
        }
        break;
    case 2: //from third widget to add top/right widget
        if (four_widget_min_widget < width()) {
            m_isFinalWidget = true;
        }

        if (m_isFromSecondAddWidget) {
            popAllWidgets(1);//pop the final widget, need distinguish
            m_isFromSecondAddWidget = false;
        }
        break;
    case 3: //replace final widget to new top/right widget
        m_isFinalWidget = true;
        popAllWidgets(2);//move fourth widget(m_navView not in it , other level > 2)
        break;
    default:
        qDebug() << Q_FUNC_INFO << " error widget content conut : " << contentCount;
        return;
    }

    if (m_isFinalWidget) {
        pushFinalWidget(inter, w);
    } else {
        pushTopWidget(inter, w);
    }
}

void DccPluginTestWidget::popWidget()
{
    if (m_topWidget) {
        if(0 < m_contentStack.count())
            m_lastPushWidget = m_contentStack.last().second;
        m_topWidget->deleteLater();
        m_topWidget = nullptr;
        return;
    }
    if (!m_contentStack.size())
        return;

    QWidget *w = m_contentStack.pop().second;

    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    //delete replace widget : first delete replace widget(up code) , then pass pushWidget to set last widget
    if (m_lastThirdPage.second) {
        m_lastThirdPage.second->setVisible(true);
        pushWidget(m_lastThirdPage.first, m_lastThirdPage.second);
        //only set two pointer to nullptr , but not delete memory
        m_lastThirdPage.first = nullptr;
        m_lastThirdPage.second = nullptr;
    }

    if (m_contentStack.isEmpty())
        setCurrModule(nullptr);
}

void DccPluginTestWidget::pushTopWidget(ModuleInterface * const inter, QWidget * const w)
{
    Q_UNUSED(inter)

    m_topWidget = new FourthColWidget(this);
    m_topWidget->initWidget(w, inter);
    m_topWidget->setVisible(true);

    connect(m_topWidget, &FourthColWidget::signalBack, this, &DccPluginTestWidget::onBack);

    m_topWidget->setFixedHeight(height() - this->titlebar()->height());
    m_topWidget->move(0, titlebar()->height());

    resetNavList(m_contentStack.empty());
}

void DccPluginTestWidget::resetTabOrder()
{
    QWidget *pre = m_navView;
    pre->setFocusPolicy(Qt::TabFocus);
    for (int i = 0; i < m_contentStack.size(); ++i) {
        auto tw = m_contentStack.at(i).second;
        findFocusChild(tw, pre);
    }
}

void DccPluginTestWidget::findFocusChild(QWidget *w, QWidget *&pre)
{
    if (!w->layout() || w->layout()->count() == 0)
        return;

    auto l = w->layout();
    findFocusChild(l, pre);
}

void DccPluginTestWidget::findFocusChild(QLayout *l, QWidget *&pre)
{
    for (int i = 0; i < l->count(); ++i) {
        auto cw = l->itemAt(i)->widget();
        auto cl = l->itemAt(i)->layout();

        if (cw) {
            if (cw && cw->layout() && cw->layout()->count()) {
                findFocusChild(cw, pre);
            }

            if (auto scr = qobject_cast<QScrollArea *>(cw)) {
                if (scr->widget()) {
                    findFocusChild(scr->widget(), pre);
                }
            }

            if ((!qobject_cast<QAbstractButton *>(cw)
                    && !qobject_cast<QLineEdit *>(cw)
                    && !qobject_cast<QAbstractItemView *>(cw)
                    && !qobject_cast<QAbstractSlider *>(cw)) || !cw->isEnabled()) {
                continue;
            }

            if (!pre) {
                pre = cw;
                continue;
            }

            setTabOrder(pre, cw);
            pre = cw;
        } else if (cl) {
            if (cl && cl->layout() && cl->layout()->count()) {
                findFocusChild(cl, pre);
            }
        }
    }
}

void DccPluginTestWidget::pushFinalWidget(ModuleInterface * const inter, QWidget * const w)
{
    w->layout()->setMargin(0);
    w->setContentsMargins(0, 8, 0, 8);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w, 7);

    w->setMinimumWidth(third_widget_min_width);
    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);
    }
}

void DccPluginTestWidget::updateViewBackground()
{
    DPalette pa = DPaletteHelper::instance()->palette(m_navView);
    QColor base_color = palette().base().color();

    if (m_navView->viewMode() == QListView::IconMode) {
        DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(base_color);

        if (ct == DGuiApplicationHelper::LightType) {
            pa.setBrush(DPalette::ItemBackground, palette().base());
        } else {
            base_color = DGuiApplicationHelper::adjustColor(base_color, 0, 0, +5, 0, 0, 0, 0);
            pa.setColor(DPalette::ItemBackground, base_color);
        }

    } else {
        pa.setColor(DPalette::ItemBackground, base_color);
    }

    DPaletteHelper::instance()->setPalette(m_navView, pa);
}

void DccPluginTestWidget::updateWinsize()
{
    int w = QGuiApplication::primaryScreen()->geometry().width();
    int h = QGuiApplication::primaryScreen()->geometry().height();
    WidgetMinimumWidth = qMin(w, 820);
    WidgetMinimumHeight = qMin(h, 634);
    setMinimumSize(QSize(WidgetMinimumWidth, WidgetMinimumHeight));
}

void DccPluginTestWidget::resetNavList(bool isIconMode)
{
    if (isIconMode && m_navView->viewMode() == QListView::IconMode)
        return;

    if (!isIconMode && m_navView->viewMode() == QListView::ListMode)
        return;

    if (isIconMode) {
        //Only remain 1 level page : back to top page
        m_navView->setViewportMargins(QMargins(0, 0, 0, 0));
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setDragEnabled(false);
        m_navView->setMaximumWidth(QWIDGETSIZE_MAX);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_navView->setIconSize(ListViweItemIconSize);
        m_navView->setItemSize(ListViweItemSize);
        m_navView->setItemSpacing(0);
        m_navView->setSpacing(20);
        m_navView->clearSelection();
        m_navView->setSelectionMode(QAbstractItemView::NoSelection);

        //Icon模式，"update"使用右上角角标Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if (m_modules.at(i).first->name() == data.m_name) {
                    data.m_action.first->setVisible(data.m_action.second->isVisible());
                    data.m_action.second->setVisible(false);
                    if (data.m_action.first->isVisible())
                        m_navModel->item(i, 0)->setData(QVariant::fromValue(QMargins(ActionIconSize + 15, 0, 0, 0)), Dtk::MarginsRole);
                    else
                        m_navModel->item(i, 0)->setData(QVariant::fromValue(QMargins()), Dtk::MarginsRole);
                    break;
                }
            }
        }

        DStyle::setFrameRadius(m_navView, 18);
        m_rightView->hide();
        m_backwardBtn->setEnabled(false);
    } else {
        //The second page will Covered with fill blank areas
        m_navView->setViewportMargins(QMargins(10, 10, 10, 10));
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setMinimumWidth(first_widget_min_width);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_navView->setIconSize(ListViweItemIconSize_ListMode);
        m_navView->setItemSize(ListViweItemSize_ListMode);
        m_navView->setSpacing(0);
        m_navView->setSelectionMode(QAbstractItemView::SingleSelection);

        //List模式，"update"使用统一Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if (m_modules.at(i).first->name() == data.m_name) {
                    m_navModel->item(i, 0)->setData(NavItemMargin, Dtk::MarginsRole);
                    data.m_action.second->setVisible(data.m_action.first->isVisible());
                    data.m_action.first->setVisible(false);
                    break;
                }
            }
        }

        DStyle::setFrameRadius(m_navView, 8);
        // 选中当前的项
        m_navView->selectionModel()->select(m_navView->currentIndex(), QItemSelectionModel::SelectCurrent);
        m_rightView->show();
        m_backwardBtn->setEnabled(true);
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    m_navView->setAutoFillBackground(!isIconMode);
    updateViewBackground();
}

/**
 * @brief FourthColWidget::FourthColWidget
 * @param parent
 */
FourthColWidget::FourthColWidget(QWidget *parent)
    : QWidget(parent)
    , m_curWidget(nullptr)
{
}

void FourthColWidget::initWidget(QWidget *showWidget, ModuleInterface *module)
{
    this->setMinimumSize(parentWidget()->width(), this->parentWidget()->height());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    showWidget->setMinimumWidth(this->parentWidget()->width() / 2 - 40);
    showWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    showWidget->setAutoFillBackground(true);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch(7);
    layout->addWidget(showWidget, 4, Qt::AlignRight);
    this->setLayout(layout);
    m_curWidget = showWidget;
    m_curInterface = module;

    QObject::connect(m_curWidget, &QWidget::destroyed, this, &FourthColWidget::onCurrentWidgetDestroy);

    update();
}

void FourthColWidget::onCurrentWidgetDestroy()
{
    m_curWidget = nullptr;
}

void FourthColWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QLinearGradient linear;
    int startPosX = pos().x() + (m_curWidget ? m_curWidget->x() : 0);
    linear.setStart(QPointF(startPosX, 0));
    linear.setFinalStop(QPointF(startPosX - 30, 0));
    if (DGuiApplicationHelper::LightType == DApplicationHelper::instance()->themeType()) {
        linear.setColorAt(0, QColor(0, 0, 0, 13));
    } else {
        linear.setColorAt(0, QColor(0, 0, 0, 26));
    }

    linear.setColorAt(1, QColor(0, 0, 0, 0));

    DPalette pa = DPaletteHelper::instance()->palette(this);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    QColor curThemeColor = pa.base().color();
    curThemeColor.setAlphaF(0.6);  // 设置透明度
    painter.setBrush(QBrush(QColor(curThemeColor)));
    painter.drawRect(this->parentWidget()->rect());

    painter.setBrush(QBrush(linear));

    QRect rt(startPosX, 0, -30, rect().height());
    painter.drawRect(rt);
}

void FourthColWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_curWidget && m_curWidget->geometry().contains(event->pos()))
        return;
    Q_EMIT signalBack();
}
