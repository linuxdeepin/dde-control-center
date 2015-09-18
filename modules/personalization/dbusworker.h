#ifndef DBUSWORKER_H
#define DBUSWORKER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include "persionalizationfwd.h"

class AppearanceDaemonInterface;

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
    QStringList& getKeys(QString Type, QStringList& keys);
    QString getThumbnail(QString Type, QString key);
    ImageInfoList& getDetails(QString Type, QStringList& keys, ImageInfoList& details);

    int getFontSize();
    QString getCurrentTheme();
    QJsonObject getThemeByKey(QString theme);
    void getThemeObjs();
    QStringList getPreviewImages(const QString &themeName) const;

signals:
    void themeKeysChanged(const QStringList& themeKeys);
    void windowKeysChanged(const QStringList& windowKeys);
    void iconKeysChanged(const QStringList& iconKeys);
    void cursorKeysChanged(const QStringList& cursorKeys);
    void backgroundKeysChanged(const QStringList& backgroundKeys);

    void themeObjsChanged(const ThemeObjs& obj);
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

public slots:
    void doWork();
    void setTheme(QString Type, QString Key);

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
    ThemeObjs m_themeObjs;
    AppearanceDaemonInterface* m_appearanceDaemonInterface;
};

#endif // DBUSWORKER_H
