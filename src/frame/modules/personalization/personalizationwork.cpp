/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "personalizationwork.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"

using namespace dcc;
using namespace dcc::personalization;

const QString Service = "com.deepin.daemon.Appearance";
const QString Path    = "/com/deepin/daemon/Appearance";

static const std::vector<int> OPACITY_SLIDER {
    0,
    25,
    40,
    55,
    70,
    85,
    100
};

PersonalizationWork::PersonalizationWork(PersonalizationModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_dbus(new Appearance(Service, Path, QDBusConnection::sessionBus(), this)),
      m_wmSwitcher(new WMSwitcher("com.deepin.WMSwitcher", "/com/deepin/WMSwitcher", QDBusConnection::sessionBus(), this))
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

    connect(m_wmSwitcher, &WMSwitcher::WMChanged, this, &PersonalizationWork::onToggleWM);
    connect(m_dbus, &Appearance::OpacityChanged, this, &PersonalizationWork::refreshOpacity);

    m_themeModels["gtk"]           = windowTheme;
    m_themeModels["icon"]          = iconTheme;
    m_themeModels["cursor"]        = cursorTheme;
    m_fontModels["standardfont"]   = fontStand;
    m_fontModels["monospacefont"]  = fontMono;

    m_dbus->setSync(false);
    m_wmSwitcher->setSync(false);
}

void PersonalizationWork::active()
{
    m_dbus->blockSignals(false);
    m_wmSwitcher->blockSignals(false);

    refreshWMState();
    refreshOpacity(m_dbus->opacity());

    m_model->getWindowModel()->setDefault(m_dbus->gtkTheme());
    m_model->getIconModel()->setDefault(m_dbus->iconTheme());
    m_model->getMouseModel()->setDefault(m_dbus->cursorTheme());
    m_model->getMonoFontModel()->setFontName(m_dbus->monospaceFont());
    m_model->getStandFontModel()->setFontName(m_dbus->standardFont());
}

void PersonalizationWork::deactive()
{
    m_dbus->blockSignals(true);
    m_wmSwitcher->blockSignals(true);
}

QList<QJsonObject> PersonalizationWork::converToList(const QString &type, const QJsonArray &array)
{
    QList<QJsonObject> list;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        object.insert("type", QJsonValue(type));
        list.append(object);
    }
    return list;
}

void PersonalizationWork::addList(ThemeModel *model, const QString &type, const QJsonArray &array)
{
    QList<QString> list;
    QList<QJsonObject> objList;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        object.insert("type", QJsonValue(type));
        objList << object;
        list.append(object["Id"].toString());

        QDBusPendingReply<QString> pic = m_dbus->Thumbnail(type, object["Id"].toString());
        QDBusPendingCallWatcher *picWatcher = new QDBusPendingCallWatcher(pic, this);
        picWatcher->setProperty("category", type);
        picWatcher->setProperty("id", object["Id"].toString());
        connect(picWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetPicFinished);
    }

    // sort for display name
    std::sort(objList.begin(), objList.end(), [=] (const QJsonObject &obj1, const QJsonObject &obj2) {
        QCollator qc;
        return qc.compare(obj1["Id"].toString(), obj2["Id"].toString()) < 0;
    });

    for (const QJsonObject &obj : objList) {
        model->addItem(obj["Id"].toString(), obj);
    }

    for (const QString &id : model->getList().keys()) {
        if (!list.contains(id)) {
            model->removeItem(id);
        }
    }
}

void PersonalizationWork::refreshWMState()
{
    QDBusPendingCallWatcher *wmWatcher = new QDBusPendingCallWatcher(m_wmSwitcher->CurrentWM(), this);
    connect(wmWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetCurrentWMFinished);
}

void PersonalizationWork::FontSizeChanged(const double value) const
{
    FontSizeModel *fontSizeModel = m_model->getFontSizeModel();
    fontSizeModel->setFontSize(sizeToSliderValue(value));
}

void PersonalizationWork::onGetFontFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    if (!reply.isError()) {
        const QString &category = w->property("category").toString();

        setFontList(m_fontModels[category], category, reply.value());
    }
    else {
        qWarning() << reply.error();
    }

    w->deleteLater();
}

void PersonalizationWork::onGetThemeFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    if (!reply.isError()) {
        const QString &category = w->property("category").toString();
        const QJsonArray &array = QJsonDocument::fromJson(reply.value().toUtf8()).array();

        addList(m_themeModels[category], category, array);
    }
    else {
        qWarning() << reply.error();
    }

    w->deleteLater();
}

void PersonalizationWork::onGetPicFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    if (!reply.isError()) {
        const QString &category = w->property("category").toString();
        const QString &id = w->property("id").toString();

        m_themeModels[category]->addPic(id, reply.value());
    }
    else {
        qWarning() << reply.error();
    }

    w->deleteLater();
}

void PersonalizationWork::onRefreshedChanged(const QString &type)
{
    if (m_themeModels.keys().contains(type)) {
        refreshThemeByType(type);
    }

    if (m_fontModels.keys().contains(type)) {
        refreshFontByType(type);
    }
}

void PersonalizationWork::onToggleWM(const QString &wm)
{
    m_model->setIs3DWm(wm == "deepin wm");
}

void PersonalizationWork::onGetCurrentWMFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = w->reply();

    if (!reply.isError()) {
        onToggleWM(reply.value());
    }
    else {
        qWarning() << reply.error();
    }

    w->deleteLater();
}

void PersonalizationWork::setFontList(FontModel *model, const QString &type, const QString &list)
{
    QJsonArray array = QJsonDocument::fromJson(list.toLocal8Bit().data()).array();

    QStringList l;

    for (int i = 0; i != array.size(); i++)
        l << array.at(i).toString();

    QDBusPendingCallWatcher *watcher  = new QDBusPendingCallWatcher(m_dbus->Show(type, l), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        if (!w->isError()) {
            QDBusPendingReply<QString> r = w->reply();

            QJsonArray array = QJsonDocument::fromJson(r.value().toLocal8Bit().data()).array();

            QList<QJsonObject> list = converToList(type, array);
            // sort for display name
            std::sort(list.begin(), list.end(), [=] (const QJsonObject &obj1, const QJsonObject &obj2) {
                QCollator qc;
                return qc.compare(obj1["Name"].toString(), obj2["Name"].toString()) < 0;
            });

            model->setFontList(list);
        }
        else {
            qWarning() << w->error();
        }

        watcher->deleteLater();
    });
}

void PersonalizationWork::refreshTheme()
{
    for (QMap<QString, ThemeModel*>::ConstIterator it = m_themeModels.begin(); it != m_themeModels.end(); it++) {
        refreshThemeByType(it.key());
    }
}

void PersonalizationWork::refreshThemeByType(const QString &type)
{
    QDBusPendingReply<QString> theme = m_dbus->List(type);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(theme, this);
    watcher->setProperty("category", type);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetThemeFinished);
}

void PersonalizationWork::refreshFont()
{
    for (QMap<QString, FontModel*>::const_iterator it = m_fontModels.begin(); it != m_fontModels.end(); it++) {
        refreshFontByType(it.key());
    }

    FontSizeChanged(m_dbus->fontSize());
}

void PersonalizationWork::refreshFontByType(const QString &type) {
    QDBusPendingReply<QString> font = m_dbus->List(type);
    QDBusPendingCallWatcher *fontWatcher = new QDBusPendingCallWatcher(font, this);
    fontWatcher->setProperty("category", type);
    connect(fontWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetFontFinished);
}

void PersonalizationWork::refreshOpacity(double opacity)
{
    int slider { toSliderValue<int>(OPACITY_SLIDER, static_cast<int>(opacity * 100)) };
    qDebug() << QString("opacity: %1, slider: %2").arg(opacity).arg(slider);
    m_model->setOpacity(std::pair<int, double>(slider, opacity));
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

double PersonalizationWork::sliderValutToOpacity(const int value) const
{
    return static_cast<double>(OPACITY_SLIDER[value]) / static_cast<double>(100);
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
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_wmSwitcher->RequestSwitchWM(), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (watcher->isError()) {
            qDebug() << watcher->error();
        }
        watcher->deleteLater();
    });
}

void PersonalizationWork::setOpacity(int opacity)
{
    m_dbus->setOpacity(sliderValutToOpacity(opacity));
}

template<typename T>
T PersonalizationWork::toSliderValue(std::vector<T> list, T value)
{
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        if (value < *it) {
            return (--it) - list.begin();
        }
    }

    return list.end() - list.begin();
}
