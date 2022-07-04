#include "listitemdelegate.h"
#include "listview.h"
#include "mainmodule.h"
#include "src/interface/moduledatamodel.h"

#include <QHBoxLayout>

DCC_USE_NAMESPACE

const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 188;

const QSize ListViweItemIconSize_IconMode(84, 84);
const QSize ListViweItemSize_IconMode(280, 84);
const QSize ListViweItemIconSize_ListMode(32, 32);
const QSize ListViweItemSize_ListMode(168, 48);

DCC_BEGIN_NAMESPACE
class MainModulePrivate
{
public:
    explicit MainModulePrivate(MainModule *parent = nullptr)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_layout(nullptr)
    {
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_layout)
            return;
        if (m_layout->count() > 1) {
            QLayoutItem *item = m_layout->takeAt(1);
            delete item->widget();
            delete item;
        }
        if (child) {
            m_view->setViewMode(ListView::ListMode);
            m_view->setIconSize(ListViweItemIconSize_ListMode);
            m_view->setGridSize(ListViweItemSize_ListMode);
            m_view->setSpacing(0);
            m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            m_layout->addWidget(child->activePage(), 5);
            ModuleDataModel *model = static_cast<ModuleDataModel *>(m_view->model());
            m_view->setCurrentIndex(model->index(child));
        } else {
            m_view->setViewMode(ListView::IconMode);
            m_view->setIconSize(ListViweItemIconSize_IconMode);
            m_view->setGridSize(ListViweItemSize_IconMode);
            m_view->setSpacing(20);
            m_view->setAlignment(Qt::AlignCenter);
        }
    }

    QWidget *page()
    {
        Q_Q(MainModule);
        QWidget *parentWidget = new QWidget();
        m_layout = new QHBoxLayout(parentWidget);
        m_layout->setContentsMargins(0, 0, 0, 0);
        m_layout->setSpacing(0);
        parentWidget->setLayout(m_layout);

        m_view = new ListView(parentWidget);
        ListItemDelegate *delegate = new ListItemDelegate(m_view);
        m_view->setItemDelegate(delegate);
        ModuleDataModel *model = new ModuleDataModel(m_view);
        model->setModuleObject(q);
        QObject::connect(q, &MainModule::currentModuleChanged, m_layout, [this](ModuleObject *child) {
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
        m_layout->addWidget(m_view, 1);

        onCurrentModuleChanged(q->currentModule());
        return parentWidget;
    }

private:
    MainModule *q_ptr;
    Q_DECLARE_PUBLIC(MainModule)

    ListView *m_view;
    QHBoxLayout *m_layout;
};
DCC_END_NAMESPACE

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

ModuleObject *MainModule::defultModule() const
{
    return nullptr;
}
