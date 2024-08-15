//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "developermodedialog.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "widgets/titlelabel.h"

#include <DTipLabel>
#include <DTitlebar>
#include <DWindowCloseButton>
#include <DTextBrowser>
#include <DIconTheme>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QDBusInterface>
#include <QDebug>
#include <QDBusReply>
#include <QFileDialog>
#include <QJsonArray>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonDocument>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

DeveloperModeDialog::DeveloperModeDialog(QObject *parent)
    : DAbstractDialog(parent)
    , m_importFile(new QFileDialog(this))
    , m_exportFile(new QFileDialog(this))
{
    //注册类型
    qRegisterMetaType<DMIInfo>("DMIInfo");
    qDBusRegisterMetaType<DMIInfo>();
    qRegisterMetaType<HardwareInfo>("HardwareInfo");
    qDBusRegisterMetaType<HardwareInfo>();

    setAccessibleName("DeveloperModeDialog");
    setMinimumSize(QSize(350, 380));
    //总布局
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    //图标和关闭按钮布局
    QHBoxLayout *titleHBoxLayout = new QHBoxLayout();
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setAccessibleName("DeveloperModeDialog_titleIcon");
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setIcon(DIconTheme::findQIcon("preferences-system"));
    titleHBoxLayout->addWidget(titleIcon, Qt::AlignTop);
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle("");
    //内容布局
    QVBoxLayout *contentVBoxLayout = new QVBoxLayout();
    auto chooseModeTip = new TitleLabel(tr("Request Root Access"), this);
    chooseModeTip->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    chooseModeTip->setWordWrap(true);

    contentVBoxLayout->setContentsMargins(40, 0, 40, 20);
    contentVBoxLayout->addWidget(chooseModeTip, 0, Qt::AlignTop);

    auto hw = new QWidget();
    hw->setAccessibleName("hBoxLayout");
    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    m_onlineBtn = new DRadioButton(tr("Online"));
    m_offlineBtn = new DRadioButton(tr("Offline"));
    m_onlineBtn->setChecked(true);
    hw->setLayout(hBoxLayout);

    hBoxLayout->setSpacing(20);
    hBoxLayout->addWidget(m_onlineBtn, 0, Qt::AlignCenter);
    hBoxLayout->addWidget(m_offlineBtn, 0, Qt::AlignCenter);
    contentVBoxLayout->addWidget(hw, 0, Qt::AlignTop | Qt::AlignHCenter);

    //在线激活模式提示
    auto chooseModeCommonts = new DTextBrowser();
    chooseModeCommonts->setAccessibleName("DeveloperModeDialog_DTextBrowser");
    auto mpalette = this->palette();
    mpalette.setBrush(QPalette::Base, QBrush(Qt::NoBrush));
    chooseModeCommonts->setPalette(mpalette);
    chooseModeCommonts->setFrameStyle(QFrame::NoFrame);
    chooseModeCommonts->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    chooseModeCommonts->setText(tr("Please sign in to your Union ID first and continue"));

    contentVBoxLayout->addWidget(chooseModeCommonts, 1, Qt::AlignTop | Qt::AlignHCenter);

    //在线激活模式下一步按钮
    m_nextButton  = new DSuggestButton(tr("Next"));
    contentVBoxLayout->addWidget(m_nextButton, 0, Qt::AlignBottom);

    //离线激活模式导出机器信息和导入证书按钮
    auto exportBtn = new QPushButton(tr("Export PC Info"));
    auto importBtn = new DSuggestButton(tr("Import Certificate"));
    importBtn->setAccessibleName("importBtn");
    exportBtn->setVisible(false);
    importBtn->setVisible(false);

    contentVBoxLayout->addWidget(exportBtn, 0, Qt::AlignBottom);
    contentVBoxLayout->addWidget(importBtn, 0, Qt::AlignBottom);

    //加入布局
    vBoxLayout->addLayout(titleHBoxLayout);
    vBoxLayout->addLayout(contentVBoxLayout);
    vBoxLayout->setMargin(0);
    setLayout(vBoxLayout);

    m_importFile->setModal(true);
    m_importFile->setAcceptMode(QFileDialog::AcceptSave);
    m_importFile->setNameFilter("*.json");
    // 以读写方式打开主目录下的1.json文件，若该文件不存在则会自动创建
    m_importFile->selectFile("1.json");
    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if (!directory.isEmpty()) {
        m_importFile->setDirectory(directory.first());
    }

    m_exportFile->setModal(true);
    m_exportFile->setAcceptMode(QFileDialog::AcceptOpen);
    directory = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    if (!directory.isEmpty()) {
        m_exportFile->setDirectory(directory.first());
    }

    connect(m_importFile, &QFileDialog::finished, this, [ = ](int result) {
        if (result == QFileDialog::Accepted) {
            QDBusInterface licenseInfo("com.deepin.sync.Helper",
                                       "/com/deepin/sync/Helper",
                                       "com.deepin.sync.Helper",
                                       QDBusConnection::systemBus());

            QDBusReply<HardwareInfo> hardwareInfo = licenseInfo.call(QDBus::AutoDetect, "GetHardware");
            QString fileName = m_importFile->selectedFiles().first();
            if (fileName.isEmpty())
                return;

            QFile file(fileName);
            if (!file.open(QIODevice::ReadWrite))
                qDebug() << "File open error";
            else
                qDebug() << "File open!";

            // 使用QJsonObject对象插入键值对。
            QJsonObject jsonObject;
            auto hardwareInfoValue = hardwareInfo.value();
            auto hardwareDMIValue = hardwareInfo.value().dmi;
            jsonObject.insert("id", hardwareInfoValue.id);
            jsonObject.insert("hostname", hardwareInfoValue.hostName);
            jsonObject.insert("username", hardwareInfoValue.username);
            jsonObject.insert("cpu", hardwareInfoValue.cpu);
            jsonObject.insert("laptop", hardwareInfoValue.laptop);
            jsonObject.insert("memory", hardwareInfoValue.memory);
            jsonObject.insert("network_cards", hardwareInfoValue.networkCards);

            QJsonObject objectDMI;
            objectDMI.insert("bios_vendor", hardwareDMIValue.biosVendor);
            objectDMI.insert("bios_version", hardwareDMIValue.biosVersion);
            objectDMI.insert("bios_date", hardwareDMIValue.biosDate);
            objectDMI.insert("board_name", hardwareDMIValue.boardName);
            objectDMI.insert("board_serial", hardwareDMIValue.boardSerial);
            objectDMI.insert("board_vendor", hardwareDMIValue.boardVendor);
            objectDMI.insert("board_version", hardwareDMIValue.boardVersion);
            objectDMI.insert("product_name", hardwareDMIValue.productName);
            objectDMI.insert("product_family", hardwareDMIValue.productFamily);
            objectDMI.insert("product_serial", hardwareDMIValue.producctSerial);
            objectDMI.insert("product_uuid", hardwareDMIValue.productUUID);
            objectDMI.insert("product_version", hardwareDMIValue.productVersion);

            jsonObject.insert("dmi", objectDMI);
            //使用QJsonDocument设置该json对象
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);

            //将json以文本形式写入文件并关闭文件
            file.write(jsonDoc.toJson());
            file.close();
        }
    });

    connect(m_exportFile, &QFileDialog::finished, this, [ = ](int result) {
        if (result == QFileDialog::Accepted) {
            QString fileName = m_exportFile->selectedFiles().first();
            if (fileName.isEmpty())
                return;

            Q_EMIT requestCommit(fileName);
            close();
        }
    });

    //选择激活在线模式或离线模式
    connect(m_onlineBtn, &QAbstractButton::toggled, [ = ] {
        if (m_onlineBtn->isChecked()) {
            exportBtn->setVisible(false);
            importBtn->setVisible(false);
            m_nextButton->setVisible(true);

            chooseModeCommonts->setText(tr("Please sign in to your Union ID first and continue"));
            this->update();
        } else {
            m_nextButton->setVisible(false);
            exportBtn->setVisible(true);
            importBtn->setVisible(true);
            chooseModeCommonts->setText(tr("1. Export your PC information") + '\n'
                                        + tr("2. Go to https://www.chinauos.com/developMode to download an offline certificate") + '\n'
                                        + tr("3. Import the certificate"));
        }
    });

    connect(m_nextButton, &QPushButton::clicked, this, &DeveloperModeDialog::setLogin);
    connect(exportBtn, &QPushButton::clicked, [ this ] {
        m_importFile->show();
    });

    //离线模式导入证书
    connect(importBtn, &QPushButton::clicked, [ this ] {
        m_exportFile->show();
    });
}

DeveloperModeDialog::~DeveloperModeDialog()
{
    if (m_importFile)
        m_importFile->deleteLater();

    if (m_exportFile)
        m_exportFile->deleteLater();
}

void DeveloperModeDialog::setModel(CommonInfoModel *model)
{
    m_model = model;
}

void DeveloperModeDialog::shutdown()
{
    if (m_importFile && m_importFile->isVisible())
        m_importFile->reject();

    if (m_exportFile  && m_exportFile->isVisible())
        m_exportFile->reject();
}

void DeveloperModeDialog::setLogin()
{
    auto model = m_model;
    auto btn = m_nextButton;
    Q_ASSERT(model);
    auto requestDev = [this, btn] {
        btn->clearFocus();
        //防止出现弹窗时可以再次点击按钮
        hide();
        QTimer::singleShot(100, this, [ this ] {
            Q_EMIT requestDeveloperMode(true);
        });
    };

    if (!model->isLogin()) {
        m_enterDev = true;
        btn->clearFocus();
        Q_EMIT requestLogin();
        connect(model, &CommonInfoModel::isLoginChenged, this, [requestDev, this](bool log) {
            if (!log || !m_enterDev)
                return;

            requestDev();
            m_enterDev = false;
        });
    } else {
        requestDev();
    }
}

void DeveloperModeDialog::showEvent(QShowEvent *)
{
    setFocus();
}
