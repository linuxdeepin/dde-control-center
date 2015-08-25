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

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(DCC::ControlCenterWidth);
    setFixedHeight(DUI::DApplication::desktop()->height());
    setFocusPolicy(Qt::StrongFocus);

    setStyleSheet(QString("Frame { background-color:%1;}").arg(DCC::BgLightColor.name()));

    this->listPlugins();

    m_contentView = new ContentView(m_modules, this);
    m_contentView->setFixedWidth(this->width());
    m_contentView->setFixedHeight(this->height());

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedWidth(this->width());
    m_homeScreen->setFixedHeight(this->height());

    connect(m_homeScreen, SIGNAL(moduleSelected(ModuleMetaData)), this, SLOT(selectModule(ModuleMetaData)));
    connect(m_contentView, &ContentView::homeSelected, [=] { ModuleMetaData meta; this->selectModule(meta);});

    m_dbusAdaptor = new DBusControlCenter(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerObject("/ControlCenter", this);
    conn.registerService("com.deepin.dde.ControlCenter");

    m_showAni = new QPropertyAnimation(this, "geometry");
    m_showAni->setDuration(DCC::FrameAnimationDuration);
    m_showAni->setEasingCurve(DCC::FrameShowCurve);

    m_hideAni = new QPropertyAnimation(this, "geometry");
    m_hideAni->setDuration(DCC::FrameAnimationDuration);
    m_hideAni->setEasingCurve(DCC::FrameHideCurve);

#ifdef QT_DEBUG
    HideInLeft = true;
#endif

    connect(m_hideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
}

Frame::~Frame()
{
    m_showAni->deleteLater();
    m_hideAni->deleteLater();
    m_dbusAdaptor->deleteLater();
}

void Frame::changeEvent(QEvent *e)
{
#ifndef QT_DEBUG // for test, we dont need it disapper
    if (m_visible && !isActiveWindow() && e->type() == QEvent::ActivationChange)
        hide();
#endif

    QFrame::changeEvent(e);
}

// override methods
void Frame::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    }
}

void Frame::show(bool imme)
{
    if (m_visible)
        return;
    m_visible = true;

    int endX = 0;

    if (imme)
    {
        QFrame::move(endX, 0);
        QFrame::show();
    }
    else
    {
        int startX = 0;

        if (HideInLeft)
        {
            startX = -DCC::ControlCenterWidth;
            endX = 0;
        }
        else
        {
            startX = DUI::DApplication::desktop()->width();
            endX = DUI::DApplication::desktop()->width() - width();
        }

        QFrame::move(startX, 0);
        QFrame::show();
        m_hideAni->stop();
        m_showAni->stop();
        m_showAni->setStartValue(QRect(startX, 0, width(), height()));
        m_showAni->setEndValue(QRect(endX, 0, width(), height()));
        m_showAni->start();
    }

    setFocus();
    activateWindow();
}

void Frame::hide(bool imme)
{
    if (!m_visible)
        return;
    m_visible = false;

    int endX = 0;

    if (imme)
    {
        QFrame::move(endX, 0);
        QFrame::hide();
    }
    else
    {
        int startX = 0;

        if (HideInLeft)
        {
            endX = -DCC::ControlCenterWidth;
            startX = 0;
        }
        else
        {
            endX = DUI::DApplication::desktop()->width();
            startX = DUI::DApplication::desktop()->width() - width();
        }

        QFrame::move(startX, 0);
        m_showAni->stop();
        m_hideAni->stop();
        m_hideAni->setStartValue(QRect(startX, 0, width(), height()));
        m_hideAni->setEndValue(QRect(endX, 0, width(), height()));
        m_hideAni->start();
    }
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

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!QLibrary::isLibrary(fileName))
            continue;
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

    qSort(m_modules.begin(), m_modules.end(), [&](const ModuleMetaData & data1, const ModuleMetaData & data2){
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

    if (!m_visible)
        show();
}

void Frame::selectModule(const QString &moduleName)
{
    qDebug() << "select to" << moduleName;
    for (const ModuleMetaData & data : m_modules)
        if (data.name == moduleName)
            return selectModule(data);
}
