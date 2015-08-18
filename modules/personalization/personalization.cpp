#include "personalization.h"
#include <libdui/dheaderline.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dseparatorhorizontal.h>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

DUI_USE_NAMESPACE

Personalization::Personalization()
{
    for(int i= 0; i<2; i++){
        QMap<QString, QString> imageInfo;
        imageInfo.insert("key", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("name", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("url", "/usr/share/personalization/themes/Deepin/wallpapers/1.jpg");
        m_themeImageInfos.append(imageInfo);
    }

    for(int i= 0; i<2; i++){
        QMap<QString, QString> imageInfo;
        imageInfo.insert("key", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("name", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("url", "/usr/share/personalization/themes/Deepin/wallpapers/1.jpg");
        m_windowImageInfos.append(imageInfo);
    }

    for(int i= 0; i<5; i++){
        QMap<QString, QString> imageInfo;
        imageInfo.insert("key", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("name", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("url", "/usr/share/personalization/themes/Deepin/wallpapers/1.jpg");
        m_iconImageInfos.append(imageInfo);
    }

    for(int i= 0; i<4; i++){
        QMap<QString, QString> imageInfo;
        imageInfo.insert("key", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("name", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("url", "/usr/share/personalization/themes/Deepin/wallpapers/1.jpg");
        m_cursorImageInfos.append(imageInfo);
    }

    for(int i= 0; i<14; i++){
        QMap<QString, QString> imageInfo;
        imageInfo.insert("key", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("name", QString("image%1").arg(QString::number(i)));
        imageInfo.insert("url", "/usr/share/personalization/themes/Deepin/wallpapers/1.jpg");
        m_wallpaperImageInfos.append(imageInfo);
    }

    initUI();
    QTimer::singleShot(500, [=](){
        updateThemeButtons(m_themeImageInfos);
        updateWindowButtons(m_windowImageInfos);
        updateIconButtons(m_iconImageInfos);
        updateCursorButtons(m_cursorImageInfos);
        updateWallpaperButtons(m_wallpaperImageInfos);
    });
}

void Personalization::initUI(){
    m_frame = new QFrame();
    m_expandGroup = new DExpandGroup;
    DHeaderLine* titleHeader = new DHeaderLine();
    titleHeader->setStyleSheet("font-size: 16px;color:white");
    titleHeader->setFixedHeight(50);
    titleHeader->setTitle(tr("Personalization"));

    DSeparatorHorizontal* horizontalSeparator = new DSeparatorHorizontal();
    initThemeExpand();
    initWindowExpand();
    initIconExpand();
    initCursorExpand();
    initWallPaperExpand();
    initFontExpand();

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleHeader);
    mainLayout->addWidget(horizontalSeparator);
    mainLayout->addWidget(m_themeExpand);
    mainLayout->addWidget(m_windowExpand);
    mainLayout->addWidget(m_iconExpand);
    mainLayout->addWidget(m_cursorExpand);
    mainLayout->addWidget(m_wallpaperExpand);
    mainLayout->addWidget(m_fontExpand);
    mainLayout->addStretch(1);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    m_frame->setLayout(mainLayout);

    m_expandGroup->addExpand(m_themeExpand);
    m_expandGroup->addExpand(m_windowExpand);
    m_expandGroup->addExpand(m_iconExpand);
    m_expandGroup->addExpand(m_cursorExpand);
    m_expandGroup->addExpand(m_wallpaperExpand);
    m_expandGroup->addExpand(m_fontExpand);
}

void Personalization::initThemeExpand(){
    m_themeExpand = new DArrowLineExpand(m_frame);
    m_themeExpand->setTitle(tr("Theme"));
    m_themeButtonGrid = new DButtonGrid(1, 2);
    m_themeButtonGrid->setItemSize(m_itemWidth, m_itemHeight + 20);

    m_themeContentFrame = new QFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_themeButtonGrid, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 5, 0, 5);
    m_themeContentFrame->setLayout(mainLayout);
}

DArrowLineExpand* Personalization::getThemeExpand(){
    return m_themeExpand;
}

void Personalization::initWindowExpand(){
    m_windowExpand = new DArrowLineExpand(m_frame);
    m_windowExpand->setTitle(tr("Window"));

    m_windowButtonGrid = new DButtonGrid(1, 2);
    m_windowButtonGrid->setItemSize(m_itemWidth, m_itemHeight + 20);

    m_windowContentFrame = new QFrame;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_windowButtonGrid, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 5, 0, 5);
    m_windowContentFrame->setLayout(mainLayout);

}

DArrowLineExpand* Personalization::getWindowExpand(){
    return m_windowExpand;
}

void Personalization::initIconExpand(){
    m_iconExpand = new DArrowLineExpand(m_frame);
    m_iconExpand->setTitle(tr("Icon"));

    m_iconButtonGrid = new DButtonGrid(3, 2);
    m_iconButtonGrid->setItemSize(m_itemWidth, m_itemHeight + 20);

    m_iconContentFrame = new QFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_iconButtonGrid, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 5, 0, 5);
    m_iconContentFrame->setLayout(mainLayout);

}

DArrowLineExpand* Personalization::getIconExpand(){
    return m_iconExpand;
}

void Personalization::initCursorExpand(){
    m_cursorExpand = new DArrowLineExpand(m_frame);
    m_cursorExpand->setTitle(tr("Cursor"));

    m_cursorButtonGrid = new DButtonGrid(2, 2);
    m_cursorButtonGrid->setItemSize(m_itemWidth, m_itemHeight + 20);

    m_cursorContentFrame = new QFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_cursorButtonGrid, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 5, 0, 5);
    m_cursorContentFrame->setLayout(mainLayout);
}

DArrowLineExpand* Personalization::getCursorExpand(){
    return m_cursorExpand;
}

void Personalization::initWallPaperExpand(){
    m_wallpaperExpand = new DArrowLineExpand(m_frame);
    m_wallpaperExpand->setTitle(tr("Wallpaper"));

    m_wallpaperButtonGrid = new DButtonGrid(7, 2);
    m_wallpaperButtonGrid->setItemSize(m_itemWidth, m_itemHeight);

    m_wallpaperContentFrame = new QFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_wallpaperButtonGrid, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 5, 0, 5);
    m_wallpaperContentFrame->setLayout(mainLayout);
}

DArrowLineExpand* Personalization::getWallPaperExpand(){
    return m_cursorExpand;
}


void Personalization::initFontExpand(){
    m_fontExpand = new DArrowLineExpand(m_frame);
    m_fontExpand->setTitle(tr("Font"));
    QLabel *expandLabel2 = new QLabel("Arrow Expand content");
    expandLabel2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    expandLabel2->setFixedSize(200,50);
    m_fontExpand->setContent(expandLabel2);
    m_fontExpand->setExpand(false);
}

DArrowLineExpand* Personalization::getFontExpand(){
    return m_fontExpand;
}

void Personalization::updateThemeButtons(const QList<QMap<QString, QString>> &imageInfos){
    m_themeButtonGrid->addImageButtons(imageInfos);
    QMargins m = m_themeContentFrame->layout()->contentsMargins();
    int w = m_themeButtonGrid->width() + m.left() + m.right();
    int h = m_themeButtonGrid->height() + m.top() + m.bottom();
    m_themeContentFrame->setFixedSize(w, h);
    m_themeExpand->setContent(m_themeContentFrame);
    m_themeExpand->setExpand(true);
}

void Personalization::updateWindowButtons(const QList<QMap<QString, QString>> &imageInfos){
    m_windowButtonGrid->addImageButtons(imageInfos);
    QMargins m = m_windowContentFrame->layout()->contentsMargins();
    int w = m_windowButtonGrid->width() + m.left() + m.right();
    int h = m_windowButtonGrid->height() + m.top() + m.bottom();
    m_windowContentFrame->setFixedSize(w, h);
    m_windowExpand->setContent(m_windowContentFrame);
//    m_windowExpand->setExpand(false);
}

void Personalization::updateIconButtons(const QList<QMap<QString, QString>> &imageInfos){
    m_iconButtonGrid->addImageButtons(imageInfos);
    QMargins m = m_iconContentFrame->layout()->contentsMargins();
    int w = m_iconButtonGrid->width() + m.left() + m.right();
    int h = m_iconButtonGrid->height() + m.top() + m.bottom();
    m_iconContentFrame->setFixedSize(w, h);
    m_iconExpand->setContent(m_iconContentFrame);
//    m_iconExpand->setExpand(false);
}

void Personalization::updateCursorButtons(const QList<QMap<QString, QString>> &imageInfos){
    m_cursorButtonGrid->addImageButtons(imageInfos);
    QMargins m = m_cursorContentFrame->layout()->contentsMargins();
    int w = m_cursorButtonGrid->width() + m.left() + m.right();
    int h = m_cursorButtonGrid->height() + m.top() + m.bottom();
    m_cursorContentFrame->setFixedSize(w, h);
    m_cursorExpand->setContent(m_cursorContentFrame);
//    m_cursorExpand->setExpand(false);
}

void Personalization::updateWallpaperButtons(const QList<QMap<QString, QString>> &imageInfos){
    m_wallpaperButtonGrid->addImageButtons(imageInfos, false);
    QMargins m = m_wallpaperContentFrame->layout()->contentsMargins();
    int w = m_wallpaperButtonGrid->width() + m.left() + m.right();
    int h = m_wallpaperButtonGrid->height() + m.top() + m.bottom();
    m_wallpaperContentFrame->setFixedSize(w, h);
    m_wallpaperExpand->setContent(m_wallpaperContentFrame);
//    m_wallpaperExpand->setExpand(false);
}


Personalization::~Personalization()
{
    qDebug() << "~Personalization()";

    m_frame->deleteLater();
}

QFrame* Personalization::getContent()
{
    return m_frame;
}
