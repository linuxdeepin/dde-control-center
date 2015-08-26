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

HomeScreen::HomeScreen(QList<ModuleMetaData> modules, QWidget *parent) :
    QFrame(parent),
    modules(modules)
{
    m_grid = new QGridLayout;
    m_grid->setContentsMargins(0, 25, 0, 0);
    foreach (ModuleMetaData meta, modules) {
        ModuleButton * button = new ModuleButton(meta, this);

        m_grid->addWidget(button, m_moduleCount / 3, m_moduleCount % 3);
        ++m_moduleCount;

        connect(button, &ModuleButton::clicked, this, &HomeScreen::buttonClicked);
    }

    QVBoxLayout *centerVLayout = new QVBoxLayout;
    QSpacerItem *vSpace = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    centerVLayout->addLayout(m_grid);
    centerVLayout->addSpacerItem(vSpace);

    m_centerWidget = new QWidget(this);
    m_centerWidget->setLayout(centerVLayout);

    QWidget *topOuterWidget = new QWidget(this);
    topOuterWidget->setFixedHeight(DCC::HomeScreen_TopWidgetHeight);
    topOuterWidget->setFixedWidth(DCC::ControlCenterWidth);

    DUI::DImageButton *topButton = new DUI::DImageButton(DCC::IconPath + "shutdown_normal.png", "", "");
    topButton->setAttribute(Qt::WA_TranslucentBackground);

    QLabel *topLabel = new QLabel();
    topLabel->setAlignment(Qt::AlignCenter);
    topLabel->setStyleSheet(QString("color:%1;").arg(DCC::HomeScreen_UsernameColor.name()));
#ifdef QT_DEBUG // for test username label
    topLabel->setText("TestUserName");
#endif

    QHBoxLayout *topHBox_top = new QHBoxLayout;
    topHBox_top->addStretch();
    topHBox_top->addWidget(topButton);
    topHBox_top->addStretch();
    topHBox_top->setContentsMargins(0, 20, 0, 0);

    QHBoxLayout *topHBox_bot = new QHBoxLayout;
    topHBox_bot->addStretch();
    topHBox_bot->addWidget(topLabel);
    topHBox_bot->addStretch();

    QVBoxLayout *topVBox = new QVBoxLayout;
    topVBox->addLayout(topHBox_top);
    topVBox->addLayout(topHBox_bot);
    topVBox->addWidget(new DUI::DSeparatorHorizontal);
    topVBox->setSpacing(0);
    topVBox->setMargin(0);

    m_topWidget = new QWidget(topOuterWidget);
    m_topWidget->setFixedSize(topOuterWidget->size());
    m_topWidget->setLayout(topVBox);
    m_topWidget->setStyleSheet(QString("background-color:%1;").arg(DCC::BgDarkColor.name()));

    DUI::DImageButton *bottomButton = new DUI::DImageButton(DCC::IconPath + "shutdown_normal.png", DCC::IconPath + "shutdown_hover.png", "");
    bottomButton->setAttribute(Qt::WA_TranslucentBackground);

    QLabel *bottomLabel = new QLabel(tr("电源"));
    bottomLabel->setAlignment(Qt::AlignCenter);
    bottomLabel->setStyleSheet(QString("color:%1;").arg(DCC::HomeScreen_PowerTextColor.name()));

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
    m_bottomWidget->setStyleSheet("background-image:url(:/resources/images/shutdown_bg.png);");
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

    connect(bottomButton, &DUI::DImageButton::clicked, this, &HomeScreen::powerButtonClicked, Qt::DirectConnection);
    connect(topButton, SIGNAL(clicked()), this, SLOT(userAvatarClicked()));
    connect(m_ctrHideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
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
    ModuleButton * btn = qobject_cast<ModuleButton*>(sender());
    this->moduleSelected(btn->metaData());

    qDebug() << btn->metaData().name;
}

void HomeScreen::powerButtonClicked()
{
    QProcess *proc = new QProcess;
    proc->start("dde-shutdown");

    QObject::connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), proc, &QProcess::deleteLater);
}

void HomeScreen::userAvatarClicked()
{
    qDebug() << "user avatar clicked";
    for (const ModuleMetaData & data : modules)
        if (data.name == "avator")
            return moduleSelected(data);
}

// class ModuleButton
ModuleButton::ModuleButton(ModuleMetaData metaData, QWidget * parent) :
    QFrame(parent),
    m_meta(metaData)
{
    setFixedSize(96, 96);
    setMouseTracking(true);

    m_icon = new QLabel(this);
    m_text = new QLabel(this);
    m_text->setText(m_meta.name);
    m_text->setWordWrap(true);
    m_text->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(m_icon);
    hLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_text);
    vLayout->setContentsMargins(0, 15, 0, 0);
    vLayout->setSpacing(0);

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
    QString moduleIconsDir("/usr/share/dde-control-center/modules/icons/%1");
#else
    QString moduleIconsDir("modules/icons/%1");
#endif

    switch (state) {
    case Normal:
        this->setStyleSheet("QFrame { background-color: transparent; border-radius: 3 }");
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_meta.normalIcon)));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextNormalColor.name()));
        break;
    case Hover:
        this->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 3 }").arg(DCC::BgDarkColor.name()));
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_meta.hoverIcon)));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextHoverColor.name()));
        break;
    default:
        break;
    }
}
