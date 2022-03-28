/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
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
#include "logoitem.h"

#include <DToolButton>
#include <DLineEdit>
#include <DLabel>
#include <DSysInfo>
#include <DDesktopServices>

#include <QVBoxLayout>
#include <QApplication>
#include <QSettings>
#include <QIcon>
#include <QScrollArea>
#include <QScroller>
#include <QDebug>
#include <QKeyEvent>
#include <QSizePolicy>
#include <QTimer>

using namespace DCC_NAMESPACE;

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {

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

HostNameItem::HostNameItem(QWidget *parent)
    : SettingsItem(parent)
    , m_hostNameTitleLabel(new QLabel(tr("Computer Name") + ':', this))
    , m_hostNameLabel(new DLabel(this))
    , m_hostNameBtn(new DToolButton(this))
    , m_hostNameLineEdit(new HostNameEdit(this))
{
    initUI();
}

void HostNameItem::setHostName(const QString &name)
{
    m_hostname = name;
    const QString &text = getElidedText(name);
    m_hostNameLabel->setText(text);
}

void HostNameItem::onSetError(const QString &error)
{
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLabel->setVisible(false);
    m_hostNameBtn->setVisible(false);
    m_hostNameLineEdit->setAlert(true);
    m_hostNameLineEdit->showAlertMessage(error, this);
    m_alertMessage = error;
    DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
}

void HostNameItem::resizeEvent(QResizeEvent *event)
{
    if (!m_hostNameLineEdit)
        return;

    if(m_hostNameLineEdit->isAlert()) {
        m_hostNameLineEdit->hideAlertMessage();
        m_hostNameLineEdit->showAlertMessage(m_alertMessage,this);
    }

    if (m_hostNameLineEdit->lineEdit() && !m_hostnameEdit.isEmpty()) {
        m_hostNameLineEdit->lineEdit()->setText(m_hostnameEdit);
        QString txt = getElidedText(m_hostnameEdit);
        m_hostNameLabel->setText(txt);
    }
    SettingsItem::resizeEvent(event);
}
void HostNameItem::initUI()
{
    // 添加主机名称
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout *hostNameLayout = new QHBoxLayout(this);
    setLayout(hostNameLayout);
    m_hostNameTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hostNameLayout->addWidget(m_hostNameTitleLabel);
    hostNameLayout->addStretch();

    m_hostNameLabel->setForegroundRole(DPalette::TextTips);
    m_hostNameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hostNameLayout->addWidget(m_hostNameLabel);

    m_hostNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_hostNameBtn->setIconSize(QSize(12, 12));
    m_hostNameBtn->setFixedSize(36, 36);
    hostNameLayout->addWidget(m_hostNameBtn);

    QRegExp regx("^[A-Za-z0-9-]{0,64}$");
    QValidator *validator = new QRegExpValidator(regx, m_hostNameLineEdit);
    m_hostNameLineEdit->lineEdit()->setValidator(validator);
    m_hostNameLineEdit->setAlertMessageAlignment(Qt::AlignRight);
    m_hostNameLineEdit->lineEdit()->setAlignment(Qt::AlignRight);
    m_hostNameLineEdit->setFixedHeight(36);
    m_hostNameLineEdit->lineEdit()->setTextMargins(0,0,0,0);
    // lineEdit 无边框 透明背景
    DStyle::setFocusRectVisible(m_hostNameLineEdit->lineEdit(), false);
    QPalette palette = m_hostNameLineEdit->lineEdit()->palette();
    palette.setColor(QPalette::Button, Qt::transparent);
    m_hostNameLineEdit->lineEdit()->setPalette(palette);
    hostNameLayout->addWidget(m_hostNameLineEdit);
    m_hostNameLineEdit->hide();

    //点击编辑按钮
    connect(m_hostNameBtn, &DToolButton::clicked, this, &HostNameItem::onToolButtonButtonClicked);
    connect(m_hostNameLineEdit, &DLineEdit::focusChanged, this, &HostNameItem::onFocusChanged);
    connect(m_hostNameLineEdit, &DLineEdit::textEdited, this, &HostNameItem::onTextEdited);
    connect(m_hostNameLineEdit, &DLineEdit::alertChanged, this, &HostNameItem::onAlertChanged);
    connect(m_hostNameLineEdit->lineEdit(), &QLineEdit::editingFinished, this, &HostNameItem::onEditingFinished);
}

QString HostNameItem::getElidedText(const QString &string)
{
    const QString &titleText = m_hostNameTitleLabel->text();
    QFontMetrics titleFontMetrics(m_hostNameTitleLabel->font());
    int titleFontWidth = titleFontMetrics.boundingRect(titleText).width();

    int hostnameWidth = width() - titleFontWidth - m_hostNameBtn->width();
    qDebug() << "hostnameWidth:" << hostnameWidth << "width:" << width();
    QString retTxt = string;
    if (retTxt.isEmpty())
        return retTxt;

    QFontMetrics fontMetrics(font());
    int fontWidth = fontMetrics.boundingRect(string).width();

    if (fontWidth > hostnameWidth) {
        retTxt = m_hostNameLabel->fontMetrics().elidedText(string, Qt::ElideRight, hostnameWidth, 0);
    }

    return retTxt;
}

void HostNameItem::onEditingFinished()
{
    QString hostName = m_hostNameLineEdit->lineEdit()->text();
    if (hostName == m_hostname || hostName.simplified().isEmpty()) {
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
            Q_EMIT hostNameChanged(hostName);
        }
    }
}

void HostNameItem::onAlertChanged()
{
    // 输入框保持透明背景
    QPalette palette = m_hostNameLineEdit->lineEdit()->palette();
    palette.setColor(QPalette::Button, Qt::transparent);
    m_hostNameLineEdit->lineEdit()->setPalette(palette);
}

void HostNameItem::onTextEdited(const QString &hostName)
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

void HostNameItem::onFocusChanged(const bool onFocus)
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

void HostNameItem::onToolButtonButtonClicked()
{
    m_hostNameBtn->setVisible(false);
    m_hostNameLabel->setVisible(false);
    m_hostNameLineEdit->setVisible(true);
    m_hostNameLineEdit->setAlert(false);
    m_hostNameLineEdit->setText(m_hostname);
    m_hostNameLineEdit->hideAlertMessage();
    m_hostNameLineEdit->lineEdit()->setFocus();
    m_hostNameLineEdit->lineEdit()->selectAll();
}

NativeInfoWidget::NativeInfoWidget(SystemInfoModel *model, QWidget *parent)
    : QFrame(parent)
    , m_model(model)
{
    initWidget();
}

NativeInfoWidget::~NativeInfoWidget()
{
//    GSettingWatcher::instance()->erase("systeminfoNativeinfoAuthorized", m_authorized);
//    GSettingWatcher::instance()->erase("systeminfoNativeinfoKernel", m_kernel);
//    GSettingWatcher::instance()->erase("systeminfoNativeinfoProcessor", m_processor);
//    GSettingWatcher::instance()->erase("systeminfoNativeinfoMemory", m_memory);
//    GSettingWatcher::instance()->erase("edition", m_version);
}

void NativeInfoWidget::initWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    // info 与 logo 两个分组
    SettingsGroup *infoGroup = new SettingsGroup(this);
    SettingsGroup *logoGroup = new SettingsGroup(this);
    logoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoGroup->setSpacing(10);
    mainLayout->addWidget(logoGroup);
    mainLayout->addWidget(infoGroup);
    mainLayout->addStretch();

    // 添加logo及描述
    LogoItem *logo = new LogoItem(logoGroup);
    logo->setDescription(true); //显示文字描述
    logo->setDescription(systemCopyright());//LogoItem构造函数: set the discription visible=false
    logo->setLogo(DSysInfo::distributionOrgLogo(DSysInfo::Distribution, DSysInfo::Normal));
    logoGroup->appendItem(logo);

    HostNameItem *hostName = new HostNameItem(infoGroup);
    // 需要在初始化后进行设置，因为要获取width()属性
    QTimer::singleShot(0, hostName, [this, hostName] {
        hostName->setHostName(m_model->hostName());
    });
    connect(m_model, &SystemInfoModel::hostNameChanged, hostName, &HostNameItem::setHostName);
    connect(m_model, &SystemInfoModel::setHostNameError, hostName, &HostNameItem::onSetError);
    connect(hostName, &HostNameItem::hostNameChanged, m_model, &SystemInfoModel::setHostNameChanged);
    infoGroup->appendItem(hostName);

    if (DSysInfo::isDeepin()) {
        TitleValueItem *productName = new TitleValueItem(infoGroup);
        productName->setTitle(tr("OS Name") + ':');
        productName->setValue(m_model->productName());
        infoGroup->appendItem(productName);
        connect(m_model, &SystemInfoModel::productNameChanged, productName, &TitleValueItem::setValue);

        TitleValueItem *versionNumber = new TitleValueItem(infoGroup);
        versionNumber->setTitle(tr("Version") + ':');
        versionNumber->setValue(m_model->versionNumber());
        infoGroup->appendItem(versionNumber);
        connect(m_model, &SystemInfoModel::versionNumberChanged, versionNumber, &TitleValueItem::setValue);
    }

    TitleValueItem *edition = new TitleValueItem(infoGroup);
    edition->setTitle(tr("Edition") + ':');
    edition->setValue(m_model->version());
    infoGroup->appendItem(edition);
    connect(m_model, &SystemInfoModel::versionChanged, edition, &TitleValueItem::setValue);

    TitleValueItem *type = new TitleValueItem(infoGroup);
    type->setTitle(tr("Type") + ':');
    type->setValue(typeStr(m_model->type()));
    infoGroup->appendItem(type);
    connect(m_model, &SystemInfoModel::typeChanged, type, &TitleValueItem::setValue);

    if (!DSysInfo::isCommunityEdition()) {
        TitleAuthorizedItem *authorized = new TitleAuthorizedItem(infoGroup);
        authorized->setTitle(tr("Authorization") + ':');
        setLicenseState(authorized, m_model->licenseState());
        infoGroup->appendItem(authorized);
        connect(m_model, &SystemInfoModel::licenseStateChanged, authorized, [this, authorized] (ActiveState state) {
            setLicenseState(authorized, state);
        });
        connect(authorized, &TitleAuthorizedItem::clicked, this, &NativeInfoWidget::clickedActivator);
    }

    TitleValueItem *kernel = new TitleValueItem(infoGroup);
    kernel->setTitle(tr("Kernel") + ':');
    kernel->setValue(m_model->kernel());
    infoGroup->appendItem(kernel);
    connect(m_model, &SystemInfoModel::kernelChanged, kernel, &TitleValueItem::setValue);

    TitleValueItem *processor = new TitleValueItem(this);
    processor->setTitle(tr("Processor") + ':');
    processor->setValue(m_model->processor());
    infoGroup->appendItem(processor);
    connect(m_model, &SystemInfoModel::processorChanged, processor, &TitleValueItem::setValue);

    TitleValueItem *memory = new TitleValueItem(this);
    memory->setTitle(tr("Memory") + ':');
    memory->setValue(m_model->memory());
    infoGroup->appendItem(memory);
    connect(m_model, &SystemInfoModel::memoryChanged, memory, &TitleValueItem::setValue);
}

void NativeInfoWidget::setLicenseState(TitleAuthorizedItem *const authorized, ActiveState state)
{
    if (state == Authorized) {
        authorized->setValue(tr("Activated"));
        authorized->setValueForegroundRole(QColor(21, 187, 24));
        authorized->setButtonText(tr("View"));
    } else if (state == Unauthorized) {
        authorized->setValue(tr("To be activated"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(tr("Activate"));
    } else if (state == AuthorizedLapse) {
        authorized->setValue(tr("Expired"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(tr("View"));
    } else if (state == TrialAuthorized) {
        authorized->setValue(tr("In trial period"));
        authorized->setValueForegroundRole(QColor(255, 170, 0));
        authorized->setButtonText(tr("Activate"));
    } else if (state == TrialExpired) {
        authorized->setValue(tr("Trial expired"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(tr("Activate"));
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

const QString NativeInfoWidget::typeStr(const QString &type) const
{
    return tr("%1-bit").arg(type);
}

}

