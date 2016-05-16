/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PERSONALIZATION_H
#define PERSONALIZATION_H

#include <QObject>
#include <QtPlugin>
#include <QThread>
#include <QMargins>
#include <QJsonObject>
#include <QScrollArea>
#include "interfaces.h"
#include <dwidget_global.h>
#include <darrowlineexpand.h>
#include <dbuttongrid.h>
#include <dexpandgroup.h>
#include <dheaderline.h>
#include <dslider.h>
#include <dcombobox.h>
#include <dsimplecombobox.h>
#include <dlabel.h>
#include <dfontcombobox.h>


#include "persionalizationfwd.h"

DWIDGET_USE_NAMESPACE

class QFrame;
class DBusWorker;

class Personalization: public QObject
{
    Q_OBJECT
public:
    Personalization(QObject *parent = NULL);
    ~Personalization();
    QFrame *getContent();

    void initUI();
    void initControllers();
    void initConnect();

    void initWindowExpand();
    DArrowLineExpand *getWindowExpand();

    void initIconExpand();
    DArrowLineExpand *getIconExpand();

    void initCursorExpand();
    DArrowLineExpand *getCursorExpand();

    void initWallPaperExpand();
    DArrowLineExpand *getWallPaperExpand();

    void initFontExpand();
    DArrowLineExpand *getFontExpand();

    int getValidKeyIndex(const ImageInfoList &infoList, const QString &key) const;

signals:
    void dataRequested();
    void itemDeleted(QString type, QString id);

public slots:
    void updateWindow(const QString &window);
    void updateIcon(const QString &icon);
    void updateCursor(const QString &window);
    void updateWallpaper(const QString &window);
    void updateStandardFont(const QString &window);
    void updateMonospaceFont(const QString &window);

    void updateWindowKeys(const QStringList &windowKeys);
    void updateIconKeys(const QStringList &iconKeys);
    void updateCursorKeys(const QStringList &cursorKeys);
    void updateBackgroundKeys(const QStringList &backgroundKeys);

    void updateWindowButtons(const ImageInfoList &imageInfos);
    void updateIconButtons(const ImageInfoList &imageInfos);
    void updateCursorButtons(const ImageInfoList &imageInfos);
    void updateWallpaperButtons(const ImageInfoList &imageInfos);
    void updateStandardFontCombox(const QStringList &standardFonts);
    void updateMonospaceFontCombox(const QStringList &monospaceFonts);

    void handleDataFinished();

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
    ImageInfoList m_windowImageInfos;
    ImageInfoList m_iconImageInfos;
    ImageInfoList m_cursorImageInfos;
    ImageInfoList m_wallpaperImageInfos;

    QFrame *m_frame = NULL;

    DHeaderLine *m_headerLine;

    DArrowLineExpand *m_windowExpand = NULL;
    DButtonGrid *m_windowButtonGrid = NULL;
    QFrame *m_windowContentFrame = NULL;

    DArrowLineExpand *m_iconExpand = NULL;
    DButtonGrid *m_iconButtonGrid = NULL;
    QScrollArea *m_iconContentArea = NULL;

    DArrowLineExpand *m_cursorExpand = NULL;
    DButtonGrid *m_cursorButtonGrid = NULL;
    QScrollArea *m_cursorContentArea = NULL;

    DArrowLineExpand *m_wallpaperExpand = NULL;
    DButtonGrid *m_wallpaperButtonGrid = NULL;
    QFrame *m_wallpaperContentFrame = NULL;

    DArrowLineExpand *m_fontExpand = NULL;
    QFrame *m_fontContentFrame = NULL;

    DFontComboBox *m_standardFontCombox;
    DFontComboBox *m_monospaceFontCombox;
    DSlider *m_slider;
    DLabel *m_fontTipLabel;

    DExpandGroup *m_expandGroup = NULL;


    DBusWorker *m_dbusWorker = NULL;
    QThread *m_workerThread;

    QList<DButtonGrid *> m_buttonGrids;
    QList<QFrame *> m_contentFrames;

    QStringList m_windowKeys {};
    QStringList m_iconKeys {};
    QStringList m_cursorKeys {};
    QStringList m_backgroundKeys {};
    QStringList m_standardFonts {};
    QStringList m_monospaceFonts {};
    QString m_currentTheme;
    JosnMapObjs m_themeObjs;


    int m_itemWidth = 140;
    int m_itemHeight = 90;
    int m_maxExpandContentHeight = 0;
};

class PersonalizationModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "personalization.json")
    Q_INTERFACES(ModuleInterface)

public:
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    Personalization *m_personalization = NULL;
};

#endif // PERSONALIZATION_H
