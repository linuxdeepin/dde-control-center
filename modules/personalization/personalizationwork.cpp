#include "personalizationwork.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"

using namespace dcc;
using namespace dcc::personalization;

const QString Service = "com.deepin.daemon.Appearance";
const QString Path    = "/com/deepin/daemon/Appearance";

PersonalizationWork::PersonalizationWork(PersonalizationModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_dbus(new Appearance(Service, Path, QDBusConnection::sessionBus(), this)),
      m_wmSwitcher(new wm_switcher("com.deepin.wm_switcher", "/com/deepin/wm_switcher", QDBusConnection::sessionBus(), this))
{
    ThemeModel *cursorTheme      = m_model->getMouseModel();
    ThemeModel *windowTheme      = m_model->getWindowModel();
    ThemeModel *iconTheme        = m_model->getIconModel();
    FontModel *fontMono          = m_model->getMonoFontModel();
    FontModel *fontStand         = m_model->getStandFontModel();

    connect(m_dbus, &Appearance::GtkThemeChanged,      windowTheme,   &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::CursorThemeChanged,   cursorTheme,   &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::IconThemeChanged,     iconTheme,     &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::MonospaceFontChanged, fontMono,      &FontModel::setFontName);
    connect(m_dbus, &Appearance::StandardFontChanged,  fontStand,     &FontModel::setFontName);
    connect(m_dbus, &Appearance::FontSizeChanged, this, &PersonalizationWork::FontSizeChanged);
    connect(m_dbus, &Appearance::Refreshed, this, &PersonalizationWork::onRefreshedChanged);

    connect(m_wmSwitcher, &wm_switcher::toggleWM, this, &PersonalizationWork::onToggleWM);

    m_dbus->setSync(false);
    m_wmSwitcher->setSync(false);
}

void PersonalizationWork::active()
{
    m_dbus->blockSignals(false);
    m_wmSwitcher->blockSignals(false);

    onGetList();
    onToggleWM();
}

void PersonalizationWork::deactive()
{
    m_dbus->blockSignals(true);
    m_wmSwitcher->blockSignals(true);
}

QList<QJsonObject> PersonalizationWork::converToList(const QString &type, QJsonArray &array)
{
    QList<QJsonObject> list;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        object.insert("type", QJsonValue(type));
        list.append(object);
    }
    return list;
}

void PersonalizationWork::addList(ThemeModel *model, const QString &type, QJsonArray &array)
{
    QList<QString> list;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        object.insert("type", QJsonValue(type));
        model->addItem(object["Id"].toString(), object);
        list.append(object["Id"].toString());

        QDBusPendingReply<QString> pic = m_dbus->Thumbnail(type, object["Id"].toString());
        QDBusPendingCallWatcher *picWatcher = new QDBusPendingCallWatcher(pic, this);
        picWatcher->setProperty("category", type);
        picWatcher->setProperty("id", object["Id"].toString());
        connect(picWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetPicFinished);
    }

    for (const QString &id : model->getList().keys()) {
        if (!list.contains(id)) {
            model->removeItem(id);
        }
    }
}

void PersonalizationWork::FontSizeChanged(const double value) const
{
    FontSizeModel *fontSizeModel = m_model->getFontSizeModel();
    fontSizeModel->setFontSize(sizeToSliderValue(value));
}

void PersonalizationWork::onStandardFontFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    FontModel *fontStand = m_model->getStandFontModel();
    QJsonArray array = QJsonDocument::fromJson(reply.value().toLocal8Bit().data()).array();
    fontStand->setFontList(converToList("standardfont", array));
    fontStand->setFontName(m_dbus->standardFont());
    w->deleteLater();
}

void PersonalizationWork::onMonoFontFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    FontModel *fontMono = m_model->getMonoFontModel();
    QJsonArray array = QJsonDocument::fromJson(reply.value().toLocal8Bit().data()).array();
    fontMono->setFontList(converToList("monospacefont", array));
    fontMono->setFontName(m_dbus->monospaceFont());
    w->deleteLater();
}

void PersonalizationWork::onGetThemeFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    const QString category = w->property("category").toString();

    QJsonArray array = QJsonDocument::fromJson(reply.value().toUtf8()).array();

    if (category == "gtk") {
        ThemeModel *windowTheme = m_model->getWindowModel();
        addList(windowTheme, "gtk", array);
    } else if (category == "icon") {
        ThemeModel *iconTheme = m_model->getIconModel();
        addList(iconTheme, "icon", array);
    } else {
        ThemeModel *cursorTheme = m_model->getMouseModel();
        addList(cursorTheme, "cursor", array);
    }

    w->deleteLater();
}

void PersonalizationWork::onGetPicFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    const QString category = w->property("category").toString();
    const QString id = w->property("id").toString();

    if (category == "gtk") {
        ThemeModel *windowTheme = m_model->getWindowModel();
        windowTheme->addPic(id, reply.value());
    } else if (category == "icon") {
        ThemeModel *iconTheme = m_model->getIconModel();
        iconTheme->addPic(id, reply.value());
    } else {
        ThemeModel *cursorTheme = m_model->getMouseModel();
        cursorTheme->addPic(id, reply.value());
    }

    w->deleteLater();
}

void PersonalizationWork::onRefreshedChanged(const QString &type)
{
    if (type == "gtk") {
        QDBusPendingReply<QString> gtk = m_dbus->List("gtk");
        QDBusPendingCallWatcher *gtkWatcher = new QDBusPendingCallWatcher(gtk, this);
        gtkWatcher->setProperty("category", "gtk");
        connect(gtkWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);
    }

    if (type == "icon") {
        QDBusPendingReply<QString> icon = m_dbus->List("icon");
        QDBusPendingCallWatcher *iconWatcher = new QDBusPendingCallWatcher(icon, this);
        iconWatcher->setProperty("category", "icon");
        connect(iconWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);
    }

    if (type == "cursor") {
        QDBusPendingReply<QString> cursor = m_dbus->List("cursor");
        QDBusPendingCallWatcher *cursorWatcher = new QDBusPendingCallWatcher(cursor, this);
        cursorWatcher->setProperty("category", "cursor");
        connect(cursorWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);
    }
}

void PersonalizationWork::onToggleWM()
{
  QDBusPendingCallWatcher *wmWatcher = new QDBusPendingCallWatcher(m_wmSwitcher->currentWM(), this);
  connect(wmWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetCurrentWMFinished);
}

void PersonalizationWork::onGetCurrentWMFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = w->reply();

    m_model->setIs3DWm(reply.value() == "deepin wm");

    w->deleteLater();
}

void PersonalizationWork::onGetList()
{
    QDBusPendingReply<QString> standardFont = m_dbus->List("standardfont");
    QDBusPendingCallWatcher *standardFontWatcher = new QDBusPendingCallWatcher(standardFont, this);
    connect(standardFontWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onStandardFontFinished);

    QDBusPendingReply<QString> monoFont = m_dbus->List("monospacefont");
    QDBusPendingCallWatcher *monoFontWatcher = new QDBusPendingCallWatcher(monoFont, this);
    connect(monoFontWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onMonoFontFinished);

    QDBusPendingReply<QString> gtk = m_dbus->List("gtk");
    QDBusPendingCallWatcher *gtkWatcher = new QDBusPendingCallWatcher(gtk, this);
    gtkWatcher->setProperty("category", "gtk");
    connect(gtkWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);

    QDBusPendingReply<QString> icon = m_dbus->List("icon");
    QDBusPendingCallWatcher *iconWatcher = new QDBusPendingCallWatcher(icon, this);
    iconWatcher->setProperty("category", "icon");
    connect(iconWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);

    QDBusPendingReply<QString> cursor = m_dbus->List("cursor");
    QDBusPendingCallWatcher *cursorWatcher = new QDBusPendingCallWatcher(cursor, this);
    cursorWatcher->setProperty("category", "cursor");
    connect(cursorWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);

    ThemeModel *cursorTheme      = m_model->getMouseModel();
    ThemeModel *windowTheme      = m_model->getWindowModel();
    ThemeModel *iconTheme        = m_model->getIconModel();

    windowTheme->setDefault(m_dbus->gtkTheme());
    iconTheme->setDefault(m_dbus->iconTheme());
    cursorTheme->setDefault(m_dbus->cursorTheme());
    FontSizeChanged(m_dbus->fontSize());
}


int PersonalizationWork::sizeToSliderValue(const double value) const
{
    if (value <= 8.2) {
        return 0;
    } else if (value <= 9) {
        return 1;
    } else if (value <= 9.7) {
        return 2;
    } else if (value <= 11.2) {
        return 3;
    } else if (value <= 12) {
        return 4;
    } else if (value <= 13.5) {
        return 5;
    } else if (value >= 14.5) {
        return 6;
    } else {
        return 1;
    }
}

float PersonalizationWork::sliderValueToSize(const int value) const
{
    switch (value) {
    case 0:
        return 8.2;
    case 1:
        return 9;
    case 2:
        return 9.7;
    case 3:
        return 11.2;
    case 4:
        return 12;
    case 5:
        return 13.5;
    case 6:
        return 14.5;
    default:
        return 9;
    }
}

void PersonalizationWork::setDefault(const QJsonObject &value)
{
    //使用type去调用
    m_dbus->Set(value["type"].toString(), value["Id"].toString());
}

void PersonalizationWork::setFontSize(const int value)
{
    m_dbus->setFontSize(sliderValueToSize(value));
}

void PersonalizationWork::switchWM()
{
    m_wmSwitcher->requestSwitchWM();
}
