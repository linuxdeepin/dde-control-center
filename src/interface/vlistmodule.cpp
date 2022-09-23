#include "moduledatamodel.h"
#include "vlistmodule.h"

#include <DListView>
#include <DVerticalLine>
#include <QHBoxLayout>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

DCC_BEGIN_NAMESPACE
class VListModulePrivate
{
public:
    explicit VListModulePrivate(VListModule *parent)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_layout(nullptr)
    {
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_layout)
            return;
        if (m_layout->count() > 2) {
            QLayoutItem *item = m_layout->takeAt(2);
            delete item->widget();
            delete item;
        }
        if (child) {
            QWidget *widget = nullptr;
            if (child->extra() && child->hasChildrens())
                widget = child->children(0)->activePage();
            else
                widget = child->activePage();
            if (widget) {
                widget->setSizePolicy(QSizePolicy::Expanding, widget->sizePolicy().verticalPolicy());
                m_layout->addWidget(widget, 5);
                ModuleDataModel *model = static_cast<ModuleDataModel *>(m_view->model());
                m_view->setCurrentIndex(model->index(child));
            }
        }
    }

    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        int index = m_extraModules.indexOf(childModule);
        if (index != -1) {
            QLayoutItem *item = m_hlayout->takeAt(index);
            item->widget()->deleteLater();
            delete item;
            m_extraModules.removeAt(index);
        }
    }

    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (ModuleObject::IsHiden(childModule) || !childModule->extra() || m_extraModules.contains(childModule))
            return;

        Q_Q(VListModule);
        int index = 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHiden(child) && child->extra())
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            m_hlayout->insertWidget(index, newPage);
            m_extraModules.insert(index, childModule);
        }
    }

    QWidget *page()
    {
        Q_Q(VListModule);
        QWidget *parentWidget = new QWidget();
        m_layout = new QHBoxLayout(parentWidget);
        parentWidget->setLayout(m_layout);
        QObject::connect(parentWidget, &QObject::destroyed, q, [this]() { m_layout = nullptr; });

        DListView *view = new DListView(parentWidget);
        QWidget *widget = new QWidget(parentWidget);
        QVBoxLayout *vlayout = new QVBoxLayout;
        m_hlayout = new QHBoxLayout;
        widget->setLayout(vlayout);
        vlayout->addWidget(view);
        vlayout->addLayout(m_hlayout);
        widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_layout->addWidget(widget, 1);
        m_layout->addWidget(new DVerticalLine, 1);

        ModuleDataModel *model = new ModuleDataModel(view);
        model->setModuleObject(q);
        QObject::connect(q, &VListModule::currentModuleChanged, m_layout, [this](ModuleObject *child) {
            onCurrentModuleChanged(child);
        });

        view->setModel(model);
        view->setFrameShape(QFrame::NoFrame);
        view->setAutoScroll(true);
        view->setDragEnabled(false);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setSpacing(0);
        view->setItemSpacing(2);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_view = view;

        for (auto tmpChild : q->childrens()) {
            auto page = tmpChild->activePage();
            if (page) {
                if (tmpChild->extra()) {
                    m_hlayout->addWidget(page);
                    m_extraModules.append(tmpChild);
                }
            }
        }

        auto onClicked = [](const QModelIndex &index) {
            ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
            if (obj && !ModuleObject::IsDisabled(obj))
                obj->trigger();
        };

        QObject::connect(m_view, &DListView::activated, m_view, &DListView::clicked);
        QObject::connect(m_view, &DListView::clicked, m_view, onClicked);
        auto addModuleSlot = [this](ModuleObject *const tmpChild) {
            onAddChild(tmpChild);
        };
        // 监听子项的添加、删除、状态变更，动态的更新界面
        QObject::connect(q, &ModuleObject::insertedChild, m_view, addModuleSlot);
        QObject::connect(q, &ModuleObject::appendedChild, m_view, addModuleSlot);
        QObject::connect(q, &ModuleObject::removedChild, m_view, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, m_view, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHidenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        onCurrentModuleChanged(q->currentModule());
        return parentWidget;
    }

private:
    VListModule *q_ptr;
    Q_DECLARE_PUBLIC(VListModule)

    QAbstractItemView *m_view;
    QHBoxLayout *m_layout;
    QHBoxLayout *m_hlayout;
    QList<DCC_NAMESPACE::ModuleObject *> m_extraModules;
};
DCC_END_NAMESPACE

VListModule::VListModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, contentText, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, contentText, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, contentText, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::~VListModule()
{
}

QWidget *VListModule::page()
{
    Q_D(VListModule);
    return d->page();
}
