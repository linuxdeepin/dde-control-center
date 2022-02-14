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
#include "window/gsettingwatcher.h"

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
    GSettingWatcher::instance()->insertState("authenticationFinger");
    GSettingWatcher::instance()->insertState("authenticationFace");
    GSettingWatcher::instance()->insertState("authenticationIris");
}

void LoginOptionsModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    m_fingerModel = new FingerModel(this);
    m_fingerWorker = new FingerWorker(m_fingerModel);
    m_fingerModel->moveToThread(qApp->thread());
    m_fingerWorker->moveToThread(qApp->thread());

    m_charaMangerModel = new CharaMangerModel(this);
    m_charaMangerWorker = new CharaMangerWorker(m_charaMangerModel);
    m_charaMangerModel->moveToThread(qApp->thread());
    m_charaMangerWorker->moveToThread(qApp->thread());

    addChildPageTrans();
    initSearchData();
}

void LoginOptionsModule::initialize()
{

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
    connect(m_loginOptionsWidget, &LoginOptionsWidget::requestUpdateSecondMenu, this, [=](bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop) {
            m_frameProxy->popWidget(this);
        }
        m_loginOptionsWidget->showDefaultWidget();
    });

    m_frameProxy->pushWidget(this, m_loginOptionsWidget);
    m_loginOptionsWidget->setVisible(true);
    m_loginOptionsWidget->showDefaultWidget();
}

int LoginOptionsModule::load(const QString &path)
{
    if (!m_loginOptionsWidget)
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
    if (m_frameProxy != nullptr) {
        //authentication
        m_frameProxy->addChildPageTrans("Fingerprint", tr("Fingerprint"));
        m_frameProxy->addChildPageTrans("Face", tr("Face"));
        m_frameProxy->addChildPageTrans("Iris", tr("Iris"));
    }
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
    connect(w, &IrisDetailWidget::requestEntollStart, m_charaMangerWorker, &CharaMangerWorker::entollStart);
    connect(w, &IrisDetailWidget::requestStopEnroll, m_charaMangerWorker, &CharaMangerWorker::stopEnroll);
    connect(w, &IrisDetailWidget::requestDeleteIrisItem, m_charaMangerWorker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &IrisDetailWidget::requestRenameIrisItem, m_charaMangerWorker, &CharaMangerWorker::renameCharaItem);
    connect(w, &IrisDetailWidget::noticeEnrollCompleted, m_charaMangerWorker, &CharaMangerWorker::refreshUserEnrollList);
    m_frameProxy->pushWidget(this, w);
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
    const QString& module = displayName();
    const QString& finger = tr("Fingerprint");
    const QString& face = tr("Face");
    const QString& iris = tr("Iris");

    const QStringList& gsSecList {
        "authenticationFinger",
        "authenticationFace",
        "authenticationIris"
    };

    static QMap<QString, bool> gsettingsMap;
    auto func_is_visible = [ = ] (const QString &gsettings, QString state = ""){
        if (gsettings == "") {
            return false;
        }

        bool ret = false;
        if (state == "") {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }

        gsettingsMap.insert(gsettings, ret);

        return ret;
    };

    auto func_finger_changed = [ = ] () {
        bool bFinger = func_is_visible("authenticationFinger");
        m_frameProxy->setWidgetVisible(module, finger, bFinger);
    };

    auto func_face_changed = [ = ] () {
        bool bFace = func_is_visible("authenticationFace");
        m_frameProxy->setWidgetVisible(module, face, bFace);
    };

    auto func_iris_changed = [ = ] () {
        bool bIris = func_is_visible("authenticationIris");
        m_frameProxy->setWidgetVisible(module, iris, bIris);
    };

    auto func_process_all = [ = ]() {
        func_finger_changed();
        func_face_changed();
        func_iris_changed();
    };

    // 追踪设备是否存在
    connect(m_fingerModel, &FingerModel::vaildChanged, [=](){
        func_finger_changed();
    });

    connect(m_charaMangerModel, &CharaMangerModel::vaildFaceDriverChanged, [=](){
        func_face_changed();
    });

    connect(m_charaMangerModel, &CharaMangerModel::vaildIrisDriverChanged, [=](){
        func_iris_changed();
    });

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if (state != ""  && (!gsettingsMap.contains(gsetting))) {
            return;
        }
        bool bGsMap = gsettingsMap.value(gsetting);
        if (gsSecList.contains(gsetting) && GSettingWatcher::instance()->get(gsetting).toBool() == bGsMap) {
            return;
        }

        if ("Fingerprint" == gsetting) {
            func_finger_changed();
        } else if ("Face" == gsetting) {
            func_face_changed();
        } else if ("Iris" == gsetting){
            func_iris_changed();
        }


        m_frameProxy->updateSearchData(module);
    });
    func_process_all();
}
