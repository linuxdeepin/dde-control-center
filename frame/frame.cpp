#include "frame.h"
#include "homescreen.h"
#include "contentview.h"
#include "constants.h"
#include "dtipsframe.h"
#include "dbus/displayinterface.h"

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QDebug>
#include <QDesktopWidget>

#include <libdui/dapplication.h>

DUI_USE_NAMESPACE

Frame::Frame(QWidget *parent) :
    QFrame(parent),
    m_dbusXMouseArea(new DBusXMouseArea(this))
{
    QPalette palette;
    palette.setColor(QPalette::Background, DCC::BgLightColor);
    m_centeralWidget = new QWidget(this);
    m_centeralWidget->setPalette(palette);
    m_centeralWidget->setAutoFillBackground(true);

    listPlugins();
    m_contentView = new ContentView(m_modules, m_hideInLeft, m_centeralWidget);
    m_contentView->setFixedWidth(DCC::ControlCenterWidth);
    m_homeScreen = new HomeScreen(m_modules, m_centeralWidget);
    m_homeScreen->setFixedWidth(DCC::ControlCenterWidth);

    m_showAni = new QPropertyAnimation(m_centeralWidget, "pos");
    m_showAni->setDuration(DCC::FrameAnimationDuration);
    m_showAni->setEasingCurve(DCC::FrameShowCurve);

    m_hideAni = new QPropertyAnimation(m_centeralWidget, "pos");
    m_hideAni->setDuration(DCC::FrameAnimationDuration);
    m_hideAni->setEasingCurve(DCC::FrameHideCurve);

    const QSize frameSize(DCC::ControlCenterWidth, qApp->primaryScreen()->size().height());
    m_centeralWidget->setFixedSize(frameSize);
    m_contentView->setFixedSize(frameSize);
    m_homeScreen->setFixedSize(frameSize);

    DisplayInterface *display_dbus = new DisplayInterface(this);
    connect(display_dbus, &DisplayInterface::PrimaryChanged, this, &Frame::updateGeometry);
    connect(m_homeScreen, &HomeScreen::powerBtnClicked, [this] {hide(true);});
    connect(this, &Frame::hideInLeftChanged, this, &Frame::updateGeometry);
    connect(m_dbusXMouseArea, &DBusXMouseArea::ButtonRelease, this, &Frame::globalMouseReleaseEvent);
    connect(m_hideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
    connect(m_hideAni, &QPropertyAnimation::valueChanged, this, &Frame::xChanged);
    connect(m_showAni, &QPropertyAnimation::valueChanged, this, &Frame::xChanged);
    connect(m_showAni, &QPropertyAnimation::finished, m_centeralWidget, static_cast<void (QWidget::*)()>(&QWidget::update), Qt::QueuedConnection);
    connect(m_homeScreen, SIGNAL(moduleSelected(ModuleMetaData)), this, SLOT(selectModule(ModuleMetaData)));
    connect(m_contentView, &ContentView::homeSelected, [ = ] {this->selectModule(ModuleMetaData());});
    connect(m_contentView, &ContentView::shutdownSelected, m_homeScreen, &HomeScreen::powerButtonClicked, Qt::DirectConnection);
    connect(m_contentView, &ContentView::shutdownSelected, [this]() -> void {hide();});
    connect(m_homeScreen, &HomeScreen::showAniFinished, m_contentView, &ContentView::unloadOldPlugin);
    connect(this, &Frame::hideInLeftChanged, m_contentView, &ContentView::reLayout);

    setFixedSize(frameSize);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);
    setHideInLeft(false);
}

Frame::~Frame()
{
//    m_showAni->deleteLater();
//    m_hideAni->deleteLater();
}

void Frame::show(bool imme)
{
    if (m_visible || m_hideAni->state() == QPropertyAnimation::Running) {
        return;
    }
    m_visible = true;

    if (imme) {
        m_centeralWidget->move(0, 0);
    } else {
        int startX = m_hideInLeft ? -DCC::ControlCenterWidth : DCC::ControlCenterWidth;

        m_hideAni->stop();
        m_showAni->stop();
        m_showAni->setStartValue(QPoint(startX, 0));
        m_showAni->setEndValue(QPoint(0, 0));
        m_showAni->start();
    }

    QDBusPendingReply<QString> reply = m_dbusXMouseArea->RegisterFullScreen();
    reply.waitForFinished();
    m_dbusFullScreenKey = reply.value();

    QFrame::show();
    setFocus();
    activateWindow();

    emit xChanged();
}

void Frame::hide(bool imme)
{
    if (m_canNotHide || !m_visible || m_showAni->state() == QPropertyAnimation::Running) {
        return;
    }
    m_visible = false;

    if (imme) {
        QFrame::hide();
    } else {
        int endX = m_hideInLeft ? -DCC::ControlCenterWidth : DCC::ControlCenterWidth;

        m_hideAni->stop();
        m_showAni->stop();
        m_hideAni->setEndValue(QPoint(endX, 0));
        m_hideAni->setStartValue(QPoint(0, 0));
        m_hideAni->start();
    }

    m_dbusXMouseArea->UnregisterArea(m_dbusFullScreenKey).waitForFinished();

    emit xChanged();
}

// private methods
void Frame::listPlugins()
{
#ifndef QT_DEBUG
    QDir pluginsDir("/usr/share/dde-control-center/modules");
#else
    QDir pluginsDir("modules");
#endif

    // TODO: make this QStringList dynamic in the future to allow 3rd party modules.
    QStringList moduleOrder;
    moduleOrder << "account" << "display" << "defaultapps" << "personalization";
    moduleOrder << "network" << "bluetooth" << "sound" << "datetime" << "power";
    moduleOrder << "mouse" << "wacom" << "keyboard" << "shortcuts" << "grub" << "system_info";

    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!QLibrary::isLibrary(fileName)) {
            continue;
        }
        QString filePath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader pluginLoader(filePath);
        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();

        ModuleMetaData meta = {
            filePath,
            metaData.value("id").toString(),
            metaData.value("name").toString()
        };

        m_modules << meta;
    }

    qSort(m_modules.begin(), m_modules.end(), [&](const ModuleMetaData & data1, const ModuleMetaData & data2) {
        return moduleOrder.indexOf(data1.id) < moduleOrder.indexOf(data2.id);
    });
}

// private slots
void Frame::selectModule(ModuleMetaData metaData)
{
    qWarning() << metaData.path;

    if (!metaData.path.isNull() && !metaData.path.isEmpty()) {
        m_contentView->setModule(metaData);
        m_contentView->show();
        m_homeScreen->hide();
    } else {
        m_homeScreen->show();
        m_contentView->hide();
    }

    if (!m_visible) {
        show();
    }
}

void Frame::globalMouseReleaseEvent(int button, int x, int y)
{
    Q_UNUSED(button);

    if (!rect().contains(x - this->x(), y - this->y())) {
        hide();
    }
}

void Frame::hideAndShowAnotherSide()
{
    hide();
    m_hideInLeft = !m_hideInLeft;
    QTimer::singleShot(DCC::FrameAnimationDuration + 10, this, SLOT(hideAndShowAnotherSideFinish()));
}

void Frame::hideAndShowAnotherSideFinish()
{
    emit hideInLeftChanged(m_hideInLeft);
    show();
}

void Frame::selectModule(const QString &moduleId)
{
    qDebug() << "select to" << moduleId;
    for (const ModuleMetaData &data : m_modules)
        if (data.id == moduleId) {
            selectModule(data);
        }
}

int Frame::visibleFrameXPos()
{
    return pos().x() + m_centeralWidget->pos().x();
}

bool Frame::canNotHide() const
{
    return m_canNotHide;
}

void Frame::setHideInLeft(bool hideInLeft)
{
    if (m_hideInLeft == hideInLeft)
        return;

    m_hideInLeft = hideInLeft;

    emit hideInLeftChanged(hideInLeft);
}

void Frame::updateGeometry()
{
    DisplayInterface *inter = qobject_cast<DisplayInterface *>(sender());
    QScreen *primaryScreen = qApp->primaryScreen();

    if (inter)
    {
        for (QScreen *screen : qApp->screens())
        {
            if (screen->name() == inter->primary())
            {
                primaryScreen = screen;
                break;
            }
        }
    }

    int posX;
    if (m_hideInLeft)
        posX = primaryScreen->geometry().left();
    else
        posX = primaryScreen->geometry().right() - DCC::ControlCenterWidth + 1;

    move(posX, primaryScreen->geometry().y());
    setFixedHeight(primaryScreen->size().height());
    m_centeralWidget->setFixedHeight(primaryScreen->size().height());
    m_contentView->setFixedHeight(primaryScreen->size().height());
    m_homeScreen->setFixedHeight(primaryScreen->size().height());

    QFrame::updateGeometry();
}

void Frame::setCanNotHide(bool canNotHide)
{
    if (m_canNotHide == canNotHide)
        return;

    m_canNotHide = canNotHide;
    emit canNotHideChanged(canNotHide);
}

void Frame::toggle(bool inLeft)
{
    if (m_hideAni->state() == QPropertyAnimation::Running ||
        m_showAni->state() == QPropertyAnimation::Running)
        return;

    const bool lastState = isHideInLeft();

    if (lastState == inLeft)
        isVisible() ? hide() : show();
    else
        hideAndShowAnotherSide();
}
