//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "updatecontrolpanel.h"
#include "common.h"

#include <DFloatingButton>
#include <DCommandLinkButton>
#include <DLabel>
#include <DIconButton>
#include <DTipLabel>
#include <DProgressBar>
#include <DSysInfo>
#include <DFontSizeManager>
#include <DIconTheme>

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

static constexpr int ProcessBarFixedWidth = 130;

updateControlPanel::updateControlPanel(QWidget *parent)
    : SettingsItem(parent)
    , m_titleLable(new DLabel(this))
    , m_versionLabel(new DLabel(this))
    , m_detailLabel(new DTipLabel("", this))
    , m_dateLabel(new DLabel(this))
    , m_progressLabel(new DLabel(this))
    , m_showMoreBUtton(new DCommandLinkButton("", this))
    , m_startButton(new DIconButton(this))
    , m_Progess(new DProgressBar(this))
    , m_buttonStatus(ButtonStatus::invalid)
    , m_progressType(UpdateDProgressType::InvalidType)
    , m_currentValue(0)
{
    initUi();
    initConnect();
}

void updateControlPanel::onStartUpdate()
{
    showUpdateProcess(true);

    setButtonStatus(ButtonStatus::pause);

    Q_EMIT startUpdate();
}

void updateControlPanel::setProgressValue(int value)
{
    if (value < 0 || value > 100)
        return;

    if (value == 0 && m_currentValue != 100) {
        return;
    }

    m_currentValue = value;

    m_Progess->setValue(value);
    QString text;
    switch (m_progressType) {
    case UpdateDProgressType::Download:
        text = tr("Downloading ") + QString("%1%").arg(value);
        break;
    case UpdateDProgressType::Paused:
        text = tr("Waiting ") + QString("%1%").arg(value);
        break;
    case UpdateDProgressType::Install:
        text = tr("Installing ") + QString("%1%").arg(value);
        break;
    case UpdateDProgressType::Backup:
        text = tr("Backing up ") + QString("%1%").arg(value);
        break;
    default:
        text = "";
        break;
    }

    setProgressText(text);
}

void updateControlPanel::setButtonIcon(ButtonStatus status)
{
    switch (status) {
    case ButtonStatus::start:
        m_startButton->setIcon(DIconTheme::findQIcon("dcc_start"));
        break;
    case ButtonStatus::pause:
        m_startButton->setIcon(DIconTheme::findQIcon("dcc_pause"));
        break;
    case ButtonStatus::retry:
        m_startButton->setIcon(DIconTheme::findQIcon("dcc_retry"));
        break;
    default:
        m_startButton->setIcon(static_cast<QStyle::StandardPixmap>(-1));
        break;

    }
}

UpdateDProgressType updateControlPanel::getProgressType() const
{
    return m_progressType;
}

void updateControlPanel::setProgressType(const UpdateDProgressType &progressType)
{
    m_progressType = progressType;
}

void updateControlPanel::showUpdateProcess(bool visible)
{
    m_Progess->setVisible(visible);
    m_startButton->setVisible(visible);
    m_progressLabel->setVisible(visible);
}

int updateControlPanel::getCurrentValue() const
{
    return m_currentValue;
}

void updateControlPanel::setCurrentValue(int currentValue)
{
    m_currentValue = currentValue;
}

void updateControlPanel::showButton(bool visible)
{
    m_startButton->setVisible(visible);
}

void updateControlPanel::setCtrlButtonEnabled(bool enabled)
{
    m_startButton->setEnabled(enabled);
}

void updateControlPanel::setDetailEnable(bool enable)
{
    m_showMoreBUtton->setEnabled(enable);
}
void updateControlPanel::setShowMoreButtonVisible(bool visible)
{
    m_showMoreBUtton->setVisible(visible);
}

void updateControlPanel::setDetailLabelVisible(bool visible)
{
    m_detailLabel->setVisible(visible);
}

void updateControlPanel::setVersionVisible(bool visible)
{
    m_versionLabel->setVisible(visible);
}

ButtonStatus updateControlPanel::getButtonStatus() const
{
    return m_buttonStatus;
}

void updateControlPanel::setButtonStatus(const ButtonStatus &value)
{
    m_buttonStatus = value;
    setButtonIcon(value);
    if (value == ButtonStatus::invalid) {
        m_startButton->setEnabled(false);
    }
}

void updateControlPanel::setTitle(QString title)
{
    m_titleLable->setText(title);
}

void updateControlPanel::setVersion(QString version)
{
    m_versionLabel->setVisible(!version.isEmpty());
    if (!version.isEmpty()) {
        m_versionLabel->setText(version);
    }
}

void updateControlPanel::setDetail(QString detail)
{
    m_detailLabel->setVisible(!detail.isEmpty());
    if (!detail.isEmpty()) {
        m_detailLabel->setText(htmlToCorrectColor(detail));
    }
}

void updateControlPanel::setDate(QString date)
{
    m_dateLabel->setVisible(!date.isEmpty());
    if (!date.isEmpty()) {
        m_dateLabel->setText(date);
    }
}

void updateControlPanel::setProgressText(const QString &text, const QString &toolTip)
{
    m_progressLabel->setText(getElidedText(m_progressLabel, text, Qt::ElideRight, m_progressLabel->maximumWidth() - 10, 0, __LINE__));
    m_progressLabel->setToolTip(toolTip);
}

//used to display long string: "12345678" -> "12345..."
const QString updateControlPanel::getElidedText(QWidget *widget, QString data, Qt::TextElideMode mode, int width, int flags, int line)
{
    QString retTxt = data;
    if (retTxt == "")
        return retTxt;

    QFontMetrics fontMetrics(font());
    int fontWidth = fontMetrics.horizontalAdvance(data);

    qInfo() << Q_FUNC_INFO << " [Enter], data, width, fontWidth : " << data << width << fontWidth << line;

    if (fontWidth > width) {
        retTxt = widget->fontMetrics().elidedText(data, mode, width, flags);
    }

    qInfo() << Q_FUNC_INFO << " [End], retTxt : " << retTxt;

    return retTxt;
}

void updateControlPanel::setShowMoreButtomText(QString text)
{
    m_showMoreBUtton->setText(text);
}

void updateControlPanel::onButtonClicked()
{
    int value = m_Progess->value();
    QString text = tr("Downloading ") + QString("%1%").arg(value);
    if (value <= 0 || value >= 100) {
        text = "";
    }

    ButtonStatus status = ButtonStatus::invalid;
    switch (m_buttonStatus) {
    case ButtonStatus::start:
        status = ButtonStatus::pause;
        setProgressText(text);
        Q_EMIT StartDownload();
        break;
    case ButtonStatus::pause:
        status = ButtonStatus::start;
        setProgressText(text);
        Q_EMIT PauseDownload();
        break;
    case ButtonStatus::retry:
        status = ButtonStatus::invalid;
        setProgressText("");
        Q_EMIT RetryUpdate();
        break;
    default:
        break;
    }

    setButtonStatus(status);
}

void updateControlPanel::setDatetimeVisible(bool visible)
{
    m_dateLabel->setVisible(visible);
}

void updateControlPanel::initUi()
{
    QVBoxLayout *titleLay = new QVBoxLayout();
    titleLay->setMargin(0);
    m_titleLable->setForegroundRole(DPalette::TextTitle);
    m_titleLable->setWordWrap(true);
    DFontSizeManager::instance()->bind(m_titleLable, DFontSizeManager::T6, QFont::DemiBold);
    titleLay->addWidget(m_titleLable, 0, Qt::AlignTop);

    DFontSizeManager::instance()->bind(m_versionLabel, DFontSizeManager::T8);
    m_versionLabel->setForegroundRole(DPalette::TextTitle);
    m_versionLabel->setObjectName("versionLabel");
    titleLay->addWidget(m_versionLabel);
    titleLay->addStretch();
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->addLayout(titleLay);

    QVBoxLayout *buttonLay = new QVBoxLayout();
    buttonLay->setSpacing(0);
    buttonLay->setContentsMargins(0, 0, 8, 0);

    m_startButton->setIcon(DIconTheme::findQIcon("dcc_start"));
    m_startButton->setIconSize(QSize(32, 32));
    m_startButton->setFlat(true);//设置背景透明
    m_startButton->setFixedSize(32, 32);
    m_startButton->hide();

    QHBoxLayout *progressLay = new QHBoxLayout;
    m_Progess->setFixedHeight(8);
    m_Progess->setRange(0, 100);
    m_Progess->setAlignment(Qt::AlignRight);
    m_Progess->setFixedWidth(ProcessBarFixedWidth);

    m_progressLabel->setVisible(false);
    DFontSizeManager::instance()->bind(m_progressLabel, DFontSizeManager::T10);
    m_progressLabel->setFixedWidth(ProcessBarFixedWidth);
    m_progressLabel->setScaledContents(true);
    m_progressLabel->setAlignment(Qt::AlignHCenter);


    QVBoxLayout *progressVlay = new QVBoxLayout;
    progressVlay->setSpacing(0);
    progressVlay->addWidget(m_progressLabel);
    progressVlay->addSpacing(2);

    progressVlay->addWidget(m_Progess);
    progressVlay->addStretch();
    progressLay->addLayout(progressVlay);

    QVBoxLayout *ctrlButtonVlay = new QVBoxLayout;
    int progressHeight = m_progressLabel->height();
    ctrlButtonVlay->addSpacing(progressHeight - 24);
    ctrlButtonVlay->addWidget(m_startButton, 0, Qt::AlignTop);
    ctrlButtonVlay->addStretch();
    progressLay->addLayout(ctrlButtonVlay);

    buttonLay->addLayout(progressLay);

    hlay->addLayout(buttonLay);

    DFontSizeManager::instance()->bind(m_detailLabel, DFontSizeManager::T8);
    m_detailLabel->setForegroundRole(DPalette::TextTips);
    m_detailLabel->adjustSize();
    m_detailLabel->setTextFormat(Qt::RichText);
    m_detailLabel->setAlignment(Qt::AlignJustify | Qt::AlignLeft);
    m_detailLabel->setWordWrap(true);
    m_detailLabel->setOpenExternalLinks(true);

    QHBoxLayout *dateLay = new QHBoxLayout();
    DFontSizeManager::instance()->bind(m_dateLabel, DFontSizeManager::T8);
    m_dateLabel->setObjectName("dateLable");
    m_dateLabel->setEnabled(false);

    auto pal = m_dateLabel->palette();
    QColor base_color = pal.text().color();
    base_color.setAlpha(255 / 10 * 6);
    pal.setColor(QPalette::Text, base_color);
    m_dateLabel->setPalette(pal);
    dateLay->addWidget(m_dateLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    dateLay->setSpacing(0);

    m_showMoreBUtton->setText(tr("Learn more"));
    DFontSizeManager::instance()->bind(m_showMoreBUtton, DFontSizeManager::T8);
    m_showMoreBUtton->setForegroundRole(DPalette::Button);
    dateLay->addStretch();
    dateLay->addWidget(m_showMoreBUtton, 0, Qt::AlignTop);
    dateLay->setContentsMargins(0, 0, 8, 0);

    QVBoxLayout *main = new QVBoxLayout();
    main->setSpacing(0);
    main->addLayout(hlay);
    main->addWidget(m_detailLabel);
    m_detailLabel->setContentsMargins(0, 5, 0, 0);
    main->addLayout(dateLay);
    main->addStretch();

    setLayout(main);
}

void updateControlPanel::initConnect()
{
    connect(m_showMoreBUtton, &DCommandLinkButton::clicked, this, &updateControlPanel::showDetail);
    connect(m_startButton, &DIconButton::clicked, this, &updateControlPanel::onButtonClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &updateControlPanel::onThemeChanged);
}

void updateControlPanel::onThemeChanged()
{
    m_detailLabel->setText(htmlToCorrectColor(m_detailLabel->text()));
}