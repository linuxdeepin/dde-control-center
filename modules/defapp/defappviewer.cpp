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
    //以后删除title，使用SettingsGroup的head
    m_modBrowser = new DefCategoryWidget;
    m_modBrowser->setTitle(tr("Browser"));
    m_modMail = new DefCategoryWidget;
    m_modMail->setTitle(tr("Mail"));
    m_modText = new DefCategoryWidget;
    m_modText->setTitle(tr("Text"));
    m_modMusic = new DefCategoryWidget;
    m_modMusic->setTitle(tr("Music"));
    m_modVideo = new DefCategoryWidget;
    m_modVideo->setTitle(tr("Video"));
    m_modPicture = new DefCategoryWidget;
    m_modPicture->setTitle(tr("Picture"));
    m_modTerminal = new DefCategoryWidget;
    m_modTerminal->setTitle(tr("Terminal"));
    m_modCDAudio = new DefCategoryWidget;
    m_modCDAudio->hide();
    m_modCDAudio->setTitle(tr("CD Audio"));
    m_modDVDVideo = new DefCategoryWidget;
    m_modDVDVideo->hide();
    m_modDVDVideo->setTitle(tr("DVD Video"));
    m_modMusicPlayer = new DefCategoryWidget;
    m_modMusicPlayer->hide();
    m_modMusicPlayer->setTitle(tr("Music Player"));
    m_modCamera = new DefCategoryWidget;
    m_modCamera->hide();
    m_modCamera->setTitle(tr("Camera"));
    m_modSoftware = new DefCategoryWidget;
    m_modSoftware->hide();
    m_modSoftware->setTitle(tr("Software"));

    m_switchWidget = new dcc::SwitchWidget();
    connect(m_switchWidget, &dcc::SwitchWidget::checkedChanegd, this, &DefAppViewer::autoOpenChanged);

    QPushButton *button = new QPushButton("Reset");
    connect(button,&QPushButton::clicked,this,&DefAppViewer::Reset);

    QPushButton *editButton = new QPushButton("Edit");
    connect(editButton, &QPushButton::clicked, this ,&DefAppViewer::onEdit);
    m_mainlayout->addWidget(editButton);
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

    connect(m_modBrowser,  &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modMail,     &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modText,     &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modMusic,    &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modVideo,    &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modPicture,  &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);
    connect(m_modTerminal, &DefCategoryWidget::setDefaultApp, this, &DefAppViewer::SetDefaultApp);

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

void DefAppViewer::onEdit()
{
    for (int cc = m_mainlayout->layout()->count()-1; cc >= 0; --cc) {
        QLayoutItem *it = m_mainlayout->layout()->itemAt(cc);
        DefCategoryWidget *orderHistory = qobject_cast<DefCategoryWidget *>(it->widget());
        if (orderHistory != 0)
        {
            orderHistory->onEdit();
        }
    }
}
