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

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

using namespace dcc;
using namespace dcc::personalization;

#define GSETTING_EFFECT_LOAD "effect-load"

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

#ifdef WINDOW_MODE
const QList<int> FontSizeList {11, 12, 13, 14, 15, 16, 18, 20};
#endif

PersonalizationWork::PersonalizationWork(PersonalizationModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_dbus(new Appearance(Service, Path, QDBusConnection::sessionBus(), this)),
      m_wmSwitcher(new WMSwitcher("com.deepin.WMSwitcher", "/com/deepin/WMSwitcher", QDBusConnection::sessionBus(), this)),
      m_wm(new WM("com.deepin.wm", "/com/deepin/wm", QDBusConnection::sessionBus(), this)),
      m_effects(new Effects("org.kde.KWin", "/Effects", QDBusConnection::sessionBus(), this))
{
    ThemeModel *cursorTheme      = m_model->getMouseModel();
    ThemeModel *windowTheme      = m_model->getWindowModel();
    ThemeModel *iconTheme        = m_model->getIconModel();
    FontModel *fontMono          = m_model->getMonoFontModel();
    FontModel *fontStand         = m_model->getStandFontModel();
    m_setting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);

    connect(m_dbus, &Appearance::GtkThemeChanged,      windowTheme,   &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::CursorThemeChanged,   cursorTheme,   &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::IconThemeChanged,     iconTheme,     &ThemeModel::setDefault);
    connect(m_dbus, &Appearance::MonospaceFontChanged, fontMono,      &FontModel::setFontName);
    connect(m_dbus, &Appearance::StandardFontChanged,  fontStand,     &FontModel::setFontName);
    connect(m_dbus, &Appearance::FontSizeChanged, this, &PersonalizationWork::FontSizeChanged);
    connect(m_dbus, &Appearance::Refreshed, this, &PersonalizationWork::onRefreshedChanged);

    //connect(m_wmSwitcher, &WMSwitcher::WMChanged, this, &PersonalizationWork::onToggleWM);
    connect(m_dbus, &Appearance::OpacityChanged, this, &PersonalizationWork::refreshOpacity);
    connect(m_dbus, &Appearance::QtActiveColorChanged, this, &PersonalizationWork::refreshActiveColor);
    connect(m_wm, &WM::CompositingAllowSwitchChanged, this, &PersonalizationWork::onCompositingAllowSwitch);
    connect(m_wm, &WM::compositingEnabledChanged, this, &PersonalizationWork::onWindowWM);


    //获取最小化设置
    if (m_setting->keys().contains("effectLoad", Qt::CaseSensitivity::CaseInsensitive)) {
        bool isMinEffect = m_setting->get(GSETTING_EFFECT_LOAD).toBool();
        m_model->setMiniEffect(isMinEffect);
        if (isMinEffect) {
            m_effects->loadEffect("magiclamp");
        } else {
            m_effects->unloadEffect("magiclamp");
        }
    } else {
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_effects->isEffectLoaded("magiclamp"), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] (QDBusPendingCallWatcher *watcher) {
            qDebug() << watcher->error();
            if (!watcher->isError()) {
                QDBusReply<bool> value = watcher->reply();
                if (value) {
                    m_model->setMiniEffect(1);
                } else {
                    m_model->setMiniEffect(0);
                }
            }
        });
    };

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
    refreshActiveColor(m_dbus->qtActiveColor());
    onCompositingAllowSwitch(m_wm->compositingAllowSwitch());

    m_model->getWindowModel()->setDefault(m_dbus->gtkTheme());
    m_model->getIconModel()->setDefault(m_dbus->iconTheme());
    m_model->getMouseModel()->setDefault(m_dbus->cursorTheme());
    m_model->getMonoFontModel()->setFontName(m_dbus->monospaceFont());
    m_model->getStandFontModel()->setFontName(m_dbus->standardFont());

    bool ok = false;
    QDBusInterface interface(Service, Path, Service, QDBusConnection::sessionBus());
    int radius = interface.property("WindowRadius").toInt(&ok);
    if (ok)
        m_model->setWindowRadius(radius);
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
    } else {
        qDebug() << reply.error();
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
    } else {
        qDebug() << reply.error();
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
    } else {
        qDebug() << reply.error();
    }

    w->deleteLater();
}

void PersonalizationWork::onGetActiveColorFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    if (!reply.isError()) {
        m_model->setActiveColor(reply.value());
    } else {
        qDebug() << reply.error();
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
    qDebug() << "onToggleWM: " << wm;
    m_model->setIs3DWm(wm == "deepin wm");
}

void PersonalizationWork::onWindowWM(bool value)
{
    qDebug() << "onWindowWM: " << value;
    m_model->setIs3DWm(value);
}

void PersonalizationWork::onCompositingAllowSwitch(bool value)
{
    m_model->setCompositingAllowSwitch(value);
}

void PersonalizationWork::onGetCurrentWMFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = w->reply();

    if (!reply.isError()) {
        onToggleWM(reply.value());
    } else {
        qDebug() << reply.error();
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

            QJsonArray arrayValue = QJsonDocument::fromJson(r.value().toLocal8Bit().data()).array();

            QList<QJsonObject> list = converToList(type, arrayValue);
            // sort for display name
            std::sort(list.begin(), list.end(), [=] (const QJsonObject &obj1, const QJsonObject &obj2) {
                QCollator qc;
                return qc.compare(obj1["Name"].toString(), obj2["Name"].toString()) < 0;
            });

            model->setFontList(list);
        } else {
            qDebug() << w->error();
        }

        watcher->deleteLater();
    });
}

void PersonalizationWork::refreshTheme()
{
    for (QMap<QString, ThemeModel *>::ConstIterator it = m_themeModels.begin(); it != m_themeModels.end(); it++) {
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
    for (QMap<QString, FontModel *>::const_iterator it = m_fontModels.begin(); it != m_fontModels.end(); it++) {
        refreshFontByType(it.key());
    }

    FontSizeChanged(m_dbus->fontSize());
}

void PersonalizationWork::refreshFontByType(const QString &type)
{
    QDBusPendingReply<QString> font = m_dbus->List(type);
    QDBusPendingCallWatcher *fontWatcher = new QDBusPendingCallWatcher(font, this);
    fontWatcher->setProperty("category", type);
    connect(fontWatcher, &QDBusPendingCallWatcher::finished, this, &PersonalizationWork::onGetFontFinished);
}

void PersonalizationWork::refreshActiveColor(const QString &color)
{
    m_model->setActiveColor(color);
}

bool PersonalizationWork::allowSwitchWM()
{
    QDBusPendingReply<bool> reply  = m_wmSwitcher->AllowSwitch();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *w) {
        if (w->isError())
            qDebug() << watcher->error();
        watcher->deleteLater();
    });
    return reply.value();
}

void PersonalizationWork::refreshOpacity(double opacity)
{
#ifdef WINDOW_MODE
    int slider { static_cast<int>(opacity * 100) };
#else
    int slider { toSliderValue<int>(OPACITY_SLIDER, static_cast<int>(opacity * 100)) };
#endif
    qDebug() << QString("opacity: %1, slider: %2").arg(opacity).arg(slider);
    m_model->setOpacity(std::pair<int, double>(slider, opacity));
}

#ifdef WINDOW_MODE
const int RENDER_DPI = 72;
const double DPI = 96;

double ptToPx(double pt) {
    double px = pt / RENDER_DPI * DPI + 0.5;
    return px;
}

double pxToPt(double px) {
    double pt = px * RENDER_DPI / DPI;
    return pt;
}

//字体大小通过点击刻度调整字体大小，可选刻度为：11px、12px、13px、14px、15px、16px、18px、20px;
//社区版默认值为12px；专业版默认值为12px；
int PersonalizationWork::sizeToSliderValue(const double value) const
{
    int px = static_cast<int>(ptToPx(value));

    if (px < FontSizeList.first()) {
        return 0;
    } else if (px > FontSizeList.last()){
        return (FontSizeList.size() - 1);
    }

    return FontSizeList.indexOf(px);
}

double PersonalizationWork::sliderValueToSize(const int value) const
{
    return pxToPt(FontSizeList.at(value));
}
#else
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

double PersonalizationWork::sliderValueToSize(const int value) const
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
#endif

double PersonalizationWork::sliderValutToOpacity(const int value) const
{
#ifdef WINDOW_MODE
    return static_cast<double>(value) / static_cast<double>(100);
#else
    return static_cast<double>(OPACITY_SLIDER[value]) / static_cast<double>(100);
#endif
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
    //check is allowed to switch wm
    bool allow = allowSwitchWM();
    if (!allow)
        return;

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_wmSwitcher->RequestSwitchWM(), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (watcher->isError()) {
            qDebug() << watcher->error();
        }
        watcher->deleteLater();
    });
}

void PersonalizationWork::windowSwitchWM(bool value)
{
    QDBusInterface Interface("com.deepin.wm",
                                 "/com/deepin/wm",
                                "org.freedesktop.DBus.Properties",
                                QDBusConnection::sessionBus());

     QDBusMessage reply = Interface.call("Set","com.deepin.wm","compositingEnabled", QVariant::fromValue(QDBusVariant(value)));
     if (reply.type() == QDBusMessage::ErrorMessage) {
         qDebug() << "reply.type() = " << reply.type();
     }

}

void PersonalizationWork::setOpacity(int opacity)
{
    m_dbus->setOpacity(sliderValutToOpacity(opacity));
}

void PersonalizationWork::setMiniEffect(int effect)
{
    switch(effect){
    case 0:
        qDebug() << "scale";
        m_effects->unloadEffect("magiclamp");
        m_setting->set(GSETTING_EFFECT_LOAD, false);
        m_model->setMiniEffect(effect);
        break;
    case 1:
        qDebug() << "magiclamp";
        m_effects->loadEffect("magiclamp");
        m_setting->set(GSETTING_EFFECT_LOAD, true);
        m_model->setMiniEffect(effect);
        break;
    default:break;
    }
}

void PersonalizationWork::setActiveColor(const QString &hexColor)
{
    m_dbus->setQtActiveColor(hexColor);
}

void PersonalizationWork::setWindowRadius(int radius)
{
    QDBusInterface interface(Service, Path, Service, QDBusConnection::sessionBus());
    interface.setProperty("WindowRadius", radius);
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
