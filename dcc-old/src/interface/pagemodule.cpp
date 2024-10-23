//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "pagemodule.h"

#include <QEvent>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWidget>

#include <DGuiApplicationHelper>
#include <DSizeMode>
using namespace DCC_NAMESPACE;

#define DCC_NO_Scroll 0x00080000  // 无滚动条(父项)
#define DCC_NO_STRETCH 0x00040000 // 无下方弹簧(父项)

namespace DCC_NAMESPACE {
class PageModulePrivate : public QObject
{
public:
    explicit PageModulePrivate(PageModule *parent)
        : QObject(parent)
        , q_ptr(parent)
        , m_spacing(Dtk::Widget::DSizeModeHelper::element(6, 10))
        , m_minimumWidth(0)
        , m_maximumWidth(QWIDGETSIZE_MAX)
    {
        clearData();
        QObject::connect(q_ptr, &PageModule::currentModuleChanged, q_ptr, [this](ModuleObject *currentModule) {
            onCurrentModuleChanged(currentModule);
        });
    }
    void insertModule(ModuleObject *const module, int stretch, Qt::Alignment alignment)
    {
        m_mapModules.insert(module, { stretch, alignment });
    }
    void removeModule(ModuleObject *const module)
    {
        m_mapModules.remove(module);
    }
    QPair<int, Qt::Alignment> layoutParam(ModuleObject *const module)
    {
        if (m_mapModules.contains(module))
            return m_mapModules.value(module);
        return { 0, Qt::Alignment() };
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
            mainLayout->addWidget(areaWidget, 0, m_contentsMargins.isNull() ? Qt::Alignment() : Qt::AlignHCenter);
        } else {
            m_area = new QScrollArea(parentWidget);
            m_area->installEventFilter(this);
            m_area->setFrameShape(QFrame::NoFrame);
            m_area->setWidgetResizable(true);
            areaWidget->setParent(m_area);
            m_area->setWidget(areaWidget);
            if (!m_contentsMargins.isNull())
                m_area->setAlignment(Qt::AlignHCenter);
            mainLayout->addWidget(m_area);
        }

        QWidget *controlRestrictorWidget = new QWidget;
        controlRestrictorWidget->setMinimumWidth(m_minimumWidth);
        controlRestrictorWidget->setMaximumWidth(500);
        controlRestrictorWidget->setLayout(m_hlayout);
        QHBoxLayout *controlRestrictorLayout = new QHBoxLayout;
        controlRestrictorLayout->setContentsMargins(m_contentsMargins.left(),
                                                    0,
                                                    m_contentsMargins.right(),
                                                    0);
        controlRestrictorLayout->addWidget(controlRestrictorWidget);
        mainLayout->addLayout(controlRestrictorLayout);

        m_vlayout = new QVBoxLayout(areaWidget);
        m_vlayout->setContentsMargins(m_contentsMargins);
        m_vlayout->setSpacing(m_spacing);
        areaWidget->setLayout(m_vlayout);
        areaWidget->setMinimumWidth(m_minimumWidth);
        areaWidget->setMaximumWidth(m_maximumWidth);

        for (auto &&tmpChild : q->childrens()) {
            auto page = tmpChild->activePage();
            if (page) {
                if (tmpChild->extra())
                    m_hlayout->addWidget(page);
                else {
                    QPair<int, Qt::Alignment> param = layoutParam(tmpChild);
                    m_vlayout->addWidget(page, param.first, param.second);
                }
                m_mapWidget.insert(tmpChild, page);
                page->setDisabled(ModuleObject::IsDisabled(tmpChild));
            }
        }
        if (!q->noStretch())
            m_vlayout->addStretch(1);
        
        if (m_hlayout->count() == 0) {
            controlRestrictorWidget->hide();
        }

        // 监听子项的添加、删除、状态变更，动态的更新界面
        QObject::connect(q, &ModuleObject::insertedChild, areaWidget, [this](ModuleObject *const childModule) { onAddChild(childModule); });
        QObject::connect(q, &ModuleObject::removedChild, areaWidget, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, areaWidget, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHiddenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        QObject::connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::sizeModeChanged, m_vlayout, [this](){
            m_spacing = Dtk::Widget::DSizeModeHelper::element(6, 10);
            m_vlayout->setSpacing(m_spacing);
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
        if (ModuleObject::IsHidden(childModule) || m_mapWidget.contains(childModule))
            return;

        Q_Q(PageModule);
        bool isExtra = childModule->extra();
        int index = 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHidden(child) && child->extra() == isExtra)
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            if (isExtra) {
                m_hlayout->insertWidget(index, newPage);
            } else {
                QPair<int, Qt::Alignment> param = layoutParam(childModule);
                m_vlayout->insertWidget(index, newPage, param.first, param.second);
            }

            newPage->setDisabled(ModuleObject::IsDisabled(childModule));
            m_mapWidget.insert(childModule, newPage);
        }
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (QEvent::Resize == event->type() && m_area) {
            QResizeEvent *e = static_cast<QResizeEvent *>(event);
            int left, right, top, bottom;
            m_vlayout->getContentsMargins(&left, &top, &right, &bottom);
            int width = qMin(e->size().width() - left - right, m_maximumWidth);
            if (m_maximumWidth >= width && width > 0) {
                for (int i = 0; i < m_vlayout->count(); ++i) {
                    QAbstractScrollArea *w = qobject_cast<QAbstractScrollArea *>(m_vlayout->itemAt(i)->widget());
                    if (w) {
                        w->setMaximumWidth(width);
                    }
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
    QMap<ModuleObject *, QPair<int, Qt::Alignment>> m_mapModules;
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    QMargins m_contentsMargins;
    int m_spacing;
    int m_minimumWidth;
    int m_maximumWidth;
};

}

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

PageModule::PageModule(const ModuleInitContext &message, QObject *parent)
    : ModuleObject(message, parent)
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

int PageModule::maximumWidth() const
{
    Q_D(const PageModule);
    return d->m_maximumWidth;
}

void PageModule::setMaximumWidth(int maxw)
{
    Q_D(PageModule);
    d->m_maximumWidth = maxw;
}

int PageModule::minimumWidth() const
{
    Q_D(const PageModule);
    return d->m_minimumWidth;
}

void PageModule::setMinimumWidth(int minw)
{
    Q_D(PageModule);
    d->m_minimumWidth = minw;
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

void PageModule::appendChild(ModuleObject *const module)
{
    appendChild(module, 0, Qt::Alignment());
}

void PageModule::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module)
{
    insertChild(before, module, 0, Qt::Alignment());
}

void PageModule::insertChild(const int index, ModuleObject *const module)
{
    insertChild(index, module, 0, Qt::Alignment());
}

void PageModule::removeChild(ModuleObject *const module)
{
    Q_D(PageModule);
    d->removeModule(module);
    ModuleObject::removeChild(module);
}

void PageModule::removeChild(const int index)
{
    Q_D(PageModule);
    d->removeModule(children(index));
    ModuleObject::removeChild(index);
}

void PageModule::appendChild(ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(PageModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::appendChild(module);
}

void PageModule::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(PageModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::insertChild(before, module);
}

void PageModule::insertChild(const int index, ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(PageModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::insertChild(index, module);
}

QWidget *PageModule::page()
{
    Q_D(PageModule);
    return d->page();
}
