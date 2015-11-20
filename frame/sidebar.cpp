#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPropertyAnimation>

#include "sidebar.h"
#include "constants.h"

SideBar::SideBar(QWidget *parent)
    : QFrame(parent)
{
    m_pluginsManager = PluginsManager::getInstance(this);

    setFixedWidth(DCC::SideBarWidth);

    //setStyleSheet("SideBar { background-color: rgba(100, 0, 0, 50%) }");

    m_tips = new DTipsFrame;

    m_sidebarLayout = new QVBoxLayout;
    m_sidebarLayout->setContentsMargins(0, 24, 0, 0);
    m_sidebarLayout->setSpacing(24);

    // meta for home button
    ModuleMetaData home {
        "home",
        "home",
        "Home"
    };
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Home"));

    // meta for power button
    ModuleMetaData power {
        "power",
        "shutdown",
        "Power"
    };
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Power"));

    addSideBarButton(home);
    for (const ModuleMetaData &meta : m_pluginsManager->pluginsList())
        addSideBarButton(meta);
    addSideBarButton(power);

    m_sidebarLayout->addStretch();
    setLayout(m_sidebarLayout);

    connect(m_pluginsManager, &PluginsManager::pluginInserted, this, &SideBar::insertPlugin);
    connect(m_pluginsManager, &PluginsManager::pluginRemoved, this, &SideBar::removePlugin);
}

void SideBar::enterEvent(QEvent *e)
{
    QFrame::enterEvent(e);
}

void SideBar::leaveEvent(QEvent *e)
{
//    m_tips->hide();
    QMetaObject::invokeMethod(m_tips, "hide", Qt::QueuedConnection);

    QFrame::leaveEvent(e);
}

void SideBar::addSideBarButton(const ModuleMetaData &meta)
{
    SideBarButton *button = new SideBarButton(meta, this);
    m_sidebarLayout->addWidget(button);
    m_sidebarLayout->setAlignment(button, Qt::AlignHCenter);

    connect(button, &SideBarButton::clicked, this, &SideBar::onSideBarButtonClicked);
    connect(button, &SideBarButton::hovered, m_tips, &DTipsFrame::followTheSender, Qt::QueuedConnection);
    connect(button, &SideBarButton::hovered, [this, button] {m_tips->setTipsText(button->metaData().name);});
}

void SideBar::insertPlugin(const int position, const ModuleMetaData &meta)
{
    SideBarButton *button = new SideBarButton(meta, this);
    m_sidebarLayout->insertWidget(position, button);
    m_sidebarLayout->setAlignment(button, Qt::AlignHCenter);

    connect(button, &SideBarButton::clicked, this, &SideBar::onSideBarButtonClicked);
    connect(button, &SideBarButton::hovered, m_tips, &DTipsFrame::followTheSender, Qt::QueuedConnection);
    connect(button, &SideBarButton::hovered, [this, button] {m_tips->setTipsText(button->metaData().name);});
}

void SideBar::removePlugin(const ModuleMetaData &meta)
{
    const int count = m_sidebarLayout->count();
    for (int i(0); i != count; ++i)
    {
        SideBarButton *btn = qobject_cast<SideBarButton *>(m_sidebarLayout->itemAt(i)->widget());
        if (!btn)
            continue;

        if (btn->metaData().id == meta.id)
        {
            // go back to home screen if removed is current selected
            if (m_selectedBtn == btn)
                switchToModule("home");

            m_sidebarLayout->removeWidget(btn);
            btn->deleteLater();
            break;
        }
    }
}

// private slots
void SideBar::onSideBarButtonClicked()
{
    SideBarButton *button = qobject_cast<SideBarButton *>(sender());

    if (!button || button == m_selectedBtn)
        return;

    switchToModule(button->metaData().id);
}

void SideBar::switchToModule(const QString &pluginId)
{
    SideBarButton *btn = findChild<SideBarButton *>(pluginId);

    if (!btn)
        return;

    if (m_selectedBtn)
    {
        if (m_selectedBtn->metaData().id == pluginId)
            return;
        else
            m_selectedBtn->release();
    }

    m_selectedBtn = btn;

    if (m_selectedBtn->metaData().id != "home" && m_selectedBtn->metaData().id != "shutdown")
        m_selectedBtn->presse();

    emit moduleSelected(m_selectedBtn->metaData());
}

DTipsFrame *SideBar::getTipFrame() const
{
    return m_tips;
}

// SideBarButton
SideBarButton::SideBarButton(ModuleMetaData metaData, QWidget *parent) :
    QAbstractButton(parent),
    m_meta(metaData)
{
    setFixedSize(24, 24);
    setMouseTracking(true);

    m_icon = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_icon);
    layout->setAlignment(m_icon, Qt::AlignCenter);
    layout->addStretch();
    layout->setMargin(0);
    layout->setSpacing(0);

    setState(Normal);

    setObjectName(metaData.id);
    setAccessibleName(metaData.id);
}

void SideBarButton::enterEvent(QEvent *)
{
    if (m_state == Normal) {
        this->setState(Hover);
    }

    emit hovered();
}

void SideBarButton::leaveEvent(QEvent *)
{
    if (m_state == Hover) {
        this->setState(Normal);
    }
}

void SideBarButton::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void SideBarButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
}

void SideBarButton::presse()
{
    setState(Selected);
}

void SideBarButton::release()
{
    setState(Normal);
}

void SideBarButton::setState(State state)
{
    m_state = state;

#ifndef QT_DEBUG
    QString moduleIconsDir("/usr/share/dde-control-center/modules/icons/24/%1_%2.svg");
#else
    QString moduleIconsDir("modules/icons/24/%1_%2.svg");
#endif

    QString fileName;

    switch (state) {
    case Normal:
        fileName = moduleIconsDir.arg(m_meta.id).arg("normal");
        break;
    case Hover:
        fileName = moduleIconsDir.arg(m_meta.id).arg("hover");
        break;
    case Selected:
        fileName = moduleIconsDir.arg(m_meta.id).arg("press");
        break;
    }

    m_icon->setPixmap(QPixmap(fileName));
}
