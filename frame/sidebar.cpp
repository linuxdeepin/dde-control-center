#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPropertyAnimation>

#include "sidebar.h"
#include "constants.h"

SideBar::SideBar(QList<ModuleMetaData> modules, QWidget *parent)
    : QFrame(parent)
{
    setFixedWidth(DCC::SideBarWidth);

    //setStyleSheet("SideBar { background-color: rgba(100, 0, 0, 50%) }");

    m_tips = new DTipsFrame;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 24, 0, 0);
    layout->setSpacing(24);

    // meta for home button
    ModuleMetaData home {
        "",
        "home",
        "Back to home"
    };
    modules.insert(0, home);
    // meta for power button
    ModuleMetaData power {
        "",
        "shutdown",
        "Power"
    };
    modules.append(power);

    foreach(ModuleMetaData meta, modules) {
        SideBarButton *button = new SideBarButton(meta, this);
        layout->addWidget(button);
        layout->setAlignment(button, Qt::AlignHCenter);

        connect(button, &SideBarButton::clicked, this, &SideBar::onSideBarButtonClicked);
        connect(button, &SideBarButton::hovered, [this, button]() -> void {m_tips->setTipsText(button->metaData().name);});
        connect(button, &SideBarButton::hovered, m_tips, &DTipsFrame::show, Qt::DirectConnection);
        connect(button, &SideBarButton::hovered, m_tips, &DTipsFrame::followTheSender, Qt::QueuedConnection);
    }

    layout->addStretch();

    this->setLayout(layout);
}

void SideBar::enterEvent(QEvent *e)
{
    QFrame::enterEvent(e);
}

void SideBar::leaveEvent(QEvent *e)
{
    m_tips->hide();

    QFrame::leaveEvent(e);
}

void SideBar::switchToSideBarButton(SideBarButton *btn)
{
    qDebug() << "switchToSideBarButton: " << btn->metaData().name;

    if (m_selectedBtn) {
        m_selectedBtn->release();
    }

    m_selectedBtn = btn;

    if (!m_selectedBtn->metaData().path.isNull() && !m_selectedBtn->metaData().path.isEmpty()) {
        m_selectedBtn->presse();
    }
}

// private slots
void SideBar::onSideBarButtonClicked()
{
    SideBarButton *button = qobject_cast<SideBarButton *>(sender());

    if (!button) {
        return;
    }

    ModuleMetaData meta = button->metaData();
    switchToSideBarButton(button);

    emit moduleSelected(meta);
}

void SideBar::switchToModule(const ModuleMetaData &meta)
{
    SideBarButton *btn = findChild<SideBarButton *>(meta.name);

    if (!btn) {
        return;
    }

    switchToSideBarButton(btn);
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
//    setStyleSheet("background-color:red;");

    setObjectName(metaData.name);
    setAccessibleName(metaData.name);
}

ModuleMetaData SideBarButton::metaData()
{
    return m_meta;
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

void SideBarButton::mouseReleaseEvent(QMouseEvent *)
{
    //    this->setState(Hover);
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
