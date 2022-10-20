#include "listitemdelegate.h"
#include "listview.h"
#include "mainmodule.h"
#include "src/interface/moduledatamodel.h"
#include "pagemodule.h"

#include <QHBoxLayout>
#include <QSplitter>

using namespace DCC_NAMESPACE;

const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 188;

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
        , m_splitter(nullptr)
    {
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_splitter)
            return;
        QWidget *newWidget = nullptr;
        QWidget *oldWidget = nullptr;
        if (child) {
            if (m_view->viewMode() != ListView::ListMode) {
                m_view->setViewMode(ListView::ListMode);
                m_view->setIconSize(ListViweItemIconSize_ListMode);
                m_view->setGridSize(ListViweItemSize_ListMode);
                m_view->setContentsMargins(10, 0, 10, 0);
                m_view->setSpacing(10);
                m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
                // 最小160,默认180
                m_view->setMinimumWidth(160);
                m_view->setMaximumWidth(330);
                m_view->resize(180, -1);
            }
            ModuleDataModel *model = static_cast<ModuleDataModel *>(m_view->model());
            m_view->setCurrentIndex(model->index(child));
            PageModule *module = qobject_cast<PageModule *>(child);
            if(module) {
                module->setContentsMargins(60, 0, 60, 0);
                module->setMaximumWidth(DCC_PAGEMODULE_MAX_WIDTH + 60 * 2);
            }
            newWidget = child->activePage();
            oldWidget = m_splitter->replaceWidget(1, newWidget);

            if (!m_splitter->handle(1)->isEnabled()) {
                m_splitter->handle(1)->setEnabled(true);
                m_splitter->setSizes({180,600});
            }
        } else if (m_view->viewMode() != ListView::IconMode) {
            m_view->setViewMode(ListView::IconMode);
            m_view->setIconSize(ListViweItemIconSize_IconMode);
            m_view->setGridSize(ListViweItemSize_IconMode);
            m_view->setContentsMargins(0, 0, 0, 0);
            m_view->setSpacing(20);
            m_view->setAlignment(Qt::AlignCenter);
            m_splitter->handle(1)->setEnabled(false);
            newWidget = m_emptyWidget;
            oldWidget = m_splitter->replaceWidget(1, newWidget);

            m_view->setMinimumWidth(0);
            m_view->setMaximumWidth(QWIDGETSIZE_MAX);
        }
        if (newWidget)
            newWidget->setVisible(newWidget != m_emptyWidget);
        if (oldWidget && oldWidget != m_emptyWidget)
            delete oldWidget;
    }

    QWidget *page()
    {
        Q_Q(MainModule);
        m_splitter = new QSplitter(Qt::Horizontal);

        m_view = new ListView(m_splitter);
        ListItemDelegate *delegate = new ListItemDelegate(m_view);
        m_view->setItemDelegate(delegate);
        ModuleDataModel *model = new ModuleDataModel(m_view);
        model->setModuleObject(q);
        QObject::connect(q, &MainModule::currentModuleChanged, m_splitter, [this](ModuleObject *child) {
            onCurrentModuleChanged(child);
        });
        m_view->setModel(model);
        m_view->setFrameShape(QFrame::Shape::NoFrame);
        m_view->setAutoScroll(true);
        m_view->setDragEnabled(false);
        m_view->setMaximumWidth(NavViewMaximumWidth);
        m_view->setMinimumWidth(NavViewMinimumWidth);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_view->setAcceptDrops(false);

        auto onClicked = [](const QModelIndex &index) {
            ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
            if (obj)
                obj->trigger();
        };

        QObject::connect(m_view, &ListView::activated, m_view, &ListView::clicked);
        QObject::connect(m_view, &ListView::clicked, m_view, onClicked);
        m_splitter->addWidget(m_view);
        m_emptyWidget = new QWidget(m_splitter);
        m_splitter->addWidget(m_emptyWidget);
        m_emptyWidget->setVisible(false);

        m_splitter->setCollapsible(0, false);
        m_splitter->setCollapsible(1, true);
        m_splitter->setChildrenCollapsible(true);

        onCurrentModuleChanged(q->currentModule());
        return m_splitter;
    }

private:
    MainModule *q_ptr;
    Q_DECLARE_PUBLIC(MainModule)

    ListView *m_view;
    QSplitter *m_splitter;
    QWidget *m_emptyWidget;
};
}

MainModule::MainModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, contentText, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, contentText, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, icon, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
}

MainModule::MainModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, contentText, icon, parent)
    , DCC_INIT_PRIVATE(MainModule)
{
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
