#include "listitemdelegate.h"
#include "listview.h"
#include "mainmodule.h"
#include "src/interface/moduledatamodel.h"
#include "pagemodule.h"

#include <QHBoxLayout>
#include <QSplitter>

using namespace DCC_NAMESPACE;

const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 160;

const QSize ListViweItemIconSize_IconMode(84, 84);
const QSize ListViweItemSize_IconMode(280, 84);
const QSize ListViweItemIconSize_ListMode(32, 32);
const QSize ListViweItemSize_ListMode(168, 48);

namespace DCC_NAMESPACE {
class MainModulePrivate
{
public:
    explicit MainModulePrivate(MainModule *parent = nullptr)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_layout(nullptr)
    {
    }
    ListView *createListView(QWidget *parentWidget)
    {
        Q_Q(MainModule);
        ListView *view = new ListView(parentWidget);
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
            m_sidebarWidget->setIconSize(ListViweItemIconSize_ListMode);
            m_sidebarWidget->setGridSize(ListViweItemSize_ListMode);
            m_sidebarWidget->setContentsMargins(10, 0, 10, 0);
            m_sidebarWidget->setSpacing(10);
            m_sidebarWidget->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ModuleDataModel *model = static_cast<ModuleDataModel *>(m_sidebarWidget->model());
            m_sidebarWidget->setCurrentIndex(model->index(child));
            PageModule *module = qobject_cast<PageModule *>(child);
            if (module) {
                module->setContentsMargins(60, 0, 60, 0);
                module->setMaximumWidth(DCC_PAGEMODULE_MAX_WIDTH + 60 * 2);
            }
            while (!m_layout->isEmpty()) {
                QLayoutItem *item = m_layout->takeAt(0);
                if (item->widget() && item->widget() != m_view)
                    delete item->widget();
                delete item;
            }
            m_layout->addWidget(child->activePage());
            m_mainWindow->setSidebarWidget(m_sidebarWidget);
            m_mainWindow->setSidebarWidth(180);
            m_mainWindow->setSidebarVisible(true);
            m_mainWindow->setSidebarExpanded(true);
            m_view->setVisible(false);
        } else {
            m_view->setViewMode(ListView::IconMode);
            m_view->setIconSize(ListViweItemIconSize_IconMode);
            m_view->setGridSize(ListViweItemSize_IconMode);
            m_view->setContentsMargins(0, 0, 0, 0);
            m_view->setSpacing(20);
            m_view->setAlignment(Qt::AlignCenter);

            while (!m_layout->isEmpty()) {
                QLayoutItem *item = m_layout->takeAt(0);
                if (item->widget() && item->widget() != m_view)
                    delete item->widget();
                delete item;
            }
            m_layout->addWidget(m_view);

            m_view->setMinimumWidth(0);
            m_view->setMaximumWidth(QWIDGETSIZE_MAX);
            m_view->setVisible(true);
            m_view->clearSelection();
            m_mainWindow->setSidebarExpanded(false);
            m_mainWindow->setSidebarVisible(false);
        }
    }

    QWidget *page()
    {
        Q_Q(MainModule);
        QWidget *parentWidget = new QWidget();
        m_layout = new QHBoxLayout;
        parentWidget->setLayout(m_layout);
        QObject::connect(q, &MainModule::currentModuleChanged, parentWidget, [this](ModuleObject *child) {
            onCurrentModuleChanged(child);
        });
        m_view = createListView(parentWidget);
        m_sidebarWidget = createListView(parentWidget);
        m_mainWindow->setSidebarWidget(m_sidebarWidget);

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
}

MainModule::MainModule(DTK_WIDGET_NAMESPACE::DMainWindow *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(MainModule)
{
    Q_D(MainModule);
    d->m_mainWindow = parent;
}

MainModule::~MainModule()
{
}

QWidget *MainModule::page()
{
    Q_D(MainModule);
    return d->page();
}

ModuleObject *MainModule::defultModule()
{
    return nullptr;
}
