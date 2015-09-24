#include <QDebug>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProcess>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dimagebutton.h>

#include "homescreen.h"
#include "constants.h"

#include "useravatar.h"

HomeScreen::HomeScreen(QList<ModuleMetaData> modules, QWidget *parent) :
    QFrame(parent),
    m_dbusAccounts(new DBusAccounts(this)),
    modules(modules)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    // get user info d-bus interface
    m_dbusUserInfo = new DBusUser(m_dbusAccounts->userList().first(), this);

    m_grid = new QGridLayout;
    m_grid->setContentsMargins(1, 0, 1, 0);
    m_grid->setSpacing(2);
    foreach(ModuleMetaData meta, modules) {
        ModuleButton *button = new ModuleButton(meta, this);

        m_grid->addWidget(button, m_moduleCount / 3, m_moduleCount % 3);
        ++m_moduleCount;

        connect(button, &ModuleButton::clicked, this, &HomeScreen::buttonClicked);
    }

    QVBoxLayout *centerVLayout = new QVBoxLayout;
    QSpacerItem *vSpace = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    centerVLayout->addLayout(m_grid);
    centerVLayout->addSpacerItem(vSpace);
    centerVLayout->setSpacing(0);
    centerVLayout->setMargin(0);

    m_centerWidget = new QWidget(this);
    m_centerWidget->setLayout(centerVLayout);

    QWidget *topOuterWidget = new QWidget(this);
    topOuterWidget->setFixedHeight(DCC::HomeScreen_TopWidgetHeight);
    topOuterWidget->setFixedWidth(DCC::ControlCenterWidth);

    UserAvatar *topButton = new UserAvatar;
    topButton->setIcon(m_dbusUserInfo->iconFile());
    topButton->setFixedSize(80, 80);
    topButton->setAvatarSize(UserAvatar::AvatarLargeSize);

    QLabel *topLabel = new QLabel;
    topLabel->setFixedHeight(30);
    topLabel->setAlignment(Qt::AlignCenter);
    topLabel->setObjectName("Username");
    topLabel->setText(m_dbusUserInfo->userName());

    QVBoxLayout *topWidgetLayout = new QVBoxLayout;
    topWidgetLayout->addWidget(topButton);
    topWidgetLayout->setAlignment(topButton, Qt::AlignCenter);
    topWidgetLayout->addWidget(topLabel);
    topWidgetLayout->addStretch();
    topWidgetLayout->setSpacing(20);
    topWidgetLayout->setContentsMargins(0, 20, 0, 15);

    QWidget *topWidget = new QWidget;
    topWidget->setLayout(topWidgetLayout);
    topWidget->setObjectName("TopWidget");

    QVBoxLayout *topVBox = new QVBoxLayout;
    topVBox->addWidget(topWidget);
    topVBox->addWidget(new DSeparatorHorizontal);
    topVBox->setSpacing(0);
    topVBox->setMargin(0);

    m_topWidget = new QWidget(topOuterWidget);
    m_topWidget->setFixedSize(topOuterWidget->size());
    m_topWidget->setLayout(topVBox);

    DImageButton *bottomButton = new DImageButton(DCC::IconPath + "power-button-normal.png",
                                                  DCC::IconPath + "power-button-hover.png",
                                                  DCC::IconPath + "power-button-press.png");
    bottomButton->setAttribute(Qt::WA_TranslucentBackground);

    QLabel *bottomLabel = new QLabel(tr("Power"));
    bottomLabel->setObjectName("PowerLabel");
    bottomLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *bottomVLayout = new QVBoxLayout;
    bottomVLayout->addWidget(bottomButton);
    bottomVLayout->addWidget(bottomLabel);
    bottomVLayout->setSpacing(0);
    bottomVLayout->setContentsMargins(0, 0, 0, 10);

    QHBoxLayout *bottomHLayout = new QHBoxLayout;
    bottomHLayout->addStretch();
    bottomHLayout->addLayout(bottomVLayout);
    bottomHLayout->addStretch();
    bottomHLayout->setSpacing(0);
    bottomHLayout->setMargin(0);

    QWidget *bottomOuterWidget = new QWidget(this);
    bottomOuterWidget->setFixedHeight(DCC::HomeScreen_BottomWidgetHeight);
    bottomOuterWidget->setFixedWidth(DCC::ControlCenterWidth);

    m_bottomWidget = new QWidget(bottomOuterWidget);
    m_bottomWidget->setObjectName("BottomWidget");
    m_bottomWidget->setLayout(bottomHLayout);
    m_bottomWidget->setFixedSize(bottomOuterWidget->size());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(topOuterWidget);
    mainLayout->addWidget(m_centerWidget);
    mainLayout->addWidget(bottomOuterWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    this->setLayout(mainLayout);

    m_opacityEffect = new QGraphicsOpacityEffect;
    m_opacityEffect->setOpacity(1.0);
    m_centerWidget->setGraphicsEffect(m_opacityEffect);

    m_topAni = new QPropertyAnimation(m_topWidget, "geometry");
    m_topAni->setDuration(DCC::FrameAnimationDuration);

    m_ctrHideAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_ctrHideAni->setDuration(DCC::FrameAnimationDuration);
    m_ctrHideAni->setEasingCurve(DCC::FrameHideCurve);

    m_ctrShowAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_ctrShowAni->setDuration(DCC::FrameAnimationDuration);
    m_ctrShowAni->setEasingCurve(DCC::FrameShowCurve);

    m_botAni = new QPropertyAnimation(m_bottomWidget, "geometry");
    m_botAni->setDuration(DCC::FrameAnimationDuration);

    connect(bottomButton, &DImageButton::clicked, this, &HomeScreen::powerButtonClicked, Qt::DirectConnection);
    connect(topButton, &UserAvatar::clicked, this, &HomeScreen::userAvatarClicked);
    connect(m_ctrHideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
    connect(m_ctrShowAni, &QPropertyAnimation::finished, this, &HomeScreen::showAniFinished, Qt::QueuedConnection);
}

HomeScreen::~HomeScreen()
{
    m_opacityEffect->deleteLater();
    m_topAni->deleteLater();
    m_ctrHideAni->deleteLater();
    m_ctrShowAni->deleteLater();
    m_botAni->deleteLater();
}

void HomeScreen::hide()
{
    m_topAni->stop();
    m_ctrHideAni->stop();
    m_ctrShowAni->stop();
    m_botAni->stop();

    m_ctrHideAni->setStartValue(1.0);
    m_ctrHideAni->setEndValue(0.0);

    m_botAni->setStartValue(QRect(0, 0, m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEndValue(QRect(0, m_bottomWidget->height(), m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEasingCurve(DCC::FrameHideCurve);

    m_topAni->setStartValue(QRect(0, 0, m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEndValue(QRect(0, -m_topWidget->height(), m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEasingCurve(DCC::FrameHideCurve);

    m_ctrHideAni->start();
    m_topAni->start();
    m_botAni->start();
}

void HomeScreen::show()
{
    m_topAni->stop();
    m_ctrHideAni->stop();
    m_ctrShowAni->stop();
    m_botAni->stop();

    m_ctrShowAni->setStartValue(0.0);
    m_ctrShowAni->setEndValue(1.0);

    m_botAni->setEndValue(QRect(0, 0, m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setStartValue(QRect(0, m_bottomWidget->height(), m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEasingCurve(DCC::FrameShowCurve);

    m_topAni->setEndValue(QRect(0, 0, m_topWidget->width(), m_topWidget->height()));
    m_topAni->setStartValue(QRect(0, -m_topWidget->height(), m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEasingCurve(DCC::FrameShowCurve);

    m_ctrShowAni->start();
    m_botAni->start();
    m_topAni->start();
    QFrame::show();
}

void HomeScreen::buttonClicked()
{
    ModuleButton *btn = qobject_cast<ModuleButton *>(sender());
    this->moduleSelected(btn->metaData());

    qDebug() << btn->metaData().name;
}

void HomeScreen::powerButtonClicked()
{
    QProcess *proc = new QProcess;
    QObject::connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), proc, &QProcess::deleteLater, Qt::QueuedConnection);
    proc->start("dde-shutdown");
}

void HomeScreen::userAvatarClicked()
{
    for (const ModuleMetaData &data : modules)
        if (data.name == "Account") {
            return moduleSelected(data);
        }
}

// class ModuleButton
ModuleButton::ModuleButton(ModuleMetaData metaData, QWidget *parent) :
    QFrame(parent),
    m_meta(metaData)
{
    setFixedSize(118, 110);
    setMouseTracking(true);

    // text font
    QFont font(QFont().family());
    font.setPixelSize(11);

    m_icon = new QLabel(this);
    m_text = new QLabel(this);
    m_text->setFont(font);
    m_text->setText(m_meta.name);
    m_text->setWordWrap(true);
    m_text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_text->setFixedWidth(90);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(m_icon);
    vLayout->setAlignment(m_icon, Qt::AlignHCenter);
    vLayout->addWidget(m_text);
    vLayout->setAlignment(m_text, Qt::AlignHCenter);
    vLayout->setContentsMargins(0, 24, 0, 0);
    vLayout->setSpacing(6);

    this->setLayout(vLayout);

    setState(Normal);
}

ModuleMetaData ModuleButton::metaData()
{
    return m_meta;
}

void ModuleButton::enterEvent(QEvent *)
{
    this->setState(Hover);
}

void ModuleButton::leaveEvent(QEvent *)
{
    this->setState(Normal);
}

void ModuleButton::mousePressEvent(QMouseEvent *)
{
    this->clicked();
}

void ModuleButton::mouseReleaseEvent(QMouseEvent *)
{
//    this->setState(Hover);
}

void ModuleButton::setState(State state)
{
#ifndef QT_DEBUG
    QString moduleIconsDir("/usr/share/dde-control-center/modules/icons/32/%1_%2.svg");
#else
    QString moduleIconsDir("modules/icons/32/%1_%2.svg");
#endif

    switch (state) {
    case Normal:
        this->setStyleSheet("QFrame { background-color: transparent; border-radius: 3 }");
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_meta.id).arg("normal")));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextNormalColor.name()));
        break;
    case Hover:
        this->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 3 }").arg(DCC::BgDarkColor.name()));
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_meta.id).arg("hover")));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextHoverColor.name()));
        break;
    default:
        break;
    }
}
