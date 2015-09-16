#include "dbusworker.h"
#include "dbus/appearancedaemon_interface.h"
#include <QStringList>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QDebug>

DBusWorker::DBusWorker(QObject *parent) : QObject(parent)
{
    m_appearanceDaemonInterface = new AppearanceDaemonInterface(staticServiceName(),
                                                                staticObjectPathName(),
                                                                QDBusConnection::sessionBus(),
                                                                this);

    connect(m_appearanceDaemonInterface, &AppearanceDaemonInterface::Changed,
            m_appearanceDaemonInterface, [this](const QString &str1, const QString &){
        if(str1 != "background")
            return;

        m_themeDetails.clear();
        getDetails(staticTypeKeys.value("TypeDTheme"), m_themeKeys, m_themeDetails);
        emit themeDetailsChanged(m_themeDetails);
    });
}

void DBusWorker::doWork(){
    getKeys(staticTypeKeys.value("TypeDTheme"), m_themeKeys);
    getKeys(staticTypeKeys.value("TypeGtkTheme"), m_windowKeys);
    getKeys(staticTypeKeys.value("TypeIconTheme"), m_iconKeys);
    getKeys(staticTypeKeys.value("TypeCursorTheme"), m_cursorKeys);
    getKeys(staticTypeKeys.value("TypeBackground"), m_backgroundKeys);
    getKeys(staticTypeKeys.value("TypeStandardFont"), m_standardFontKeys);
    getKeys(staticTypeKeys.value("TypeMonospaceFont"), m_monospaceFontKeys);
    getThemeObjs();

    getDetails(staticTypeKeys.value("TypeDTheme"), m_themeKeys, m_themeDetails);
    getDetails(staticTypeKeys.value("TypeGtkTheme"), m_windowKeys, m_windowDetails);
    getDetails(staticTypeKeys.value("TypeIconTheme"), m_iconKeys, m_iconDetails);
    getDetails(staticTypeKeys.value("TypeCursorTheme"), m_cursorKeys, m_cursorDetails);
    getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundDetails);

    getFontSize();
    getCurrentTheme();

    emit themeKeysChanged(m_themeKeys);
    emit windowKeysChanged(m_windowKeys);
    emit iconKeysChanged(m_iconKeys);
    emit cursorKeysChanged(m_cursorKeys);
    emit backgroundKeysChanged(m_backgroundKeys);

    emit themeObjsChanged(m_themeObjs);
    emit themeDetailsChanged(m_themeDetails);
    emit windowDetailsChanged(m_windowDetails);
    emit iconDetailsChanged(m_iconDetails);
    emit cursorDetailsChanged(m_cursorDetails);
    emit backgroundDetailsChanged(m_backgroundDetails);
    emit standardFontDetailsChanged(m_standardFontKeys);
    emit monospaceFontDetailsChanged(m_monospaceFontKeys);
    emit currentThemeChanged(m_currentThemeKey);
    emit fontSizeChanged(m_fontSize);
    emit dataFinished();
}

QStringList& DBusWorker::getKeys(QString Type, QStringList &keys){
    QDBusPendingReply<QStringList> reply = m_appearanceDaemonInterface->List(Type);
    reply.waitForFinished();
    if (!reply.isError()){
        keys = reply.argumentAt(0).toStringList();
    }else{
        qDebug() << reply.error().message();
    }
    return keys;
}

QString DBusWorker::getThumbnail(QString Type, QString key){
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->Thumbnail(Type, key);
    reply.waitForFinished();
    QString thumbnail;
    if (!reply.isError()){
        thumbnail = reply.argumentAt(0).toString();
    }else{
        qDebug() << reply.error().message();
    }
    return thumbnail;
}

ImageInfoList& DBusWorker::getDetails(QString Type, QStringList &keys, ImageInfoList &details){
    if (keys.length() == 0){
        keys = getKeys(Type, keys);
    }
    foreach (QString key, keys) {
        QString thumbnail = getThumbnail(Type, key);
        if (thumbnail.length() > 0){
            QString name = key;
            if (Type == staticTypeKeys.value("TypeDTheme")){
                name = m_themeObjs.value(key).value("Name").toString();
            }
            QMap<QString, QString> detail{
                {"key", key},
                {"name", name},
                {"url", thumbnail}
            };
            details.append(detail);
        }
    }
    return details;
}


int DBusWorker::getFontSize(){
    m_fontSize = m_appearanceDaemonInterface->fontSize();
    return m_fontSize;
}

QString DBusWorker::getCurrentTheme(){
    m_currentThemeKey = m_appearanceDaemonInterface->theme();
    return m_currentThemeKey;
}

QJsonObject DBusWorker::getThemeByKey(QString theme){
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->Show(staticTypeKeys.value("TypeDTheme"),                                                           theme);
    reply.waitForFinished();
    QJsonObject obj;
    if (!reply.isError()){
        QString  themeDetail = reply.argumentAt(0).toString();
        obj = QJsonDocument::fromJson(themeDetail.toStdString().c_str()).object();
    }else{
        qDebug() << reply.error().message();
    }
    return obj;
}

void DBusWorker::getThemeObjs(){
    foreach (QString key, m_themeKeys) {
        QJsonObject obj = getThemeByKey(key);
        m_themeObjs.insert(key, obj);
    }
}

void DBusWorker::setTheme(QString Type, QString Key){
    QDBusPendingReply<> reply = m_appearanceDaemonInterface->Set(Type, Key);
    qDebug() << "set" << Type << Key;
}

DBusWorker::~DBusWorker()
{

}
