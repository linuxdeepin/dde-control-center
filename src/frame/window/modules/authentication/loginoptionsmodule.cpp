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

    m_facedlg = new FaceInfoDialog(m_charaMangerModel);
    m_addFaceInfodlg = new AddFaceInfoDialog(m_charaMangerModel);
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
    connect(w, &FingerDetailWidget::requestAddThumbs, this, &LoginOptionsModule::onShowAddThumb);
    connect(w, &FingerDetailWidget::requestDeleteFingerItem, m_fingerWorker, &FingerWorker::deleteFingerItem);
    connect(w, &FingerDetailWidget::requestRenameFingerItem, m_fingerWorker, &FingerWorker::renameFingerItem);
    connect(w, &FingerDetailWidget::noticeEnrollCompleted, m_fingerWorker, &FingerWorker::refreshUserEnrollList);

    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}

void LoginOptionsModule::showFaceidPage()
{
    FaceidDetailWidget *w = new FaceidDetailWidget(m_charaMangerModel);
    connect(w, &FaceidDetailWidget::requestAddFace, this, &LoginOptionsModule::onShowAddFace);
    connect(w, &FaceidDetailWidget::requestDeleteFaceItem, m_charaMangerWorker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &FaceidDetailWidget::requestRenameFaceItem, m_charaMangerWorker, &CharaMangerWorker::renameCharaItem);
    connect(w, &FaceidDetailWidget::noticeEnrollCompleted, m_charaMangerWorker, &CharaMangerWorker::refreshUserEnrollList);
    m_frameProxy->pushWidget(this, w);
}

void LoginOptionsModule::showIrisPage()
{
    IrisDetailWidget *w = new IrisDetailWidget(m_charaMangerModel);
    connect(w, &IrisDetailWidget::requestAddIris, this, &LoginOptionsModule::onShowAddIris);
    connect(w, &IrisDetailWidget::requestDeleteIrisItem, m_charaMangerWorker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &IrisDetailWidget::requestRenameIrisItem, m_charaMangerWorker, &CharaMangerWorker::renameCharaItem);
    connect(w, &IrisDetailWidget::noticeEnrollCompleted, m_charaMangerWorker, &CharaMangerWorker::refreshUserEnrollList);
    m_frameProxy->pushWidget(this, w);
}

// 显示添加指纹弹窗
void LoginOptionsModule::onShowAddThumb(const QString &name, const QString &thumb)
{
    AddFingeDialog *dlg = new AddFingeDialog(thumb);
    connect(dlg, &AddFingeDialog::requestEnrollThumb, m_fingerWorker, [=] {
        m_fingerWorker->tryEnroll(name, thumb);
    });
    connect(dlg, &AddFingeDialog::requestStopEnroll, m_fingerWorker, &FingerWorker::stopEnroll);
    connect(dlg, &AddFingeDialog::requesetCloseDlg, dlg, [=](const QString &userName) {
        m_fingerWorker->refreshUserEnrollList(userName);
        onSetMainWindowEnabled(true);
        dlg->deleteLater();
    });

    connect(m_fingerWorker, &FingerWorker::tryEnrollResult, dlg, [=] (FingerWorker::EnrollResult res) {
        // 第一次tryEnroll进入时显示添加指纹对话框
        if (m_pMainWindow->isEnabled()) {
            if (res == FingerWorker::Enroll_Success) {
                onSetMainWindowEnabled(false);
                m_fingerModel->resetProgress();
                dlg->setFingerModel(m_fingerModel);
                dlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
                dlg->setUsername(name);
                dlg->show();
                dlg->setFocus();
                dlg->activateWindow();
            } else {
                m_fingerWorker->stopEnroll(name);
                dlg->deleteLater();
            }
        } else {
            //　已经在添加指纹对话框中的Enroll处理
            if (res == FingerWorker::Enroll_AuthFailed) {
                onSetMainWindowEnabled(true);
                dlg->deleteLater();
            } else {
                dlg->setInitStatus();
            }
        }
    });

    m_fingerWorker->tryEnroll(name, thumb);
}

// 添加人脸弹框
void LoginOptionsModule::onShowAddFace(const QString &driverName, const int &charaType, const QString &charaName)
{
    // TODO: 人脸视频录入完成后， 将dlg弹出， 显示对应状态
    // 第一次进入添加人脸对话框
    if (!m_pMainWindow->isEnabled())
        return;

    connect(m_addFaceInfodlg, &AddFaceInfoDialog::requestStopEnroll, m_charaMangerWorker, &CharaMangerWorker::stopEnroll, Qt::UniqueConnection);
    disconnect(m_addFaceInfodlg, &AddFaceInfoDialog::requesetCloseDlg, m_addFaceInfodlg, nullptr);
    connect(m_addFaceInfodlg, &AddFaceInfoDialog::requesetCloseDlg, m_addFaceInfodlg, [this]{
        onSetMainWindowEnabled(true);
    });

    // 用户点击对话框开始录入
    disconnect(m_addFaceInfodlg, &AddFaceInfoDialog::requestShowFaceInfoDialog, this, nullptr);
    connect(m_addFaceInfodlg, &AddFaceInfoDialog::requestShowFaceInfoDialog, this, [=](){
        m_addFaceInfodlg->hide();
        onSetMainWindowEnabled(true);
        qDebug() << "connect(dlg, &AddFaceInfoDialog::requestShowFaceInfoDialog ";
        onShowAddFaceidVideo(driverName, charaType, charaName);
    });

    onSetMainWindowEnabled(false);
    m_addFaceInfodlg->responseEnrollInfoState(CharaMangerModel::AddInfoState::StartState, QString());
    m_addFaceInfodlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    m_addFaceInfodlg->show();
    m_addFaceInfodlg->setFocus();
    m_addFaceInfodlg->activateWindow();
}

// 添加人脸数据录入人脸视频对话框 ： 只处理开始录入
void LoginOptionsModule::onShowAddFaceidVideo(const QString &driverName, const int &charaType, const QString &charaName)
{
    // 开始录入人脸
    disconnect(m_facedlg, &FaceInfoDialog::requestCloseDlg, m_facedlg, nullptr);
    connect(m_facedlg, &FaceInfoDialog::requestCloseDlg, m_facedlg, [this](){
        m_charaMangerWorker->stopEnroll();
        onSetMainWindowEnabled(true);
    });

    // 开始录入就弹出  TODO:  处理拿到FD后的内容
    disconnect(m_charaMangerWorker, &CharaMangerWorker::tryStartInputFace, m_facedlg, nullptr);
    connect(m_charaMangerWorker, &CharaMangerWorker::tryStartInputFace, m_facedlg, [this](const int &facedf){
        onSetMainWindowEnabled(false);
        m_facedlg->faceInfoLabel()->createConnection(facedf);

        m_facedlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
        m_facedlg->exec();
        m_facedlg->setFocus();
        m_facedlg->activateWindow();
    });

    // TODO: FD
    m_charaMangerWorker->entollStart(driverName, charaType, charaName);
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
    if (m_facedlg) m_facedlg->deleteLater();
    if (m_addFaceInfodlg) m_addFaceInfodlg->deleteLater();
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
