/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nativeinfowidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/titlevalueitem.h"
#include "modules/systeminfo/logoitem.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DToolButton>
#include <DLineEdit>
#include <DLabel>
#include <DDesktopServices>

#include <QVBoxLayout>
#include <QApplication>
#include <QSettings>
#include <QIcon>
#include <QScrollArea>
#include <QScroller>
#include <QDebug>
#include <DSysInfo>
#include <QKeyEvent>

using namespace dcc::widgets;
using namespace dcc::systeminfo;
#define GSETTINGS_CONTENS_SERVER "iscontens-server"
const int hostname_placeholder = 120;

DCORE_USE_NAMESPACE
namespace DCC_NAMESPACE {
namespace systeminfo {

HostNameEdit::HostNameEdit(QWidget *parent)
        : DLineEdit(parent)
{
    lineEdit()->setAcceptDrops(false);
    lineEdit()->setContextMenuPolicy(Qt::NoContextMenu);
    lineEdit()->installEventFilter(this);
}

bool HostNameEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == lineEdit() && event->type() == QEvent::KeyPress) {
        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);
        if (e && (e->matches(QKeySequence::Copy) || e->matches(QKeySequence::Cut) || e->matches(QKeySequence::Paste))) {
            return true;
        }

        if(e->key() >= 0x20 && e->key() <= 0x0a1) {
            //首先判断键盘事件带的字符串是否为符合"^[A-Za-z0-9-]+$"规则
            QRegExp regx("^[A-Za-z0-9-]+$");
            QRegExpValidator v(regx);
            QString text = e->text();
            int pos = 0;
            if(QValidator::Acceptable != v.validate(text, pos)) {
                DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
            }
        }
    }
    return DLineEdit::eventFilter(obj, event);
}

NativeInfoWidget::NativeInfoWidget(SystemInfoModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_mainLayout(new QVBoxLayout(this))
    , m_hostNameLayout(new QHBoxLayout(this))
    , m_hostNameSettingItem(new SettingsItem(this))
    , isContensServers(false)
    , m_hostname(model ? model->hostName() : "")
    , m_hostnameEdit(m_hostname)
{
    initWidget();
}

NativeInfoWidget::~NativeInfoWidget()
{
    GSettingWatcher::instance()->erase("systeminfoNativeinfoAuthorized", m_authorized);
    GSettingWatcher::instance()->erase("systeminfoNativeinfoKernel", m_kernel);
    GSettingWatcher::instance()->erase("systeminfoNativeinfoProcessor", m_processor);
    GSettingWatcher::instance()->erase("systeminfoNativeinfoMemory", m_memory);
    GSettingWatcher::instance()->erase("edition", m_version);
}

void NativeInfoWidget::initWidget()
{
    QFrame *frame = new QFrame(this);
    frame->setAccessibleName("NativeInfoWidget_frame");
    QWidget *container = new dcc::widgets::TranslucentFrame(this);
    container->setLayout(m_mainLayout);
    setContent(container);

    SettingsGroup *infoGroup = new SettingsGroup(frame);
    SettingsGroup *logoGroup = new SettingsGroup(frame);
    logoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    LogoItem *logo = new LogoItem(frame);
    logo->setDescription(true); //显示文字描述
    logo->setDescription(systemCopyright());//LogoItem构造函数: set the discription visible=false
    logo->setLogo(DSysInfo::distributionOrgLogo(DSysInfo::Distribution, DSysInfo::Normal));

    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer ||
            (DSysInfo::uosType() == DSysInfo::UosType::UosDesktop)) {
        m_productName = new TitleValueItem(frame);
        //~ contents_path /systeminfo/About This PC
        m_hostNameTitleLabel = new DLabel(tr("Computer Name:"));
        m_hostNameTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_hostNameLayout->addWidget(m_hostNameTitleLabel);

        m_hostNameLayout->addStretch(1);

        m_hostNameLabel = new DLabel();
        m_hostNameLabel->setForegroundRole(DPalette::TextTips);
        m_hostNameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_hostNameLayout->addWidget(m_hostNameLabel);

        m_hostNameBtn = new DToolButton(this);
        m_hostNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
        m_hostNameBtn->setIconSize(QSize(12, 12));
        m_hostNameBtn->setFixedSize(36, 36);
        m_hostNameLayout->addWidget(m_hostNameBtn);

        m_hostNameLineEdit = new HostNameEdit();
        m_hostNameLineEdit->lineEdit()->setFixedWidth(this->width() - hostname_placeholder);
        QRegExp regx("^[A-Za-z0-9-]{0,64}$");
        QValidator *validator = new QRegExpValidator(regx, m_hostNameLineEdit);
        m_hostNameLineEdit->lineEdit()->setValidator(validator);
        m_hostNameLineEdit->setAlertMessageAlignment(Qt::AlignRight);
        m_hostNameLineEdit->lineEdit()->setAlignment(Qt::AlignRight);
        m_hostNameLineEdit->setFixedHeight(36);
        m_hostNameLineEdit->lineEdit()->setFixedHeight(36);
        m_hostNameLineEdit->lineEdit()->setTextMargins(0,0,0,0);
        // lineEdit 无边框 透明背景
        DStyle::setFocusRectVisible(m_hostNameLineEdit->lineEdit(), false);
        QPalette palette = m_hostNameLineEdit->lineEdit()->palette();
        palette.setColor(QPalette::Button, Qt::transparent);
        m_hostNameLineEdit->lineEdit()->setPalette(palette);
        m_hostNameLayout->addWidget(m_hostNameLineEdit);
        m_hostNameLayout->setContentsMargins(10, 5, 0, 5);
        m_hostNameLineEdit->hide();

        m_hostNameSettingItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_hostNameSettingItem->setLayout(m_hostNameLayout);
        m_hostNameSettingItem->addBackground();

        m_hostNameLabel->setToolTip(m_model->hostName());
        m_hostNameLabel->setText(getElidedText(m_hostNameLabel, m_model->hostName(), Qt::ElideRight, this->width() - hostname_placeholder - 30, 0, __LINE__));
        m_hostNameLabel->setMinimumHeight(m_hostNameLineEdit->lineEdit()->height());
        //点击编辑按钮
        connect(m_hostNameBtn, &DToolButton::clicked, this, &NativeInfoWidget::onToolButtonButtonClicked);

        connect(m_hostNameLineEdit, &DLineEdit::focusChanged, this, &NativeInfoWidget::onFocusChanged);
        connect(m_hostNameLineEdit, &DLineEdit::textEdited, this, &NativeInfoWidget::onTextEdited);
        connect(m_hostNameLineEdit, &DLineEdit::alertChanged, this, &NativeInfoWidget::onAlertChanged);
        connect(m_hostNameLineEdit->lineEdit(), &QLineEdit::editingFinished, this, &NativeInfoWidget::onEditingFinished);
        connect(m_model, &SystemInfoModel::hostNameChanged, this, &NativeInfoWidget::onHostNameChanged);
        connect(m_model, &SystemInfoModel::setHostNameError, this, &NativeInfoWidget::onSetHostNameError);

        //~ contents_path /systeminfo/About This PC
        m_productName->setTitle(tr("OS Name:"));
        m_productName->setValue(m_model->productName());

        m_versionNumber = new TitleValueItem(frame);
        //~ contents_path /systeminfo/About This PC
        m_versionNumber->setTitle(tr("Version:"));
        m_versionNumber->setValue(m_model->versionNumber());
    }
    m_version = new TitleValueItem(frame);
    //~ contents_path /systeminfo/About This PC
    m_version->setTitle(tr("Edition:"));
    m_version->setValue(m_model->version());
    GSettingWatcher::instance()->bind("edition", m_version);
    m_type = new TitleValueItem(frame);
    //~ contents_path /systeminfo/About This PC
    m_type->setTitle(tr("Type:"));

    m_authorized = new TitleAuthorizedItem(frame);
    m_authorized->setTitle(tr("Authorization") + ':');
    m_authorized->setValue(tr("To be activated"));
    m_authorized->setValueForegroundRole(QColor(255, 0, 0));
    m_authorized->setButtonText(tr("Activate"));
    if (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC) {
        m_authorized->setVisable(false);
    } else {
        m_authorized->setVisable(true);
    }

    m_kernel = new TitleValueItem(frame);
    //~ contents_path /systeminfo/About This PC
    m_kernel->setTitle(tr("Kernel:"));
    m_kernel->setValue(m_model->kernel());

    m_processor = new TitleValueItem(frame);
    //~ contents_path /systeminfo/About This PC
    m_processor->setTitle(tr("Processor:"));
    m_processor->setValue(m_model->processor());

    m_memory = new TitleValueItem(frame);
    //~ contents_path /systeminfo/About This PC
    m_memory->setTitle(tr("Memory:"));
    m_memory->setValue(m_model->memory());

    GSettingWatcher::instance()->bind("systeminfoNativeinfoAuthorized", m_authorized);
    GSettingWatcher::instance()->bind("systeminfoNativeinfoKernel", m_kernel);
    GSettingWatcher::instance()->bind("systeminfoNativeinfoProcessor", m_processor);
    GSettingWatcher::instance()->bind("systeminfoNativeinfoMemory", m_memory);

    logoGroup->appendItem(logo);
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer ||
            (DSysInfo::uosType() == DSysInfo::UosType::UosDesktop)) {
        infoGroup->appendItem(m_productName);
        infoGroup->appendItem(m_versionNumber);
    }
    infoGroup->appendItem(m_version, SettingsGroup::ItemBackground);
    infoGroup->appendItem(m_type);
    if (!DSysInfo::isCommunityEdition())
        infoGroup->appendItem(m_authorized);
    m_authorized->setVisible(!DSysInfo::isCommunityEdition());
    infoGroup->appendItem(m_kernel);
    infoGroup->appendItem(m_processor);
    infoGroup->appendItem(m_memory);

    infoGroup->setSpacing(10);


    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);
    m_mainLayout->addWidget(logoGroup);
    m_mainLayout->addWidget(m_hostNameSettingItem);
    m_mainLayout->addWidget(infoGroup);
    m_mainLayout->addStretch(10);
    m_mainLayout->setContentsMargins(QMargins(10, 0, 10, 0));
    setLayout(m_mainLayout);
    setContentsMargins(0, 10, 0, 10);

    QScroller::grabGesture(this->window(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(this->window());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    connect(m_model, &SystemInfoModel::productNameChanged, this, &NativeInfoWidget::setProductName);
    connect(m_model, &SystemInfoModel::versionNumberChanged, this, &NativeInfoWidget::setVersionNumber);
    connect(m_model, &SystemInfoModel::versionChanged, this, &NativeInfoWidget::setEdition);
    connect(m_model, &SystemInfoModel::typeChanged, this, &NativeInfoWidget::setType);
    connect(m_model, &SystemInfoModel::processorChanged, this, &NativeInfoWidget::setProcessor);
    connect(m_model, &SystemInfoModel::memoryChanged, this, &NativeInfoWidget::setMemory);

    //传递button的点击信号
    connect(m_authorized, &TitleAuthorizedItem::clicked, this, &NativeInfoWidget::clickedActivator);
    connect(m_model, &SystemInfoModel::licenseStateChanged, this, &NativeInfoWidget::setLicenseState);

    setType(m_model->type());

    setLicenseState(m_model->licenseState());
}

void NativeInfoWidget::resizeEvent(QResizeEvent *event)
{
    ContentWidget::resizeEvent(event);

    if (!m_hostNameLineEdit)
        return;

    if(m_hostNameLineEdit->isAlert()) {
        m_hostNameLineEdit->hideAlertMessage();
        m_hostNameLineEdit->showAlertMessage(m_alertMessage,this);
    }

    if (m_hostNameLineEdit->lineEdit()) {
        m_hostNameLineEdit->lineEdit()->setFixedWidth(this->width() - hostname_placeholder);
        m_hostNameLineEdit->lineEdit()->setText(m_hostnameEdit);
        QString txt = getElidedText(m_hostNameLabel, m_hostname, Qt::ElideRight, this->width() - hostname_placeholder - 30, 0, __LINE__);
        m_hostNameLabel->setText(txt);
    }
}

void NativeInfoWidget::setProductName(const QString &edition)
{
    m_productName->setValue(edition);
}

void NativeInfoWidget::setVersionNumber(const QString &type)
{
    m_versionNumber->setValue(type);
}

void NativeInfoWidget::setEdition(const QString &edition)
{
    m_version->setValue(edition);
}

void NativeInfoWidget::setType(const QString &type)
{
    m_type->setValue(tr("%1-bit").arg(type));
}

void NativeInfoWidget::setProcessor(const QString &processor)
{
    m_processor->setValue(processor);
}

void NativeInfoWidget::setMemory(const QString &memory)
{
    m_memory->setValue(memory);
}

void NativeInfoWidget::setLicenseState(ActiveState state)
{
    if (state == Authorized) {
        m_authorized->setValue(tr("Activated"));
        m_authorized->setValueForegroundRole(QColor(21, 187, 24));
        m_authorized->setButtonText(tr("View"));
    } else if (state == Unauthorized) {
        m_authorized->setValue(tr("To be activated"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("Activate"));
    } else if (state == AuthorizedLapse) {
        m_authorized->setValue(tr("Expired"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("View"));
    } else if (state == TrialAuthorized) {
        m_authorized->setValue(tr("In trial period"));
        m_authorized->setValueForegroundRole(QColor(255, 170, 0));
        m_authorized->setButtonText(tr("Activate"));
    } else if (state == TrialExpired) {
        m_authorized->setValue(tr("Trial expired"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("Activate"));
    }
}

const QString NativeInfoWidget::systemCopyright() const
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString& oem_copyright = settings.value("system_info_vendor_name").toString().toLatin1();

    if (oem_copyright.isEmpty()) {
        if (DSysInfo::isCommunityEdition())
            return QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2011-%1 Deepin Community").arg(2022);
        else
            return QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2019-%1 UnionTech Software Technology Co., LTD").arg(2022);
    } else {
        return oem_copyright;
    }
}

const QString NativeInfoWidget::systemLogo() const
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString logo_path = settings.value("system_info_vendor_logo").toString();
    const QPixmap oem_logo(logo_path);

    if (oem_logo.isNull()) {
        return ":/systeminfo/themes/dark/icons/logo.png";
    } else {
        return logo_path;
    }
}
void NativeInfoWidget::onSetHostNameError(const QString &error)
{
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLineEdit->lineEdit()->setFocus();
    m_hostNameLabel->setVisible(false);
    m_hostNameBtn->setVisible(false);
    m_hostNameLineEdit->setAlert(true);
    m_hostNameLineEdit->showAlertMessage(error, this);
    m_alertMessage = error;
    DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
}

void NativeInfoWidget::onHostNameChanged(const QString &hostName)
{
    m_hostNameLabel->setToolTip(hostName);
    m_hostname = hostName;
    QString name = getElidedText(m_hostNameLabel, hostName, Qt::ElideRight, this->width() - hostname_placeholder - 30, 0, __LINE__);
    m_hostNameLabel->setText(name);
}

void NativeInfoWidget::onEditingFinished()
{
    QString hostName = m_hostNameLineEdit->lineEdit()->text();
    if (hostName == m_model->hostName() || hostName.simplified().isEmpty()) {
        m_hostNameLineEdit->lineEdit()->clearFocus();
        m_hostNameLineEdit->setVisible(false);
        m_hostNameLabel->setVisible(true);
        m_hostNameBtn->setVisible(true);
        if (m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }
        return;
    }

    if(!hostName.isEmpty()) {
        if((hostName.startsWith('-') || hostName.endsWith('-')) && hostName.size() <= 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("It cannot start or end with dashes"), this);
            m_alertMessage = tr("It cannot start or end with dashes");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
        else {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }

        if(!m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->lineEdit()->clearFocus();
            m_hostNameLineEdit->setVisible(false);
            m_hostNameLabel->setVisible(true);
            m_hostNameBtn->setVisible(true);
            Q_EMIT m_model->setHostNameChanged(hostName);
        }
    }
}

void NativeInfoWidget::onAlertChanged()
{
    // 输入框保持透明背景
    QPalette palette = m_hostNameLineEdit->lineEdit()->palette();
    palette.setColor(QPalette::Button, Qt::transparent);
    m_hostNameLineEdit->lineEdit()->setPalette(palette);
}

void NativeInfoWidget::onTextEdited(const QString &hostName)
{
    m_hostnameEdit = hostName;
    if (!hostName.isEmpty()) {
        if (hostName.size() > 63) {
            m_hostNameLineEdit->lineEdit()->backspace();
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("1~63 characters please"), this);
            m_alertMessage = tr("1~63 characters please");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_hostNameLineEdit->isAlert()) {
            m_hostNameLineEdit->setAlert(false);
            m_hostNameLineEdit->hideAlertMessage();
        }
    }
    else if (m_hostNameLineEdit->isAlert()) {
        m_hostNameLineEdit->setAlert(false);
        m_hostNameLineEdit->hideAlertMessage();
    }
}

void NativeInfoWidget::onFocusChanged(const bool onFocus)
{
    QString hostName = m_hostNameLineEdit->lineEdit()->text();
    if(!onFocus && hostName.isEmpty()) {
        m_hostNameLineEdit->setVisible(false);
        m_hostNameLabel->setVisible(true);
        m_hostNameBtn->setVisible(true);
    }
    else if(!onFocus && !hostName.isEmpty()) {
        if((hostName.startsWith('-') || hostName.endsWith('-')) && hostName.size() <= 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("It cannot start or end with dashes"), this);
            m_alertMessage = tr("It cannot start or end with dashes");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }else if (hostName.size() > 63) {
            m_hostNameLineEdit->setAlert(true);
            m_hostNameLineEdit->showAlertMessage(tr("1~63 characters please"), this);
            m_alertMessage = tr("1~63 characters please");
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    }
}

void NativeInfoWidget::onToolButtonButtonClicked()
{
    m_hostNameBtn->setVisible(false);
    m_hostNameLabel->setVisible(false);
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLineEdit->setAlert(false);
    m_hostNameLineEdit->setText(m_model->hostName());
    m_hostNameLineEdit->hideAlertMessage();
    m_hostNameLineEdit->lineEdit()->setFocus();
    m_hostNameLineEdit->lineEdit()->selectAll();
}

//used to display long string: "12345678" -> "12345..."
const QString NativeInfoWidget::getElidedText(QWidget* widget, QString data, Qt::TextElideMode mode, int width, int flags, int line)
{
    QString retTxt = data;
    if (retTxt == "")
        return retTxt;


    QFontMetrics fontMetrics(font());
    int fontWidth = fontMetrics.width(data);

    qInfo() << Q_FUNC_INFO << " [Enter], data, width, fontWidth : " << data << width << fontWidth << line;

    if (fontWidth > width) {
        retTxt = widget->fontMetrics().elidedText(data, mode, width, flags);
    }

    qInfo() << Q_FUNC_INFO << " [End], retTxt : " << retTxt;

    return retTxt;
}

}
}
