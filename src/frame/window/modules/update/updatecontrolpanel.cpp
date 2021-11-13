//
// Created by swq on 2021/9/7.
//

#include "updatecontrolpanel.h"

#include <DFontSizeManager>
#include <widgets/basiclistdelegate.h>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

updateControlPanel::updateControlPanel(QWidget *parent)
    : SettingsItem(parent)
    , m_titleLable(new DLabel(this))
    , m_versionLabel(new DLabel(this))
    , m_detailLabel(new DTipLabel("", this))
    , m_dateLabel(new DLabel(this))
    , m_progressLabel(new DLabel(this))
    , m_updateButton(new DCommandLinkButton("", this))
    , m_showMoreBUtton(new DCommandLinkButton("", this))
    , m_startButton(new DIconButton(this))
    , m_Progess(new DProgressBar)
    , m_buttonStatus(ButtonStatus::invalid)
    , m_progressType(UpdateDProgressType::InvalidType)
    , m_currentValue(0)
{
    QVBoxLayout *titleLay = new QVBoxLayout();
    m_titleLable->setForegroundRole(DPalette::TextTitle);
    DFontSizeManager::instance()->bind(m_titleLable, DFontSizeManager::T6, QFont::DemiBold);
    titleLay->addWidget(m_titleLable, 0, Qt::AlignTop);

    DFontSizeManager::instance()->bind(m_versionLabel, DFontSizeManager::T8);
    m_versionLabel->setForegroundRole(DPalette::TextTips);
    titleLay->addWidget(m_versionLabel);
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->addLayout(titleLay);

    QVBoxLayout *buttonLay = new QVBoxLayout();
    m_updateButton->setText(tr("Download and install"));
    m_updateButton->setForegroundRole(DPalette::Button);
    DFontSizeManager::instance()->bind(m_updateButton, DFontSizeManager::T8, QFont::DemiBold);
    buttonLay->addWidget(m_updateButton, 0, Qt::AlignRight | Qt::AlignTop);

    QHBoxLayout *bulay = new QHBoxLayout();
    m_startButton->setIcon(QIcon::fromTheme("dcc_start"));
    m_startButton->setIconSize(QSize(32, 32));
    m_startButton->setFlat(true);//设置背景透明
    m_startButton->setFixedSize(32, 32);
    m_startButton->hide();

    QVBoxLayout *progressLay = new QVBoxLayout;
    m_Progess->setFixedHeight(10);
    m_Progess->setTextVisible(true);
    m_Progess->setTextDirection(QProgressBar::TopToBottom);
    m_Progess->setRange(0, 100);
    m_Progess->setAlignment(Qt::AlignRight);
    m_Progess->setMaximumWidth(100);
    m_Progess->setMaximumHeight(10);
    m_Progess->setVisible(false);

    m_progressLabel->setVisible(false);
    DFontSizeManager::instance()->bind(m_progressLabel, DFontSizeManager::T10);
    m_progressLabel->setMaximumWidth(100);
    progressLay->addWidget(m_progressLabel, 0, Qt::AlignCenter);
    progressLay->addWidget(m_Progess, 0, Qt::AlignBottom);

    bulay->addStretch();
    bulay->addLayout(progressLay);

    bulay->addWidget(m_startButton, 0, Qt::AlignRight);
    buttonLay->addLayout(bulay);

    hlay->addLayout(buttonLay);

    DFontSizeManager::instance()->bind(m_versionLabel, DFontSizeManager::T8);
    m_detailLabel->setForegroundRole(DPalette::TextTips);
    m_detailLabel->adjustSize();
    m_detailLabel->setTextFormat(Qt::RichText);
    m_detailLabel->setAlignment(Qt::AlignJustify | Qt::AlignLeft);
    m_detailLabel->setWordWrap(true);

    QHBoxLayout *dateLay = new QHBoxLayout();
    DFontSizeManager::instance()->bind(m_dateLabel, DFontSizeManager::T8);
    m_dateLabel->setForegroundRole(DPalette::TextTips);
    dateLay->addWidget(m_dateLabel, 0, Qt::AlignLeft);

    m_showMoreBUtton->setText(tr("Learn more"));
    DFontSizeManager::instance()->bind(m_showMoreBUtton, DFontSizeManager::T8, QFont::DemiBold);
    m_showMoreBUtton->setForegroundRole(DPalette::Button);
    dateLay->addStretch();
    dateLay->addWidget(m_showMoreBUtton);

    QVBoxLayout *main = new QVBoxLayout();
    main->addLayout(hlay);
    main->addSpacing(10);

    main->addWidget(m_detailLabel);
    main->addLayout(dateLay);

    setLayout(main);

    connect(m_showMoreBUtton, &DCommandLinkButton::clicked, this, &updateControlPanel::showDetail);
    connect(m_updateButton, &DCommandLinkButton::clicked, this, &updateControlPanel::onStartUpdate);
    connect(m_startButton, &DIconButton::clicked, this, &updateControlPanel::onButtonClicked);
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

    m_progressLabel->setText(text);
}

void updateControlPanel::setButtonIcon(ButtonStatus status)
{
    switch (status) {
    case ButtonStatus::start:
        m_startButton->setIcon(QIcon::fromTheme("dcc_start"));
        break;
    case ButtonStatus::pause:
        m_startButton->setIcon(QIcon::fromTheme("dcc_pause"));
        break;
    case ButtonStatus::retry:
        m_startButton->setIcon(QIcon::fromTheme("dcc_retry"));
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
    m_updateButton->setVisible(!visible);
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

void updateControlPanel::setUpdateButtonVisible(bool visible)
{
    m_updateButton->setVisible(visible);
}

void updateControlPanel::setUpdateButtonEnable(bool enable)
{
    m_updateButton->setEnabled(enable);
}

ButtonStatus updateControlPanel::getButtonStatus() const
{
    return m_buttonStatus;
}

void updateControlPanel::setButtonStatus(const ButtonStatus &value)
{
    if (m_buttonStatus == value) {
        return;
    }
    m_buttonStatus = value;
    setButtonIcon(value);
}

void updateControlPanel::setTitle(QString title)
{
    m_titleLable->setText(title);
}

void updateControlPanel::setVersion(QString version)
{
    m_versionLabel->setText(version);
}

void updateControlPanel::setDetail(QString detail)
{
    m_detailLabel->setText(detail);
}

void updateControlPanel::setDate(QString date)
{
    m_dateLabel->setText(date);
}

void updateControlPanel::setProgressText(QString text)
{
    m_progressLabel->setText(text);
}

void updateControlPanel::setShowMoreButtomText(QString text)
{
    m_showMoreBUtton->setText(text);
}

void updateControlPanel::onButtonClicked()
{
    int value = m_Progess->value();
    QString text = tr("Downloading ") + QString("%1%").arg(value);

    ButtonStatus status = ButtonStatus::invalid;
    switch (m_buttonStatus) {
    case ButtonStatus::start:
        status = ButtonStatus::pause;
        setProgressText(text);
        Q_EMIT StartDownload();
        break;
    case ButtonStatus::pause:
        status = ButtonStatus::start;
        Q_EMIT PauseDownload();
        break;
    case ButtonStatus::retry:
        status = ButtonStatus::pause;
        Q_EMIT RetryUpdate();
        break;
    default:
        break;
    }

    setButtonStatus(status);
}

