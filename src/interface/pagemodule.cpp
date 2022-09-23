#include "pagemodule.h"

#include <QEvent>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWidget>

DCC_USE_NAMESPACE

#define DCC_NO_Scroll 0x00080000  // 无滚动条(父项)
#define DCC_NO_STRETCH 0x00040000 // 无下方弹簧(父项)

DCC_BEGIN_NAMESPACE
class PageModulePrivate : public QObject
{
public:
    explicit PageModulePrivate(PageModule *parent)
        : QObject(parent)
        , q_ptr(parent)
        , m_spacing(10)
    {
        m_contentsMargins.setLeft(40);
        m_contentsMargins.setRight(40);
        m_contentsMargins.setTop(10);
        m_contentsMargins.setBottom(10);
        clearData();
        QObject::connect(q_ptr, &PageModule::currentModuleChanged, q_ptr, [this](ModuleObject *currentModule) {
            onCurrentModuleChanged(currentModule);
        });
    }
    void clearData()
    {
        m_vlayout = nullptr;
        m_hlayout = nullptr;
        m_area = nullptr;
        m_mapWidget.clear();
    }

    QWidget *page()
    {
        Q_Q(PageModule);
        QWidget *parentWidget = new QWidget();

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setContentsMargins(0, 0, 0, 0);
        parentWidget->setLayout(mainLayout);
        m_hlayout = new QHBoxLayout();
        QObject::connect(parentWidget, &QObject::destroyed, q, [this]() { clearData(); });

        QWidget *areaWidget = new QWidget();
        if (q->noScroll()) {
            m_area = nullptr;
            areaWidget->setParent(parentWidget);
            mainLayout->addWidget(areaWidget);
        } else {
            m_area = new QScrollArea(parentWidget);
            m_area->installEventFilter(this);
            m_area->setFrameShape(QFrame::NoFrame);
            m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_area->setWidgetResizable(true);
            areaWidget->setParent(m_area);
            m_area->setWidget(areaWidget);
            mainLayout->addWidget(m_area);
        }
        mainLayout->addLayout(m_hlayout);

        m_vlayout = new QVBoxLayout(areaWidget);
        m_vlayout->setContentsMargins(m_contentsMargins);
        m_vlayout->setSpacing(m_spacing);
        areaWidget->setLayout(m_vlayout);

        for (auto &&tmpChild : q->childrens()) {
            auto page = tmpChild->activePage();
            if (page) {
                if (tmpChild->extra())
                    m_hlayout->addWidget(page);
                else {
                    m_vlayout->addWidget(page);
                }
                m_mapWidget.insert(tmpChild, page);
            }
        }
        if (!q->noStretch())
            m_vlayout->addStretch(1);

        auto addModuleSlot = [this](ModuleObject *const tmpChild) {
            onAddChild(tmpChild);
        };
        // 监听子项的添加、删除、状态变更，动态的更新界面
        QObject::connect(q, &ModuleObject::insertedChild, areaWidget, addModuleSlot);
        QObject::connect(q, &ModuleObject::appendedChild, areaWidget, addModuleSlot);
        QObject::connect(q, &ModuleObject::removedChild, areaWidget, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, areaWidget, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHidenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        Q_EMIT q->currentModuleChanged(q->currentModule());
        return parentWidget;
    }

private:
    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (m_area && m_mapWidget.contains(child)) {
            QWidget *w = m_mapWidget.value(child);
            if (-1 != m_vlayout->indexOf(w)) {
                QPoint p = w->mapTo(w->parentWidget(), QPoint());
                m_area->verticalScrollBar()->setSliderPosition(p.y());
            }
        }
    }
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (m_mapWidget.contains(childModule)) {
            QWidget *w = m_mapWidget.value(childModule);
            int index = m_vlayout->indexOf(w);
            if (-1 != index) {
                w->deleteLater();
                delete m_vlayout->takeAt(index);
                m_mapWidget.remove(childModule);
                return;
            }
            index = m_hlayout->indexOf(w);
            if (-1 != index) {
                w->deleteLater();
                delete m_hlayout->takeAt(index);
                m_mapWidget.remove(childModule);
            }
        }
    }
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (ModuleObject::IsHiden(childModule) || m_mapWidget.contains(childModule))
            return;

        Q_Q(PageModule);
        bool isExtra = childModule->extra();
        int index = 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHiden(child) && child->extra() == isExtra)
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            if (isExtra)
                m_hlayout->insertWidget(index, newPage);
            else
                m_vlayout->insertWidget(index, newPage);

            m_mapWidget.insert(childModule, newPage);
        }
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (QEvent::Resize == event->type() && m_area) {
            QResizeEvent *e = static_cast<QResizeEvent *>(event);
            int width = e->size().width();
            for (int i = 0; i < m_vlayout->count(); ++i) {
                QAbstractScrollArea *w = qobject_cast<QAbstractScrollArea *>(m_vlayout->itemAt(i)->widget());
                if (w) {
                    int left, right, top, bottom;
                    m_vlayout->getContentsMargins(&left, &top, &right, &bottom);
                    w->setMaximumWidth(width - left - right);
                }
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    PageModule *q_ptr;
    Q_DECLARE_PUBLIC(PageModule)

    QVBoxLayout *m_vlayout; // 上方纵向布局
    QHBoxLayout *m_hlayout; // 底下横向布局
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    QMargins m_contentsMargins;
    int m_spacing;
};

DCC_END_NAMESPACE

PageModule::PageModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, contentText, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, contentText, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, icon, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::PageModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, contentText, icon, parent)
    , DCC_INIT_PRIVATE(PageModule)
{
}

PageModule::~PageModule()
{
}

int PageModule::spacing() const
{
    Q_D(const PageModule);
    return d->m_spacing;
}

void PageModule::setSpacing(const int spacing)
{
    Q_D(PageModule);
    d->m_spacing = spacing;
}

void PageModule::getContentsMargins(int *left, int *top, int *right, int *bottom) const
{
    Q_D(const PageModule);
    *left = d->m_contentsMargins.left();
    *top = d->m_contentsMargins.top();
    *right = d->m_contentsMargins.right();
    *bottom = d->m_contentsMargins.bottom();
}

void PageModule::setContentsMargins(int left, int top, int right, int bottom)
{
    Q_D(PageModule);
    d->m_contentsMargins.setLeft(left);
    d->m_contentsMargins.setTop(top);
    d->m_contentsMargins.setRight(right);
    d->m_contentsMargins.setBottom(bottom);
}

bool PageModule::noScroll()
{
    return getFlagState(DCC_NO_Scroll);
}

void PageModule::setNoScroll(bool value)
{
    setFlagState(DCC_NO_Scroll, value);
}

bool PageModule::noStretch()
{
    return getFlagState(DCC_NO_STRETCH);
}

void PageModule::setNoStretch(bool value)
{
    setFlagState(DCC_NO_STRETCH, value);
}
QWidget *PageModule::page()
{
    Q_D(PageModule);
    return d->page();
}
