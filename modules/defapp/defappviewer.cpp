#include "defappviewer.h"
#include "defcategorywidget.h"
#include "switchwidget.h"
#include "defappmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;

DefAppViewer::DefAppViewer(QWidget *parent)
    : ContentWidget(parent)
{
    QWidget *m_defAppViewer = defappDetail();
    setTitle(tr("Default Applications"));
    m_defAppViewer->setObjectName("DefAppViewer");
    setContent(m_defAppViewer);
}

//下级菜单，该函数会初始化界面
QWidget *DefAppViewer::defappDetail()
{
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QWidget     *mainwidget = new QWidget;
    mainwidget->setLayout(mainlayout);
    mainlayout->setMargin(0);
    mainlayout->setSpacing(20);

    m_modBrowser = new DefCategoryWidget(tr("Browser"), this);
    m_modMail = new DefCategoryWidget(tr("Mail"), this);
    m_modText = new DefCategoryWidget(tr("Text"), this);
    m_modMusic = new DefCategoryWidget(tr("Music"), this);
    m_modVideo = new DefCategoryWidget(tr("Video"), this);
    m_modPicture = new DefCategoryWidget(tr("Picture"), this);
    m_modTerminal = new DefCategoryWidget(tr("Terminal"), this);

    mainlayout->addSpacing(10);
    mainlayout->addWidget(m_modBrowser);
    mainlayout->addWidget(m_modMail);
    mainlayout->addWidget(m_modText);
    mainlayout->addWidget(m_modMusic);
    mainlayout->addWidget(m_modVideo);
    mainlayout->addWidget(m_modPicture);
    mainlayout->addWidget(m_modTerminal);

    connect(m_modBrowser,  &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modMail,     &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modText,     &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modMusic,    &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modVideo,    &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modPicture,  &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modTerminal, &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);

    connect(m_modBrowser,  &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modMail,     &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modText,     &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modMusic,    &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modVideo,    &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modPicture,  &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modTerminal, &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);

    connect(m_modBrowser,  &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modMail,     &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modText,     &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modMusic,    &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modVideo,    &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modPicture,  &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modTerminal, &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);

    connect(m_modBrowser,  &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modMail,     &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modText,     &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modMusic,    &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modVideo,    &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modPicture,  &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);
    connect(m_modTerminal, &DefCategoryWidget::requestCreateFile,    this, &DefAppViewer::requestCreateFile);

    return mainwidget;
}

void DefAppViewer::setModel(DefAppModel *const model)
{
    m_modBrowser->setCategory(model->getModBrowser());
    m_modMail->setCategory(model->getModMail());
    m_modText->setCategory(model->getModText());
    m_modMusic->setCategory(model->getModMusic());
    m_modVideo->setCategory(model->getModVideo());
    m_modPicture->setCategory(model->getModPicture());
    m_modTerminal->setCategory(model->getModTerminal());
}
