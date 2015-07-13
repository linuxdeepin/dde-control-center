#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#include "sidebar.h"
#include "constants.h"

SideBar::SideBar(QList<ModuleMetaData> modules, QWidget *parent)
    : QFrame(parent)
{
    setFixedWidth(DCC::SideBarWidth);

    setStyleSheet("SideBar { background-color: rgba(100, 0, 0, 50%) }");

    QVBoxLayout * layout = new QVBoxLayout(this);

    // meta for home button
    ModuleMetaData home {
        "",
        "Back to home",
        "home_normal.png",
        "home_hover.png",
        "home_press.png"
    };
    modules.insert(0, home);

    foreach (ModuleMetaData meta, modules) {
        SideBarButton * button = new SideBarButton(meta, this);
        layout->addWidget(button);

        connect(button, &SideBarButton::clicked, this, &SideBar::onSideBarButtonClicked);
    }

    layout->addStretch();
}


// private slots
void SideBar::onSideBarButtonClicked()
{
    SideBarButton *button = qobject_cast<SideBarButton*>(sender());
    if (button) {
        ModuleMetaData meta = button->metaData();

        emit moduleSelected(meta);
    }
}

// SideBarButton
SideBarButton::SideBarButton(ModuleMetaData metaData, QWidget * parent) :
    QFrame(parent),
    m_meta(metaData)
{
    setFixedSize(36, 36);
    setMouseTracking(true);
    setStyleSheet("SideBarButton { background-color: green }");

    m_icon = new QLabel(this);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_icon);

    setState(Normal);
}

ModuleMetaData SideBarButton::metaData()
{
    return m_meta;
}

void SideBarButton::enterEvent(QEvent *)
{
    this->setState(Hover);
}

void SideBarButton::leaveEvent(QEvent *)
{
    this->setState(Normal);
}

void SideBarButton::mousePressEvent(QMouseEvent *)
{
    this->setState(Selected);
    emit clicked();
}

void SideBarButton::mouseReleaseEvent(QMouseEvent *)
{
//    this->setState(Hover);
}

void SideBarButton::setState(State state)
{
    switch (state) {
    case Normal:
        m_icon->setPixmap(QPixmap(QString("modules/icons/%1").arg(m_meta.normalIcon)));
        break;
    case Hover:
        m_icon->setPixmap(QPixmap(QString("modules/icons/%1").arg(m_meta.hoverIcon)));
        break;
    case Selected:
        m_icon->setPixmap(QPixmap(QString("modules/icons/%1").arg(m_meta.selectedIcon)));
        break;
    default:
        break;
    }
}
