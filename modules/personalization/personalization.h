#ifndef PERSONALIZATION_H
#define PERSONALIZATION_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"
#include <libdui/libdui_global.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dexpandgroup.h>

DUI_USE_NAMESPACE

class QFrame;

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

public slots:
    void updateThemeButtons(const QList<QMap<QString, QString>>& imageInfos);
    void updateWindowButtons(const QList<QMap<QString, QString>>& imageInfos);
    void updateIconButtons(const QList<QMap<QString, QString>>& imageInfos);
    void updateCursorButtons(const QList<QMap<QString, QString>>& imageInfos);
    void updateWallpaperButtons(const QList<QMap<QString, QString>>& imageInfos);


private:
    QList<QMap<QString, QString>> m_themeImageInfos;
    QList<QMap<QString, QString>> m_windowImageInfos;
    QList<QMap<QString, QString>> m_iconImageInfos;
    QList<QMap<QString, QString>> m_cursorImageInfos;
    QList<QMap<QString, QString>> m_wallpaperImageInfos;

    QFrame * m_frame = NULL;
    DArrowLineExpand* m_themeExpand = NULL;
    DButtonGrid* m_themeButtonGrid = NULL;
    QFrame* m_themeContentFrame = NULL;

    DArrowLineExpand* m_windowExpand = NULL;
    DButtonGrid* m_windowButtonGrid = NULL;
    QFrame* m_windowContentFrame = NULL;

    DArrowLineExpand* m_iconExpand = NULL;
    DButtonGrid* m_iconButtonGrid = NULL;
    QFrame* m_iconContentFrame = NULL;

    DArrowLineExpand* m_cursorExpand = NULL;
    DButtonGrid* m_cursorButtonGrid = NULL;
    QFrame* m_cursorContentFrame = NULL;

    DArrowLineExpand* m_wallpaperExpand = NULL;
    DButtonGrid* m_wallpaperButtonGrid = NULL;
    QFrame* m_wallpaperContentFrame = NULL;

    DArrowLineExpand* m_fontExpand = NULL;

    DExpandGroup* m_expandGroup = NULL;

    int m_itemWidth = 140;
    int m_itemHeight = 90;
};

#endif // PERSONALIZATION_H
