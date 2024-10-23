// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "mainmodule.h"

#include "listitemdelegate.h"
#include "listview.h"
#include "pagemodule.h"
#include "src/interface/moduledatamodel.h"

#include <dguiapplicationhelper.h>

#include <DPlatformTheme>
#include <DSizeMode>

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QSplitter>

#if DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 3, 0)
#  define USE_SIDEBAR
#endif

using namespace DCC_NAMESPACE;

constexpr int NavViewMaximumWidth = QWIDGETSIZE_MAX;
constexpr int NavViewMinimumWidth = 160;

constexpr QSize ListViewItemIconSize_IconMode(64, 64);
constexpr QSize ListViewItemGridSize_IconMode(280, 84);
constexpr QSize ListViewItemIconSize_ListMode(32, 32);
constexpr QSize ListViewItemGridSize_ListMode(168, 36);
constexpr int ListView_ListMode_MaxWidth = 400;
constexpr int ListView_IconMode_MaxWidth = 500;
// mainly about icon and margin
constexpr int ExtraWidth = 80;

namespace DCC_NAMESPACE {
class MainModulePrivate
{
public:
    explicit MainModulePrivate(MainModule *parent = nullptr)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_sidebarWidget(nullptr)
        , m_layout(nullptr)
    {
        QObject::connect(Dtk::Gui::DGuiApplicationHelper::instance(), 
            &Dtk::Gui::DGuiApplicationHelper::sizeModeChanged, [this](){
                updateSpacing();
            });
    }

    ListView *createListView(QWidget *parentWidget, bool isSizebar = false)
    {
        Q_Q(MainModule);
        const QSize viewGridSize =
                isSizebar ? ListViewItemGridSize_ListMode : ListViewItemGridSize_IconMode;
        const QSize viewIconSize =
                isSizebar ? ListViewItemIconSize_ListMode : ListViewItemIconSize_IconMode;
        const int viewMaxWidth =
                isSizebar ? ListView_ListMode_MaxWidth : ListView_IconMode_MaxWidth;
        ListView *view = new ListView(parentWidget);
        view->setGridSize(viewGridSize);
        view->setIconSize(viewIconSize);
        ListItemDelegate *delegate = new ListItemDelegate(view);
        view->setItemDelegate(delegate);
        ModuleDataModel *model = new ModuleDataModel(view);
        model->setModuleObject(q);

        view->setModel(model);
        view->setFrameShape(QFrame::Shape::NoFrame);
        view->setAutoScroll(true);
        view->setDragEnabled(false);
        view->setMaximumWidth(NavViewMaximumWidth);
        view->setMinimumWidth(NavViewMinimumWidth);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setAcceptDrops(false);

        auto onClicked = [](const QModelIndex &index) {
            ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
            if (obj)
                obj->trigger();
        };
        QObject::connect(model,
                         &ModuleDataModel::newModuleMaxDislayName,
                         view,
                         [view, viewGridSize, viewIconSize, viewMaxWidth](const QString &name) {
                             QSize gradSize = view->gridSize();
                             const int elementWidth = view->fontMetrics().horizontalAdvance(name)
                                     + viewIconSize.width() + ExtraWidth;
                             if (gradSize.width() < elementWidth && elementWidth < viewMaxWidth) {
                                 view->setGridSize(QSize(elementWidth, viewGridSize.height()));
                             }
                         });
        QObject::connect(view, &ListView::activated, view, &ListView::clicked);
        QObject::connect(view, &ListView::clicked, view, onClicked);
        return view;
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_layout)
            return;
        if (child && ModuleObject::IsVisible(child)) {
            m_sidebarWidget->setViewMode(ListView::ListMode);
            m_sidebarWidget->setContentsMargins(10, 0, 10, 0);
            m_sidebarWidget->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ModuleDataModel *model = static_cast<ModuleDataModel *>(m_sidebarWidget->model());
            m_sidebarWidget->setCurrentIndex(model->index(child));
            PageModule *module = qobject_cast<PageModule *>(child);
            if (module) {
                module->setContentsMargins(60, 0, 60, 0);
                module->setMaximumWidth(DCC_PAGEMODULE_MAX_WIDTH + 60 * 2);
            }
            while (m_layout->count() != 0) {
                QLayoutItem *item = m_layout->takeAt(0);
                if (item->widget() && item->widget() != m_view && item->widget() != m_sidebarWidget)
                    delete item->widget();
                delete item;
            }
            m_sidebarWidget->setVisible(true);
            int sizebar = m_sidebarWidget->gridSize().width();
#ifdef USE_SIDEBAR
            m_mainWindow->setSidebarWidth(sizebar);
            m_mainWindow->setSidebarVisible(true);
#else
            m_layout->addWidget(m_sidebarWidget);
            m_sidebarWidget->setFixedWidth(sizebar);
#endif
            m_layout->addWidget(child->activePage());
            m_view->setVisible(false);
            m_sidebarWidget->setFocus();
        } else {
            m_view->setViewMode(ListView::IconMode);
            m_view->setContentsMargins(0, 0, 0, 0);
            m_view->setAlignment(Qt::AlignHCenter);

            while (!m_layout->isEmpty()) {
                QLayoutItem *item = m_layout->takeAt(0);
                if (item->widget() && item->widget() != m_view && item->widget() != m_sidebarWidget)
                    delete item->widget();
                delete item;
            }
            QVBoxLayout *vlayout = new QVBoxLayout;
            vlayout->addSpacing(20);
            vlayout->addWidget(m_view);
            m_layout->addLayout(vlayout);

            m_view->setMinimumWidth(0);
            m_view->setMaximumWidth(QWIDGETSIZE_MAX);
            m_view->setVisible(true);
            m_view->clearSelection();
            m_sidebarWidget->setVisible(false);
#ifdef USE_SIDEBAR
            m_mainWindow->setSidebarWidth(100);
            m_mainWindow->setSidebarVisible(false);
#endif
        }
    }

public slots:
    void updateSpacing()
    {
        if (m_view && m_sidebarWidget) {
            m_view->setSpacing(Dtk::Widget::DSizeModeHelper::element(10, 20));
            m_sidebarWidget->setSpacing(Dtk::Widget::DSizeModeHelper::element(0, 10));
        }
    }

    QWidget *page()
    {
        Q_Q(MainModule);
        QWidget *parentWidget = new QWidget();
        m_layout = new QHBoxLayout;
        m_layout->setContentsMargins(0, 10, 0, 10);
        m_layout->setSpacing(0);
        parentWidget->setLayout(m_layout);
        QObject::connect(q,
                         &MainModule::currentModuleChanged,
                         parentWidget,
                         [this](ModuleObject *child) {
                             onCurrentModuleChanged(child);
                         });
        m_view = createListView(parentWidget);
        m_sidebarWidget = createListView(parentWidget, true);
        updateSpacing();
        QObject::connect(q, &MainModule::moduleDataChanged, q, [this, q](){
            if (auto model = dynamic_cast<ModuleDataModel*>(m_sidebarWidget->model())) {
                auto index = model->index(q->currentModule());
                model->dataChanged(index, index);
            }
        });
#ifdef USE_SIDEBAR
        m_mainWindow->setSidebarWidget(m_sidebarWidget);
#endif
        onCurrentModuleChanged(q->currentModule());
        return parentWidget;
    }

private:
    MainModule *q_ptr;
    Q_DECLARE_PUBLIC(MainModule)

    ListView *m_view;
    ListView *m_sidebarWidget;
    QHBoxLayout *m_layout;
    DTK_WIDGET_NAMESPACE::DMainWindow *m_mainWindow;
};
} // namespace DCC_NAMESPACE

MainModule::MainModule(DTK_WIDGET_NAMESPACE::DMainWindow *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(MainModule)
{
    Q_D(MainModule);
    d->m_mainWindow = parent;
}

MainModule::~MainModule() { }

QWidget *MainModule::page()
{
    Q_D(MainModule);
    return d->page();
}

ModuleObject *MainModule::defultModule()
{
    return nullptr;
}
