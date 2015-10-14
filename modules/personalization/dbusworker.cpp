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
        getDetails(staticTypeKeys.value("TypeDTheme"), m_themeKeys, m_themeObjs, m_themeDetails);
        emit themeDetailsChanged(m_themeDetails);
    });
}

void DBusWorker::doWork(){
    getDetails(staticTypeKeys.value("TypeDTheme"), m_themeKeys ,m_themeObjs , m_themeDetails);
    emit themeKeysChanged(m_themeKeys);
    emit themeObjsChanged(m_themeObjs);
    emit themeDetailsChanged(m_themeDetails);

    getDetails(staticTypeKeys.value("TypeGtkTheme"), m_windowKeys, m_windowObjs, m_windowDetails);
    getDetails(staticTypeKeys.value("TypeIconTheme"), m_iconKeys, m_iconObjs, m_iconDetails);
    getDetails(staticTypeKeys.value("TypeCursorTheme"), m_cursorKeys, m_cursorObjs, m_cursorDetails);
    getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundObjs, m_backgroundDetails);
    getDetails(staticTypeKeys.value("TypeStandardFont"), m_standardFontKeys, m_standardFontObjs);
    getDetails(staticTypeKeys.value("TypeMonospaceFont"), m_monospaceFontKeys, m_monospaceFontObjs);

    getFontSize();
    getCurrentTheme();

    emit themeKeysChanged(m_themeKeys);
    emit windowKeysChanged(m_windowKeys);
    emit iconKeysChanged(m_iconKeys);
    emit cursorKeysChanged(m_cursorKeys);
    emit backgroundKeysChanged(m_backgroundKeys);

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

void DBusWorker::getDetails(QString Type, QStringList &keys, JosnMapObjs &objs){
    keys.clear();
    objs.clear();
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->List(Type);
    reply.waitForFinished();
    if (!reply.isError()){
        QJsonDocument doc = QJsonDocument::fromJson(reply.argumentAt(0).toString().toLocal8Bit());
        for(int i=0; i< doc.array().count() ; i++){
            QJsonObject obj = doc.array().at(i).toObject();
            QString id = obj.value("Id").toString();
            objs.insert(id, obj);
            keys.append(id);
        }
    }else{
        qDebug() << reply.error().message();
    }
}

ImageInfoList& DBusWorker::getDetails(QString Type, QStringList &keys, JosnMapObjs &objs, ImageInfoList &details){
    getDetails(Type, keys, objs);
    details.clear();
    foreach (QString key, keys) {
        QString thumbnail = getThumbnail(Type, key);
        if (thumbnail.length() > 0){
            QString name = key;
            if (Type == staticTypeKeys.value("TypeDTheme")){
                name = m_themeObjs.value(key).value("Name").toString();
            }
            QString deletable("false");
            if (objs.value(key).contains("Deletable")){
                if (objs.value(key).value("Deletable").toBool()){
                    deletable = "true";
                }
            }
            QMap<QString, QString> detail{
                {"key", key},
                {"name", name},
                {"url", thumbnail},
                {"deletable", deletable}
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
        obj = QJsonDocument::fromJson(themeDetail.toLocal8Bit()).object();
    }else{
        qDebug() << reply.error().message();
    }
    return obj;
}

void DBusWorker::getThemeObjs(){
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->List(staticTypeKeys.value("TypeDTheme"));
    reply.waitForFinished();
    if (!reply.isError()){
        QJsonDocument doc = QJsonDocument::fromJson(reply.argumentAt(0).toString().toLocal8Bit());
        for(int i=0; i< doc.array().count() ; i++){
            QJsonObject obj = doc.array().at(0).toObject();
            QString id = obj.value("Id").toString();
            m_themeObjs.insert(id, obj);
        }
    }else{
        qDebug() << reply.error().message();
    }
}

void DBusWorker::setTheme(QString Type, QString Key){
    QDBusPendingReply<> reply = m_appearanceDaemonInterface->Set(Type, Key);
    qDebug() << "set" << Type << Key;
}

void DBusWorker::deleteItem(QString Type, QString Key){
    QDBusPendingReply<> reply = m_appearanceDaemonInterface->Delete(Type, Key);
    reply.waitForFinished();
    if (!reply.isError()){
        qDebug() << "delete" << Type << Key << "successfully";
        if (Type == staticTypeKeys.value("TypeBackground")){
            getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundObjs, m_backgroundDetails);
            emit backgroundKeysChanged(m_backgroundKeys);
            emit backgroundDetailsChanged(m_backgroundDetails);
        }else if (Type == staticTypeKeys.value("TypeDTheme")){
            getDetails(staticTypeKeys.value("TypeDTheme"), m_themeKeys ,m_themeObjs , m_themeDetails);
            emit themeKeysChanged(m_themeKeys);
            emit themeObjsChanged(m_themeObjs);
            emit themeDetailsChanged(m_themeDetails);
        }
    }else{
        qDebug() << reply.error().message();
    }
}

DBusWorker::~DBusWorker()
{

}
