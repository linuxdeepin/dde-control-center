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
    m_modCDAudio = new DefCategoryWidget(tr("CD Audio"), this);
    m_modCDAudio->hide();
    m_modDVDVideo = new DefCategoryWidget(tr("DVD Video"), this);
    m_modDVDVideo->hide();
    m_modMusicPlayer = new DefCategoryWidget(tr("Music Player"), this);
    m_modMusicPlayer->hide();
    m_modCamera = new DefCategoryWidget(tr("Camera"), this);
    m_modCamera->hide();
    m_modSoftware = new DefCategoryWidget(tr("Software"), this);
    m_modSoftware->hide();

    QVBoxLayout *switchLayout = new QVBoxLayout;
    SettingsGroup *group = new SettingsGroup;
    m_switchWidget = new SwitchWidget(tr("Autoplay"));
    connect(m_switchWidget, &SwitchWidget::checkedChanged, this, &DefAppViewer::autoOpenChanged);
    group->appendItem(m_switchWidget);
    switchLayout->addWidget(group);
    switchLayout->setMargin(0);

    mainlayout->addWidget(m_modBrowser);
    mainlayout->addWidget(m_modMail);
    mainlayout->addWidget(m_modText);
    mainlayout->addWidget(m_modMusic);
    mainlayout->addWidget(m_modVideo);
    mainlayout->addWidget(m_modPicture);
    mainlayout->addWidget(m_modTerminal);
    mainlayout->addLayout(switchLayout);
    mainlayout->addWidget(m_modCDAudio);
    mainlayout->addWidget(m_modDVDVideo);
    mainlayout->addWidget(m_modMusicPlayer);
    mainlayout->addWidget(m_modCamera);
    mainlayout->addWidget(m_modSoftware);

    connect(m_modBrowser,  &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modMail,     &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modText,     &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modMusic,    &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modVideo,    &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modPicture,  &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modTerminal, &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);

    connect(m_modCDAudio, &DefCategoryWidget::requestSetDefaultApp,     this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modDVDVideo, &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::requestSetDefaultApp, this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modCamera, &DefCategoryWidget::requestSetDefaultApp,      this, &DefAppViewer::requestSetDefaultApp);
    connect(m_modSoftware, &DefCategoryWidget::requestSetDefaultApp,    this, &DefAppViewer::requestSetDefaultApp);

    connect(m_modBrowser,  &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modMail,     &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modText,     &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modMusic,    &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modVideo,    &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modPicture,  &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modTerminal, &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);

    connect(m_modCDAudio, &DefCategoryWidget::requestAddUserApp,     this, &DefAppViewer::requestAddUserApp);
    connect(m_modDVDVideo, &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::requestAddUserApp, this, &DefAppViewer::requestAddUserApp);
    connect(m_modCamera, &DefCategoryWidget::requestAddUserApp,      this, &DefAppViewer::requestAddUserApp);
    connect(m_modSoftware, &DefCategoryWidget::requestAddUserApp,    this, &DefAppViewer::requestAddUserApp);

    connect(m_modBrowser,  &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modMail,     &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modText,     &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modMusic,    &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modVideo,    &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modPicture,  &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modTerminal, &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);

    connect(m_modCDAudio, &DefCategoryWidget::requestDelUserApp,     this, &DefAppViewer::requestDelUserApp);
    connect(m_modDVDVideo, &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::requestDelUserApp, this, &DefAppViewer::requestDelUserApp);
    connect(m_modCamera, &DefCategoryWidget::requestDelUserApp,      this, &DefAppViewer::requestDelUserApp);
    connect(m_modSoftware, &DefCategoryWidget::requestDelUserApp,    this, &DefAppViewer::requestDelUserApp);


    connect(m_modBrowser,  &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modMail,     &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modText,     &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modMusic,    &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modVideo,    &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modPicture,  &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modTerminal, &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);

    connect(m_modCDAudio, &DefCategoryWidget::requestFrameAutoHide,     this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modDVDVideo, &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modMusicPlayer, &DefCategoryWidget::requestFrameAutoHide, this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modCamera, &DefCategoryWidget::requestFrameAutoHide,      this, &DefAppViewer::requestFrameAutoHide);
    connect(m_modSoftware, &DefCategoryWidget::requestFrameAutoHide,    this, &DefAppViewer::requestFrameAutoHide);


    return mainwidget;
}

void DefAppViewer::setModel(DefAppModel *const model)
{
    connect(model, &DefAppModel::AutoOpenChanged, this, &DefAppViewer::setAutoChanged);
    setAutoChanged(model->setAutoOpen());

    m_modBrowser->setCategory(model->getModBrowser());
    m_modMail->setCategory(model->getModMail());
    m_modText->setCategory(model->getModText());
    m_modMusic->setCategory(model->getModMusic());
    m_modVideo->setCategory(model->getModVideo());
    m_modPicture->setCategory(model->getModPicture());
    m_modTerminal->setCategory(model->getModTerminal());
    m_modCDAudio->setCategory(model->getModCDAudio());
    m_modDVDVideo->setCategory(model->getModDVDVideo());
    m_modMusicPlayer->setCategory(model->getModMusicPlayer());
    m_modCamera->setCategory(model->getModCamera());
    m_modSoftware->setCategory(model->getModSoftware());
}

void DefAppViewer::setAutoChanged(const bool visible)
{
    qDebug() << "reset visible to " << visible;
    m_modCDAudio->setVisible(visible);
    m_modDVDVideo->setVisible(visible);
    m_modMusicPlayer->setVisible(visible);
    m_modCamera->setVisible(visible);
    m_modSoftware->setVisible(visible);
    m_switchWidget->setChecked(visible);
}

