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
      m_dbus(new Appearance(Service, Path, QDBusConnection::sessionBus(), this))
{
    m_dbus->setSync(false);
}

void PersonalizationWork::active()
{
    m_dbus->blockSignals(false);

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

    setList("gtk", m_dbus->List("gtk"));
    setList("icon", m_dbus->List("icon"));
    setList("cursor", m_dbus->List("cursor"));
    setList("standardfont", m_dbus->List("standardfont"));
    setList("monospacefont", m_dbus->List("monospacefont"));

    windowTheme->setDefault(m_dbus->gtkTheme());
    iconTheme->setDefault(m_dbus->iconTheme());
    cursorTheme->setDefault(m_dbus->cursorTheme());
    fontMono->setFontName(m_dbus->monospaceFont());
    fontStand->setFontName(m_dbus->standardFont());
    FontSizeChanged(m_dbus->fontSize());
}

void PersonalizationWork::deactive()
{
    m_dbus->blockSignals(true);
}

bool PersonalizationWork::isFont(const QString &type)
{
    if (type == "gtk" || type == "icon" || type == "cursor" || type == "fontsize") {
        return false;
    }

    if (type == "standardfont" || type == "monospacefont") {
        return true;
    }
    return false;
}

QString PersonalizationWork::getThumbnail(const QString &Type, const QString &key)
{
    //返回图片位置
    return  m_dbus->Thumbnail(Type, key);
}

QList<QJsonObject> PersonalizationWork::converToList(const QString &type, QJsonArray &array)
{
    QList<QJsonObject> list;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        if (!isFont(type)) {
            object.insert("url", QJsonValue(getThumbnail(type, object["Id"].toString())));
        }
        object.insert("type", QJsonValue(type));
        list.append(object);
    }
    return list;
}

void PersonalizationWork::setList(const QString &type, const QString &key)
{
    if (type == "gtk") {
        ThemeModel *windowTheme = m_model->getWindowModel();
        QJsonArray array = QJsonDocument::fromJson(key.toLocal8Bit().data()).array();

        windowTheme->setJson(converToList(type, array));
    }

    if (type == "icon") {
        ThemeModel *iconTheme = m_model->getIconModel();
        QJsonArray array = QJsonDocument::fromJson(key.toLocal8Bit().data()).array();

        iconTheme->setJson(converToList(type, array));
    }

    if (type == "cursor") {
        ThemeModel *cursorTheme = m_model->getMouseModel();
        QJsonArray array = QJsonDocument::fromJson(key.toLocal8Bit().data()).array();

        cursorTheme->setJson(converToList(type, array));
    }

    if (type == "standardfont") {
        FontModel *fontStand = m_model->getStandFontModel();
        QJsonArray array = QJsonDocument::fromJson(key.toLocal8Bit().data()).array();

        fontStand->setFontList(converToList(type, array));
    }

    if (type == "monospacefont") {
        FontModel *fontMono = m_model->getMonoFontModel();
        QJsonArray array = QJsonDocument::fromJson(key.toLocal8Bit().data()).array();
        fontMono->setFontList(converToList(type, array));
    }
}

void PersonalizationWork::FontSizeChanged(const int value) const
{
    FontSizeModel *fontSizeModel = m_model->getFontSizeModel();
    fontSizeModel->setFontSize(sizeToSliderValue(value));
}


int PersonalizationWork::sizeToSliderValue(const int value) const
{
    if (value < 11) {
        return -1;
    } else if (value > 15) {
        return 1;
    } else {
        return 0;
    }
}

int PersonalizationWork::sliderValueToSize(const int value) const
{
    switch (value) {
    case -1:
        return 10;
    case 0:
        return 12;
    case 1:
        return 16;
    default:
        return 12;
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
