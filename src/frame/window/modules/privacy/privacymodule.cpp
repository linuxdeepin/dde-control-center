// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "permissioninfowidget.h"
#include "permissionlistwidget.h"
#include "window/modules/privacy/privacymodule.h"
#include "modules/privacy/privacysecuritymodel.h"

using namespace DCC_NAMESPACE;

PrivacyModule::PrivacyModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
{
    m_privacyPages.append({ PermissionType::CameraPermission,
                            "camera",
                            //~ contents_path /privacy/camera
                            //~ child_page camera
                            tr("Camera"),
                            //~ contents_path /privacy/camera
                            //~ child_page camera
                            tr("Allow below apps to use your camera:"),
                            //~ contents_path /privacy/camera
                            //~ child_page camera
                            tr("No apps requested access to the camera"),
                            QIcon::fromTheme("dcc_camera"),
                            { PermissionType::CameraPermission } });

    m_privacyPages.append({ PermissionType::FoldersPermission,
                            "folders",
                            //~ contents_path /privacy/folders
                            //~ child_page folders
                            tr("Files and Folders"),
                            //~ contents_path /privacy/folders
                            //~ child_page folders
                            tr("Allow below apps to access these files and folders:"),
                            //~ contents_path /privacy/folders
                            //~ child_page folders
                            tr("No apps requested access to files and folders"),
                            QIcon::fromTheme("folder"),
                            { PermissionType::DocumentsFoldersPermission, PermissionType::PicturesFoldersPermission } });
}

PrivacyModule::~PrivacyModule()
{
    m_model->deleteLater();
}

const QString PrivacyModule::name() const
{
    return QStringLiteral("privacy");
}

const QString PrivacyModule::displayName() const
{
    return QObject::tr("Privacy and Security");
}

void PrivacyModule::preInitialize(bool sync, FrameProxyInterface::PushType type)
{
    Q_UNUSED(sync)
    Q_UNUSED(type);
    if (m_model) {
        delete m_model;
    }
    m_model = new PrivacySecurityModel(this);
    m_model->moveToThread(qApp->thread());

    QWidget *mainWindow = dynamic_cast<QWidget *>(m_frameProxy);
    if (mainWindow) {
        connect(m_model, &PrivacySecurityModel::checkAuthorization, mainWindow, &QWidget::setDisabled);
    }
    connect(m_model, &PrivacySecurityModel::fileArmorExistsChanged, this, &PrivacyModule::serveChanged);
    addChildPageTrans();
    initSearchData();
    QTimer::singleShot(100, this, [this]() {
        serveChanged(m_model->existsFileArmor());
    });
}

void PrivacyModule::initialize()
{
}

void PrivacyModule::active()
{
    activePage(m_privacyPages.first().mainPremission);
}

void PrivacyModule::deactive()
{
    m_model->deactivate();
}

void PrivacyModule::showPage(const QString &pageName)
{
    for (auto &&page : m_privacyPages) {
        if (page.name == pageName) {
            onShowPage(page.mainPremission);
        }
    }
}

QWidget *PrivacyModule::moduleWidget()
{
    return nullptr;
}

int PrivacyModule::load(const QString &path)
{
    for (auto &&page : m_privacyPages) {
        if (path == page.name) {
            activePage(page.mainPremission);
            return 0;
        }
    }
    return -1;
}

QString PrivacyModule::path() const
{
    return "mainwindow";
}

QString PrivacyModule::follow() const
{
    return QStringLiteral("keyboard");
}

QStringList PrivacyModule::availPage() const
{
    QStringList pages;
    for (auto &&page : m_privacyPages) {
        pages.append(page.name);
    }
    return pages;
}

void PrivacyModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        for (auto &&page : m_privacyPages) {
            m_frameProxy->addChildPageTrans(page.name, page.displayName);
        }
    }
}

void PrivacyModule::onShowPage(int id)
{
    for (auto &&page : m_privacyPages) {
        if (page.mainPremission == id) {
            PermissionInfoWidget *InfoWidget = new PermissionInfoWidget(page, m_model);
            m_frameProxy->pushWidget(this, InfoWidget);
            connect(InfoWidget, &PermissionInfoWidget::destroyed, m_model, &PrivacySecurityModel::checkAuthorizationCancel);
        }
    }
}

void PrivacyModule::serveChanged(bool exists)
{
    if (m_frameProxy != nullptr) {
        m_frameProxy->setModuleVisible(this, exists);
        initSearchData();
    }
}

void PrivacyModule::initSearchData()
{
    if (m_frameProxy != nullptr) {
        const QString &module = displayName();
        bool exist = m_model->existsFileArmor();
        for (auto &&page : m_privacyPages) {
            m_frameProxy->setWidgetVisible(module, page.displayName, exist);
            m_frameProxy->setDetailVisible(module, page.displayName, page.displayName, exist);
            m_frameProxy->setDetailVisible(module, page.displayName, page.desc, exist);
        }
        m_frameProxy->updateSearchData(module);
    }
}

void PrivacyModule::activePage(int id)
{
    m_model->activate();
    if (m_listWidget.isNull()) {
        m_listWidget = new PermissionListWidget(m_privacyPages);
        connect(m_listWidget, &PermissionListWidget::requestShowPage, this, &PrivacyModule::onShowPage);
        m_frameProxy->pushWidget(this, m_listWidget);
    }
    m_listWidget->setCurrent(id);
}
