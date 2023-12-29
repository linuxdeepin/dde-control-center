//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "securitylevelitem.h"
#include "widgets/accessibleinterface.h"
#include "pwqualitymanager.h"

#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include <DLineEdit>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QSvgRenderer>

#define PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH ":/accounts/icons/dcc_deepin_password_strength_unactive_light_mode.svg"
#define PASSWORD_LEVEL_ICON_DEEP_MODE_PATH ":/accounts/icons/dcc_deepin_password_strength_unactive_deep_mode.svg"
#define PASSWORD_LEVEL_ICON_LOW_PATH ":/accounts/icons/dcc_deepin_password_strength_low.svg"
#define PASSWORD_LEVEL_ICON_MIDDLE_PATH ":/accounts/icons/dcc_deepin_password_strength_middle.svg"
#define PASSWORD_LEVEL_ICON_HIGH_PATH ":/accounts/icons/dcc_deepin_password_strength_high.svg"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
SET_FORM_ACCESSIBLE(SecurityLevelItem, "SecurityLevelItem")
SecurityLevelItem::SecurityLevelItem(QWidget *parent)
    : QWidget(parent)
    , m_newPasswdLevelText(new QLabel(this))
    , m_level(NoneLevel)
{
    initIcons();
    initUi();
}

SecurityLevelItem::~SecurityLevelItem()
{
}

void SecurityLevelItem::setUser(const QString &userName)
{
    m_userName = userName;
}

void SecurityLevelItem::bind(Dtk::Widget::DLineEdit *lineEdit)
{
    QObject::disconnect(lineEdit, &DLineEdit::textChanged, this, nullptr);
    QObject::connect(lineEdit, &DLineEdit::textChanged, this, [this, lineEdit](const QString &text) {
        if (text.isEmpty()) {
            setLevel(SecurityLevelItem::NoneLevel);
            lineEdit->setAlert(false);
            lineEdit->hideAlertMessage();
            return;
        }
        PwqualityManager *pwQualityManager = PwqualityManager::instance();
        PASSWORD_LEVEL_TYPE level = pwQualityManager->GetNewPassWdLevel(text);
        PwqualityManager::ERROR_TYPE error = pwQualityManager->verifyPassword(m_userName, text);

        switch (level) {
        case PASSWORD_STRENGTH_LEVEL_HIGH:
            setLevel(SecurityLevelItem::HighLevel);
            break;
        case PASSWORD_STRENGTH_LEVEL_MIDDLE:
            setLevel(SecurityLevelItem::MidLevel);
            break;
        case PASSWORD_STRENGTH_LEVEL_LOW:
            setLevel(SecurityLevelItem::LowLevel);
            break;
        default:
            lineEdit->showAlertMessage(QObject::tr("Error occurred when reading the configuration files of password rules!"));
            return;
        }
        if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
            lineEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
            lineEdit->setAlert(true);
            lineEdit->showAlertMessage(pwQualityManager->getErrorTips(error), lineEdit, 2000);
        } else {
            lineEdit->setAlert(false);
            lineEdit->hideAlertMessage();
        }
    });
}

void SecurityLevelItem::setLevel(SecurityLevelItem::Level level)
{
    if (level == m_level)
        return;

    m_level = level;

    QPalette palette;
    switch (level) {
    case NoneLevel:
        m_newPasswdLevelText->setText(tr(""));
        m_newPasswdLevelIcons[0]->setPixmap(m_icons[NoneIcon]);
        m_newPasswdLevelIcons[1]->setPixmap(m_icons[NoneIcon]);
        m_newPasswdLevelIcons[2]->setPixmap(m_icons[NoneIcon]);
        break;
    case LowLevel:
        palette.setColor(QPalette::Text, QColor("#FF5736"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setForegroundRole(QPalette::Text);
        m_newPasswdLevelText->setText(tr("Weak"));

        m_newPasswdLevelIcons[0]->setPixmap(m_icons[RedIcon]);
        m_newPasswdLevelIcons[1]->setPixmap(m_icons[NoneIcon]);
        m_newPasswdLevelIcons[2]->setPixmap(m_icons[NoneIcon]);
        break;
    case MidLevel:
        palette.setColor(QPalette::Text, QColor("#FFAA00"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setForegroundRole(QPalette::Text);
        m_newPasswdLevelText->setText(tr("Medium"));

        m_newPasswdLevelIcons[0]->setPixmap(m_icons[YellowIcon]);
        m_newPasswdLevelIcons[1]->setPixmap(m_icons[YellowIcon]);
        m_newPasswdLevelIcons[2]->setPixmap(m_icons[NoneIcon]);
        break;
    case HighLevel:
        palette.setColor(QPalette::Text, QColor("#15BB18"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setForegroundRole(QPalette::Text);
        m_newPasswdLevelText->setText(tr("Strong"));

        m_newPasswdLevelIcons[0]->setPixmap(m_icons[GreenIcon]);
        m_newPasswdLevelIcons[1]->setPixmap(m_icons[GreenIcon]);
        m_newPasswdLevelIcons[2]->setPixmap(m_icons[GreenIcon]);
        break;
    }
}

const QPixmap SecurityLevelItem::loadSvgImg(const QString &fileName, const int width, const int hight)
{
    if (!QFileInfo::exists(fileName))
        return QPixmap();

    QPixmap pixmap(width, hight);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(qRound(qApp->devicePixelRatio()));

    return pixmap;
}

void SecurityLevelItem::initUi()
{
    for (int i = 0; i < PASSWORD_LEVEL_ICON_NUM; i++) {
        m_newPasswdLevelIcons[i] = new QLabel;
    }

    QHBoxLayout *newPasswdLevelLayout = new QHBoxLayout;
    newPasswdLevelLayout->setMargin(0);
    m_newPasswdLevelText->setFixedWidth(55);
    m_newPasswdLevelText->setFixedHeight(20);
    m_newPasswdLevelText->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_newPasswdLevelText, DFontSizeManager::T8);
    newPasswdLevelLayout->addWidget(m_newPasswdLevelText, 0, Qt::AlignRight);
    newPasswdLevelLayout->addSpacing(4);

    for (int i = 0; i < PASSWORD_LEVEL_ICON_NUM; i++) {
        m_newPasswdLevelIcons[i]->setFixedWidth(8);
        m_newPasswdLevelIcons[i]->setFixedHeight(4);
        m_newPasswdLevelIcons[i]->setPixmap(m_icons[NoneIcon]);
    }

    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[0]);
    newPasswdLevelLayout->addSpacing(4);

    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[1]);
    newPasswdLevelLayout->addSpacing(4);

    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[2]);
    newPasswdLevelLayout->addSpacing(50);
    setLayout(newPasswdLevelLayout);
}

void SecurityLevelItem::initIcons()
{
    qreal pixelRatio = devicePixelRatioF();

    auto onThemeTypeChanged = [=](DGuiApplicationHelper::ColorType themeType) {
        switch (themeType) {
        case DGuiApplicationHelper::UnknownType:
        case DGuiApplicationHelper::LightType:
            m_icons[NoneIcon] = loadSvgImg(PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH, qRound(8 * pixelRatio), qRound(4 * pixelRatio));
            break;
        case DGuiApplicationHelper::DarkType:
            m_icons[NoneIcon] = loadSvgImg(PASSWORD_LEVEL_ICON_DEEP_MODE_PATH, qRound(8 * pixelRatio), qRound(4 * pixelRatio));
            break;
        }
    };

    onThemeTypeChanged(DGuiApplicationHelper::instance()->themeType());

    m_icons[RedIcon] = loadSvgImg(PASSWORD_LEVEL_ICON_LOW_PATH, qRound(8 * pixelRatio), qRound(4 * pixelRatio));
    m_icons[YellowIcon] = loadSvgImg(PASSWORD_LEVEL_ICON_MIDDLE_PATH, qRound(8 * pixelRatio), qRound(4 * pixelRatio));
    m_icons[GreenIcon] = loadSvgImg(PASSWORD_LEVEL_ICON_HIGH_PATH, qRound(8 * pixelRatio), qRound(4 * pixelRatio));

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [=](DGuiApplicationHelper::ColorType themeType) {
        onThemeTypeChanged(themeType);
        setLevel(m_level);
    });
}
