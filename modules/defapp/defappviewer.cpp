#include "defappviewer.h"

DefAppViewer::DefAppViewer() {
    m_mainlayout = new QVBoxLayout;
    m_mainwidget = new QWidget;
    m_mainwidget->setLayout(m_mainlayout);
    QWidget *m_defAppViewer = defappDetail();
    setTitle("Set Default Applications");
    setContent(m_defAppViewer);
}

//下级菜单，该函数会初始化界面
QWidget* DefAppViewer::defappDetail() {
    m_modBrowser = new DefCategoryWidget(tr("Browser"));
    m_modMail = new DefCategoryWidget(tr("Mail"));
    m_modText = new DefCategoryWidget(tr("Text"));
    m_modMusic = new DefCategoryWidget(tr("Music"));
    m_modVideo = new DefCategoryWidget(tr("Video"));
    m_modPicture = new DefCategoryWidget(tr("Picture"));
    m_modTerminal = new DefCategoryWidget(tr("Terminal"));
    m_modCDAudio = new DefCategoryWidget(tr("CD Audio"));
    m_modCDAudio->hide();
    m_modDVDVideo = new DefCategoryWidget(tr("DVD Video"));
    m_modDVDVideo->hide();
    m_modMusicPlayer = new DefCategoryWidget(tr("Music Player"));
    m_modMusicPlayer->hide();
    m_modCamera = new DefCategoryWidget(tr("Camera"));
    m_modCamera->hide();
    m_modSoftware = new DefCategoryWidget(tr("Software"));
    m_modSoftware->hide();

    m_switchWidget = new dcc::SwitchWidget();
    connect(m_switchWidget, &dcc::SwitchWidget::checkedChanegd, this, &DefAppViewer::autoOpenChanged);

    QPushButton *button = new QPushButton("Reset");
    connect(button,&QPushButton::clicked,this,&DefAppViewer::Reset);

    m_mainlayout->addWidget(button);
    m_mainlayout->addWidget(m_modBrowser);
    m_mainlayout->addWidget(m_modMail);
    m_mainlayout->addWidget(m_modText);
    m_mainlayout->addWidget(m_modMusic);
    m_mainlayout->addWidget(m_modVideo);
    m_mainlayout->addWidget(m_modPicture);
    m_mainlayout->addWidget(m_modTerminal);
    m_mainlayout->addWidget(m_switchWidget);
    m_mainlayout->addWidget(m_modCDAudio);
    m_mainlayout->addWidget(m_modDVDVideo);
    m_mainlayout->addWidget(m_modMusicPlayer);
    m_mainlayout->addWidget(m_modCamera);
    m_mainlayout->addWidget(m_modSoftware);

    connect(m_modBrowser,  &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modMail,     &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modText,     &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modMusic,    &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modVideo,    &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modPicture,  &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modTerminal, &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);

    connect(m_modCDAudio, &DefCategoryWidget::setDefaultApp,     this, &DefAppViewer::SetDefaultApp);
    connect(m_modDVDVideo, &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modCamera, &DefCategoryWidget::setDefaultApp,      this, &DefAppViewer::SetDefaultApp);
    connect(m_modSoftware, &DefCategoryWidget::setDefaultApp,    this, &DefAppViewer::SetDefaultApp);


    connect(m_modBrowser,  &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modMail,     &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modText,     &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modMusic,    &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modVideo,    &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modPicture,  &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modTerminal, &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);

    connect(m_modCDAudio, &DefCategoryWidget::addUserApp,     this, &DefAppViewer::AddUserApp);
    connect(m_modDVDVideo, &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::addUserApp, this, &DefAppViewer::AddUserApp);
    connect(m_modCamera, &DefCategoryWidget::addUserApp,      this, &DefAppViewer::AddUserApp);
    connect(m_modSoftware, &DefCategoryWidget::addUserApp,    this, &DefAppViewer::AddUserApp);

    connect(m_modBrowser,  &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modMail,     &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modText,     &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modMusic,    &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modVideo,    &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modPicture,  &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modTerminal, &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);

    connect(m_modCDAudio, &DefCategoryWidget::delUserApp,     this, &DefAppViewer::DelUserApp);
    connect(m_modDVDVideo, &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);
    connect(m_modMusicPlayer, &DefCategoryWidget::delUserApp, this, &DefAppViewer::DelUserApp);
    connect(m_modCamera, &DefCategoryWidget::delUserApp,      this, &DefAppViewer::DelUserApp);
    connect(m_modSoftware, &DefCategoryWidget::delUserApp,    this, &DefAppViewer::DelUserApp);


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


    return m_mainwidget;
}

void DefAppViewer::setModel(DefAppModel * const model) {
    connect(model, &DefAppModel::AutoOpenChanged, this, &DefAppViewer::setAutoChanged);
    setAutoChanged(model->setAutoOpen());

    m_modBrowser->setCategory(     model->getCategory("Browser"));
    m_modMail->setCategory(        model->getCategory("Mail"));
    m_modText->setCategory(        model->getCategory("Text"));
    m_modMusic->setCategory(       model->getCategory("Music"));
    m_modVideo->setCategory(       model->getCategory("Video"));
    m_modPicture->setCategory(     model->getCategory("Picture"));
    m_modTerminal->setCategory(    model->getCategory("Terminal"));
    m_modCDAudio->setCategory(     model->getCategory("CD_Audio"));
    m_modDVDVideo->setCategory(    model->getCategory("DVD_Video"));
    m_modMusicPlayer->setCategory( model->getCategory("MusicPlayer"));
    m_modCamera->setCategory(      model->getCategory("Camera"));
    m_modSoftware->setCategory(    model->getCategory("Software"));
}

void DefAppViewer::setAutoChanged(const bool visible) {
    qDebug() << "reset visible to " << visible;
    m_modCDAudio->setVisible(visible);
    m_modDVDVideo->setVisible(visible);
    m_modMusicPlayer->setVisible(visible);
    m_modCamera->setVisible(visible);
    m_modSoftware->setVisible(visible);
    m_switchWidget->setChecked(visible);
}

