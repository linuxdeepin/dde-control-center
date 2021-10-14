#include "loginoptionsmodule.h"
#include "loginoptionswidget.h"
#include "fingerdetailwidget.h"
#include "addfingedialog.h"
#include "modules/authentication/fingerworker.h"
#include "modules/authentication/fingermodel.h"

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
    m_frameProxy->pushWidget(this, m_loginOptionsWidget);
    m_loginOptionsWidget->setVisible(true);
    m_loginOptionsWidget->showDefaultWidget();
}

int LoginOptionsModule::load(const QString &path)
{
    return -1;
}

QStringList LoginOptionsModule::availPage() const
{
    QStringList availList;
    availList << "Finger Detail";
    return availList;
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

    m_fingerWorker->tryEnroll(name, thumb);
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
}

LoginOptionsModule::~LoginOptionsModule()
{

}

void LoginOptionsModule::onSetMainWindowEnabled(const bool isEnabled)
{
    if (m_pMainWindow)
        m_pMainWindow->setEnabled(isEnabled);
}
