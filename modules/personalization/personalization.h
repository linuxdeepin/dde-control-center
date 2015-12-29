#ifndef PERSONALIZATION_H
#define PERSONALIZATION_H

#include <QObject>
#include <QtPlugin>
#include <QThread>
#include <QMargins>
#include <QJsonObject>
#include <QScrollArea>
#include "interfaces.h"
#include <libdui/libdui_global.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dexpandgroup.h>
#include <libdui/dheaderline.h>
#include <libdui/dslider.h>
#include <libdui/dcombobox.h>
#include <libdui/dsimplecombobox.h>
#include <libdui/dlabel.h>
#include <libdui/dfontcombobox.h>


#include "persionalizationfwd.h"

DUI_USE_NAMESPACE

class QFrame;
class DBusWorker;

class Personalization: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "personalization.json")
    Q_INTERFACES(ModuleInterface)

public:
    Personalization();
    ~Personalization() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

    void initUI();
    void initControllers();
    void initConnect();

    void initThemeExpand();
    DArrowLineExpand* getThemeExpand();

    void initWindowExpand();
    DArrowLineExpand* getWindowExpand();

    void initIconExpand();
    DArrowLineExpand* getIconExpand();

    void initCursorExpand();
    DArrowLineExpand* getCursorExpand();

    void initWallPaperExpand();
    DArrowLineExpand* getWallPaperExpand();

    void initFontExpand();
    DArrowLineExpand* getFontExpand();

    int getValidKeyIndex(const ImageInfoList& infoList, const QString& key) const;

signals:
    void dataRequested();
    void itemDeleted(QString type, QString id);

public slots:
    void updateThemeKeys(const QStringList& themeKeys);
    void updateWindowKeys(const QStringList& windowKeys);
    void updateIconKeys(const QStringList& iconKeys);
    void updateCursorKeys(const QStringList& cursorKeys);
    void updateBackgroundKeys(const QStringList& backgroundKeys);

    void updateThemeObjs(const JosnMapObjs& themeObjs);
    void updateThemeButtons(const ImageInfoList& imageInfos);
    void updateWindowButtons(const ImageInfoList& imageInfos);
    void updateIconButtons(const ImageInfoList& imageInfos);
    void updateCursorButtons(const ImageInfoList& imageInfos);
    void updateWallpaperButtons(const ImageInfoList& imageInfos);
    void updateStandardFontCombox(const QStringList& standardFonts);
    void updateMonospaceFontCombox(const QStringList& monospaceFonts);

    void handleDataFinished();
    void updateCurrentTheme(QString themeKey);
    void highlightCurrentTheme(QString themeKey);

    void setThemeByIndex(int index);
    void setWindowByIndex(int index);
    void setIconByIndex(int index);
    void setCursorByIndex(int index);
    void setBackgroundByIndex(int index);
    void setStandardFontByIndex(int index);
    void setMonospaceFontByIndex(int index);
    void setFontLabel(int fontSize);
    void setFontSize(int fontSize);

    void handleDataDeleteRefrehed(QString id);

private:
    QMargins m_margins;
    ImageInfoList m_themeImageInfos;
    ImageInfoList m_windowImageInfos;
    ImageInfoList m_iconImageInfos;
    ImageInfoList m_cursorImageInfos;
    ImageInfoList m_wallpaperImageInfos;

    QFrame * m_frame = NULL;

    DHeaderLine* m_headerLine;

    DArrowLineExpand* m_themeExpand = NULL;
    DButtonGrid* m_themeButtonGrid = NULL;
    QFrame* m_themeContentFrame = NULL;

    DArrowLineExpand* m_windowExpand = NULL;
    DButtonGrid* m_windowButtonGrid = NULL;
    QFrame* m_windowContentFrame = NULL;

    DArrowLineExpand* m_iconExpand = NULL;
    DButtonGrid* m_iconButtonGrid = NULL;
    QScrollArea* m_iconContentArea = NULL;

    DArrowLineExpand* m_cursorExpand = NULL;
    DButtonGrid* m_cursorButtonGrid = NULL;
    QScrollArea* m_cursorContentArea = NULL;

    DArrowLineExpand* m_wallpaperExpand = NULL;
    DButtonGrid* m_wallpaperButtonGrid = NULL;
    QFrame* m_wallpaperContentFrame = NULL;

    DArrowLineExpand* m_fontExpand = NULL;
    QFrame* m_fontContentFrame = NULL;

    DFontComboBox* m_standardFontCombox;
    DFontComboBox* m_monospaceFontCombox;
    DSlider* m_slider;
    DLabel* m_fontTipLabel;

    DExpandGroup* m_expandGroup = NULL;


    DBusWorker* m_dbusWorker = NULL;
    QThread m_workerThread;

    QList<DButtonGrid*> m_buttonGrids;
    QList<QFrame*> m_contentFrames;

    QStringList m_themeKeys{};
    QStringList m_windowKeys{};
    QStringList m_iconKeys{};
    QStringList m_cursorKeys{};
    QStringList m_backgroundKeys{};
    QStringList m_standardFonts{};
    QStringList m_monospaceFonts{};
    QString m_currentTheme;
    JosnMapObjs m_themeObjs;


    int m_itemWidth = 140;
    int m_itemHeight = 90;
    int m_maxExpandContentHeight = 0;
};

#endif // PERSONALIZATION_H
