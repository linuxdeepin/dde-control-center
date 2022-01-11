#include "loginoptionsmodule.h"
#include "loginoptionswidget.h"
#include "fingerdetailwidget.h"
#include "faceiddetailwidget.h"
#include "irisdetailwidget.h"
#include "addfingedialog.h"
#include "addfaceinfodialog.h"
#include "faceinfodialog.h"
#include "addirisinfodialog.h"
#include "modules/authentication/fingermodel.h"
#include "modules/authentication/fingerworker.h"
#include "modules/authentication/charamangermodel.h"
#include "modules/authentication/charamangerworker.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::authentication;
using namespace dcc;
using namespace dcc::authentication;

LoginOptionsModule::LoginOptionsModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject (parent)
    , ModuleInterface (frame)
{
    m_frameProxy = frame;
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

void LoginOptionsModule::initialize()
{
    m_fingerModel = new FingerModel(this);
    m_fingerWorker = new FingerWorker(m_fingerModel);
    m_fingerModel->moveToThread(qApp->thread());
    m_fingerWorker->moveToThread(qApp->thread());

    m_charaMangerModel = new CharaMangerModel(this);
    m_charaMangerWorker = new CharaMangerWorker(m_charaMangerModel);
    m_charaMangerModel->moveToThread(qApp->thread());
    m_charaMangerWorker->moveToThread(qApp->thread());
}

void LoginOptionsModule::reset()
{

}

const QString LoginOptionsModule::name() const
{
    return "authentication";
}

const QString LoginOptionsModule::displayName() const
{
    return  tr("Biometric Authentication");
}

void LoginOptionsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

void LoginOptionsModule::contentPopped(QWidget * const w)
{
    Q_UNUSED(w)
}

void LoginOptionsModule::active()
{
    m_loginOptionsWidget = new LoginOptionsWidget;
    m_loginOptionsWidget->setVisible(false);
    connect(m_loginOptionsWidget, &LoginOptionsWidget::requestShowFingerDetail, this, &LoginOptionsModule::showFingerPage);
    connect(m_loginOptionsWidget, &LoginOptionsWidget::requestShowFaceIdDetail, this, &LoginOptionsModule::showFaceidPage);
    connect(m_loginOptionsWidget, &LoginOptionsWidget::requestShowIrisDetail, this, &LoginOptionsModule::showIrisPage);
    m_frameProxy->pushWidget(this, m_loginOptionsWidget);
    m_loginOptionsWidget->setVisible(true);
    m_loginOptionsWidget->showDefaultWidget();
}

int LoginOptionsModule::load(const QString &path)
{
    if (m_loginOptionsWidget)
        active();
    return m_loginOptionsWidget->showPath(path);
}

QStringList LoginOptionsModule::availPage() const
{
    QStringList availList;
    availList << "Finger Detail";
    return availList;
}

void LoginOptionsModule::addChildPageTrans() const
{
    //todo(guoyao):请在此添加子页面菜单的翻译，参考其他模块
}

void LoginOptionsModule::showFingerPage()
{
    FingerDetailWidget *w = new FingerDetailWidget;
    w->setVisible(false);
    w->setFingerModel(m_fingerModel);
    connect(w, &FingerDetailWidget::requestAddThumbs, m_fingerWorker, &FingerWorker::tryEnroll);
    connect(w, &FingerDetailWidget::requestStopEnroll, m_fingerWorker, &FingerWorker::stopEnroll);
    connect(w, &FingerDetailWidget::requestDeleteFingerItem, m_fingerWorker, &FingerWorker::deleteFingerItem);
    connect(w, &FingerDetailWidget::requestRenameFingerItem, m_fingerWorker, &FingerWorker::renameFingerItem);
    connect(w, &FingerDetailWidget::noticeEnrollCompleted, m_fingerWorker, &FingerWorker::refreshUserEnrollList);

    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}

void LoginOptionsModule::showFaceidPage()
{
    FaceidDetailWidget *w = new FaceidDetailWidget(m_charaMangerModel);
    connect(w, &FaceidDetailWidget::requestEntollStart, m_charaMangerWorker, &CharaMangerWorker::entollStart);
    connect(w, &FaceidDetailWidget::requestStopEnroll, m_charaMangerWorker, &CharaMangerWorker::stopEnroll);
    connect(w, &FaceidDetailWidget::requestDeleteFaceItem, m_charaMangerWorker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &FaceidDetailWidget::requestRenameFaceItem, m_charaMangerWorker, &CharaMangerWorker::renameCharaItem);
    connect(w, &FaceidDetailWidget::noticeEnrollCompleted, m_charaMangerWorker, &CharaMangerWorker::refreshUserEnrollList);
    m_frameProxy->pushWidget(this, w);
}

void LoginOptionsModule::showIrisPage()
{
    IrisDetailWidget *w = new IrisDetailWidget(m_charaMangerModel);
//    connect(w, &IrisDetailWidget::requestAddIris, this, &LoginOptionsModule::onShowAddIris);
    connect(w, &IrisDetailWidget::requestEntollStart, m_charaMangerWorker, &CharaMangerWorker::entollStart);
    connect(w, &IrisDetailWidget::requestStopEnroll, m_charaMangerWorker, &CharaMangerWorker::stopEnroll);
    connect(w, &IrisDetailWidget::requestDeleteIrisItem, m_charaMangerWorker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &IrisDetailWidget::requestRenameIrisItem, m_charaMangerWorker, &CharaMangerWorker::renameCharaItem);
    connect(w, &IrisDetailWidget::noticeEnrollCompleted, m_charaMangerWorker, &CharaMangerWorker::refreshUserEnrollList);
    m_frameProxy->pushWidget(this, w);
}

void LoginOptionsModule::onShowAddIris(const QString &driverName, const int &charaType, const QString &charaName)
{
    // 第一次进入添加虹膜对话框
    if (!m_pMainWindow->isEnabled())
        return;

    AddIrisInfoDialog *irisDlg = new AddIrisInfoDialog(m_charaMangerModel);
    connect(m_charaMangerWorker, &CharaMangerWorker::tryStartInputIris, irisDlg, &AddIrisInfoDialog::refreshInfoStatusDisplay);

    connect(irisDlg, &AddIrisInfoDialog::requestStopEnroll, m_charaMangerWorker, &CharaMangerWorker::stopEnroll);
    connect(irisDlg, &AddIrisInfoDialog::requesetCloseDlg, irisDlg, [ = ] {
        onSetMainWindowEnabled(true);
        irisDlg->deleteLater();
    });

    // 点击下一步开始录入
    connect(irisDlg, &AddIrisInfoDialog::requestInputIris, m_charaMangerWorker, [ = ]{
        m_charaMangerWorker->entollStart(driverName, charaType, charaName);
    });

    onSetMainWindowEnabled(false);
    irisDlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    irisDlg->exec();
}

LoginOptionsModule::~LoginOptionsModule()
{
}

void LoginOptionsModule::onSetMainWindowEnabled(const bool isEnabled)
{
    if (m_pMainWindow)
        m_pMainWindow->setEnabled(isEnabled);
}

void LoginOptionsModule::initSearchData()
{
    //todo(guoyao):请在此添加三级页面的翻译，参考其他模块
}
