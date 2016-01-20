#ifndef DBUSWORKER_H
#define DBUSWORKER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include "persionalizationfwd.h"

class AppearanceDaemonInterface;
class QTimer;

class DBusWorker : public QObject
{
    Q_OBJECT
public:
    explicit DBusWorker(QObject *parent = 0);
    ~DBusWorker();

    static inline const char *staticServiceName()
    { return "com.deepin.daemon.Appearance"; }

    static inline const char *staticObjectPathName()
    { return "/com/deepin/daemon/Appearance"; }

    const QMap<QString, QString> staticTypeKeys{
        {"TypeDTheme" ,"dtheme"},
        {"TypeGtkTheme" ,"gtk"},
        {"TypeIconTheme" ,"icon"},
        {"TypeCursorTheme" ,"cursor"},
        {"TypeBackground", "background"},
        {"TypeStandardFont", "standardfont"},
        {"TypeMonospaceFont" ,"monospacefont"},
        {"TypeFontSize","fontsize"}
    };

    void initConnect();
    QString getThumbnail(QString Type, QString key);
    ImageInfoList& getDetails(QString Type, QStringList& keys, JosnMapObjs& objs, ImageInfoList& details);
    void getDetails(QString Type, QStringList& keys, JosnMapObjs& objs);
    int getFontSize();
    QString getCurrentTheme();
    QJsonObject getThemeByKey(QString theme);
    void getThemeObjs();

signals:
    void themeKeysChanged(const QStringList& themeKeys);
    void windowKeysChanged(const QStringList& windowKeys);
    void iconKeysChanged(const QStringList& iconKeys);
    void cursorKeysChanged(const QStringList& cursorKeys);
    void backgroundKeysChanged(const QStringList& backgroundKeys);

    void themeObjsChanged(const JosnMapObjs& obj);
    void themeDetailsChanged(const ImageInfoList& details);
    void windowDetailsChanged(const ImageInfoList& details);
    void iconDetailsChanged(const ImageInfoList& details);
    void cursorDetailsChanged(const ImageInfoList& details);
    void backgroundDetailsChanged(const ImageInfoList& details);
    void standardFontDetailsChanged(const QStringList& details);
    void monospaceFontDetailsChanged(const QStringList& details);
    void fontSizeChanged(int size);
    void dataFinished();
    void currentThemeChanged(QString themeKey);
    void currentThemeHightlighted(QString themeKey);

public slots:
    void doWork();
    void delayGetData();
    void setTheme(QString Type, QString Key);
    void deleteItem(QString Type, QString Key);

private:
    QStringList m_themeKeys{};
    ImageInfoList m_themeDetails{};
    QStringList m_windowKeys{};
    ImageInfoList m_windowDetails{};
    QStringList m_iconKeys{};
    ImageInfoList m_iconDetails{};
    QStringList m_cursorKeys{};
    ImageInfoList m_cursorDetails{};
    QStringList m_backgroundKeys{};
    ImageInfoList m_backgroundDetails{};
    QStringList m_standardFontKeys{};
    QStringList m_monospaceFontKeys{};
    int m_fontSize = 10;
    QString m_currentThemeKey;
    JosnMapObjs m_themeObjs;
    JosnMapObjs m_windowObjs;
    JosnMapObjs m_iconObjs;
    JosnMapObjs m_cursorObjs;
    JosnMapObjs m_backgroundObjs;
    JosnMapObjs m_standardFontObjs;
    JosnMapObjs m_monospaceFontObjs;
    AppearanceDaemonInterface* m_appearanceDaemonInterface;

    QTimer *m_setThemeTimer = nullptr;
    QString m_setThemeType;
    QString m_setThemeKey;
};

#endif // DBUSWORKER_H
