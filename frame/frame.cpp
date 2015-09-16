#include "frame.h"
#include "homescreen.h"
#include "contentview.h"
#include "constants.h"

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

#include "anchors.h"

#include "dbus/displayinterface.h"
#include "dtipsframe.h"

DUI_USE_NAMESPACE

Frame::Frame(QWidget *parent) :
    QFrame(parent),
    m_dbusXMouseArea(new DBusXMouseArea(this))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    resize(0, 0);
    setFocusPolicy(Qt::StrongFocus);

    setStyleSheet(QString("Frame { background-color:%1;}").arg(DCC::BgLightColor.name()));

    this->listPlugins();

#ifdef QT_DEBUG
    HideInLeft = true;
#endif

    m_contentView = new ContentView(m_modules, HideInLeft, this);
    m_contentView->setFixedWidth(DCC::ControlCenterWidth);

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedWidth(DCC::ControlCenterWidth);

    connect(m_homeScreen, SIGNAL(moduleSelected(ModuleMetaData)), this, SLOT(selectModule(ModuleMetaData)));
    connect(m_contentView, &ContentView::homeSelected, [ = ] {this->selectModule(ModuleMetaData());});
    connect(m_contentView, &ContentView::shutdownSelected, m_homeScreen, &HomeScreen::powerButtonClicked, Qt::DirectConnection);
    connect(m_contentView, &ContentView::shutdownSelected, [this]() -> void {hide();});
    connect(m_homeScreen, &HomeScreen::showAniFinished, m_contentView, &ContentView::unloadOldPlugin);
    connect(this, &Frame::hideInLeftChanged, m_contentView, &ContentView::reLayout);

    m_showAni = new QPropertyAnimation(this, "geometry");
    m_showAni->setDuration(DCC::FrameAnimationDuration);
    m_showAni->setEasingCurve(DCC::FrameShowCurve);

    m_hideAni = new QPropertyAnimation(this, "geometry");
    m_hideAni->setDuration(DCC::FrameAnimationDuration);
    m_hideAni->setEasingCurve(DCC::FrameHideCurve);

    if(HideInLeft){
        AnchorsBase::setAnchor(m_homeScreen, Qt::AnchorRight, this, Qt::AnchorRight);
        AnchorsBase::setAnchor(m_contentView, Qt::AnchorRight, this, Qt::AnchorRight);
    }

    connect(m_dbusXMouseArea, &DBusXMouseArea::ButtonRelease, this, &Frame::globalMouseReleaseEvent);
    connect(m_hideAni, &QPropertyAnimation::finished, this, &QFrame::hide);

    m_primaryScreen = qApp->primaryScreen();
    updateFrameGeometry(m_primaryScreen->geometry());
    connect(m_primaryScreen, &QScreen::geometryChanged, this, &Frame::updateFrameGeometry);

    DisplayInterface *display_dbus = new DisplayInterface(this);
    connect(display_dbus, &DisplayInterface::PrimaryChanged, [this, display_dbus]{
        disconnect(m_primaryScreen, &QScreen::geometryChanged, this, &Frame::updateFrameGeometry);

        foreach (QScreen *s, qApp->screens()) {
            if(s->name() == display_dbus->primary()){
                m_primaryScreen = s;
                break;
            }
        }
        updateFrameGeometry(m_primaryScreen->geometry());
        connect(m_primaryScreen, &QScreen::geometryChanged, this, &Frame::updateFrameGeometry);
    });
}

Frame::~Frame()
{
    m_showAni->deleteLater();
    m_hideAni->deleteLater();
}

// override methods
void Frame::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    }
}

void Frame::show(bool imme)
{
    if (m_visible || m_hideAni->state() == QPropertyAnimation::Running) {
        return;
    }
    m_visible = true;

    if (imme) {
        updateFrameGeometry(m_primaryScreen->geometry());
        resize(DCC::ControlCenterWidth, height());
        QFrame::show();
    } else {
        QRect startRect(0, y(), 0, height());
        QRect endRect(0, y(), DCC::ControlCenterWidth, height());

        if (HideInLeft) {
            endRect.moveLeft(m_primaryScreen->geometry().left());
            startRect.moveLeft(endRect.left());
            startRect.setRight(endRect.left() -1);
        } else {
            endRect.moveRight(m_primaryScreen->geometry().right());
            startRect.moveRight(endRect.right());
            startRect.setLeft(endRect.right() + 1);
        }

        QFrame::show();
        m_hideAni->stop();
        m_showAni->stop();
        m_showAni->setStartValue(startRect);
        m_showAni->setEndValue(endRect);
        m_showAni->start();
    }

    setFocus();
    activateWindow();

    QDBusPendingReply<QString> reply = m_dbusXMouseArea->RegisterFullScreen();
    reply.waitForFinished();
    m_dbusFullScreenKey = reply.value();
    qDebug() << "register full screen: " << m_dbusFullScreenKey;
}

void Frame::hide(bool imme)
{
    if (!m_visible || m_showAni->state() == QPropertyAnimation::Running) {
        return;
    }
    m_visible = false;

    if (imme) {
        QFrame::hide();
    } else {
        QRect startRect(0, y(), 0, height());
        QRect endRect(0, y(), DCC::ControlCenterWidth, height());

        if (HideInLeft) {
            endRect.moveLeft(m_primaryScreen->geometry().left());
            startRect.moveLeft(endRect.left());
            startRect.setRight(endRect.left() -1);
        } else {
            endRect.moveRight(m_primaryScreen->geometry().right());
            startRect.moveRight(endRect.right());
            startRect.setLeft(endRect.right() + 1);
        }

        m_showAni->stop();
        m_hideAni->stop();
        m_hideAni->setStartValue(endRect);
        m_hideAni->setEndValue(startRect);
        m_hideAni->start();
    }

    m_dbusXMouseArea->UnregisterArea(m_dbusFullScreenKey).waitForFinished();
}

bool Frame::isHideInLeft() const
{
    return HideInLeft;
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
    moduleOrder << "mouse" << "keyboard" << "shortcuts" << "grub" << "system_info";

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
            metaData.value("name").toString(),
            metaData.value("icon").toObject().value("normal").toString(),
            metaData.value("icon").toObject().value("hover").toString(),
            metaData.value("icon").toObject().value("selected").toString()
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

void Frame::updateFrameGeometry(QRect rect)
{
    setFixedHeight(rect.height());
    QRect tmp = this->geometry();
    tmp.moveTop(rect.top());
    if(HideInLeft)
        tmp.moveLeft(rect.left());
    else
        tmp.moveRight(rect.right());
    move(tmp.topLeft());

    m_contentView->setFixedHeight(this->height());
    m_homeScreen->setFixedHeight(this->height());
}

void Frame::selectModule(const QString &moduleId)
{
    qDebug() << "select to" << moduleId;
    for (const ModuleMetaData &data : m_modules)
        if (data.id == moduleId) {
            selectModule(data);
        }
}

void Frame::setHideInLeft(bool hideInLeft)
{
    if (HideInLeft == hideInLeft)
        return;

    HideInLeft = hideInLeft;
    if(HideInLeft){
        AnchorsBase::setAnchor(m_homeScreen, Qt::AnchorRight, this, Qt::AnchorRight);
        AnchorsBase::setAnchor(m_contentView, Qt::AnchorRight, this, Qt::AnchorRight);
    }else{
        AnchorsBase::clearAnchors(m_homeScreen);
        AnchorsBase::clearAnchors(m_contentView);
        AnchorsBase::clearAnchors(this);
        m_homeScreen->move(0, 0);
        m_contentView->move(0, 0);
    }

    updateFrameGeometry(m_primaryScreen->geometry());
    emit hideInLeftChanged(hideInLeft);
}

void Frame::moveEvent(QMoveEvent *e)
{
    QFrame::moveEvent(e);

    emit xChanged();
}
