#include "login.h"
#include "widgets/utils.h"
#include "../../../mainwindow.h"
#include "../../../protocolfile.h"

#include <DSuggestButton>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>

#include <QProcess>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

#include "window/utils.h"

QString systemName() {
    if (IsProfessionalSystem)
        return "UOS";

    return "Deepin";
}

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sync ID Sign In
    DSuggestButton *loginBtn = new DSuggestButton(tr("Sign In"));
    loginBtn->setMinimumWidth(302);
    DTipLabel *tip = new DTipLabel(tr("After signing in, you can use cloud sync, app store, email client, browser and other cloud services.")
                                   .arg(systemName()));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(3);
    QLabel *pLogo = new QLabel;
    pLogo->setPixmap(QIcon::fromTheme("dcc_cloud").pixmap(128, 100));
    m_mainLayout->addWidget(pLogo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    QLabel *signintip = nullptr;
    if (DSysInfo::isCommunityEdition())
        signintip = new QLabel(tr("Deepin ID Sign In"));
    else
        signintip = new QLabel(tr("Union ID Sign In"));
    QFont font = signintip->font();
    font.setBold(true);
    signintip->setFont(font);
    DFontSizeManager::instance()->bind(signintip, DFontSizeManager::T3);

    m_mainLayout->addWidget(signintip, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(6);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    DTipLabel *sps = new DTipLabel(tr("Only supported in Mainland China at present"));
    m_mainLayout->addWidget(sps, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch(5);

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::licenceDialog);
}

void LoginPage::setMainWindow(MainWindow *pMainWindow)
{
    m_pMainWindow = pMainWindow;
}

void LoginPage::licenceDialog()
{
    // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
    QString title("《隐私政策》");
    QString allowContent("我已阅读并同意《隐私政策》");
    // license路径
    QString zhCN_Content = "";
    QString enUS_Content = "";

    ProtocolFile::getPrivacyFile(zhCN_Content, enUS_Content);

    m_licenceProcess = new QProcess(this);
    //添加-u和-b选项显示隐私政策页面表头和选项英文
    m_licenceProcess->start("dde-license-dialog",
                            QStringList() << "-t" << title << "-u" << "Privacy Policy"  << "-c" << zhCN_Content
                            << "-b" << "I have read and agree to the Privacy Policy"
                            << "-e" << enUS_Content << "-a" << allowContent);
    qDebug() << "Deliver content QStringList() = " << "dde-license-dialog"
             << "-t" << title << "-u" << "Privacy Policy"  << "-c" << zhCN_Content
             << "-b" << "I have read and agree to the Privacy Policy"
             << "-e" << enUS_Content << "-a" << allowContent;
    connect(m_licenceProcess, &QProcess::stateChanged, this, [this](QProcess::ProcessState state) {
        if (m_pMainWindow) {
            m_pMainWindow->setEnabled(state != QProcess::Running);
        } else {
            qDebug() << "licence dialog pMainWindow is nullptr";
        }
    });
    connect(m_licenceProcess, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::errorOccurred), this, [](QProcess::ProcessError error) {
        qDebug() << "error is " << error;
    });
    connect(m_licenceProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            Q_EMIT requestLoginUser();
        }
        m_licenceProcess->deleteLater();
        m_licenceProcess = nullptr;
    });
}
