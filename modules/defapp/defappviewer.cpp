#include "defappviewer.h"

DefAppViewer::DefAppViewer() {
    m_mainlayout = new QVBoxLayout(this);
    m_mainwidget = new QWidget(this);
    QWidget *m_defAppViewer = defappDetail();
    setTitle("Set Default Applications");
    setContent(m_defAppViewer);
}

//下级菜单，该函数会初始化界面
QWidget* DefAppViewer::defappDetail() {
    //以后删除title，使用SettingsGroup的head
    m_modBrowser = new DefCategoryWidget;
    m_modBrowser->setTitle(tr("Browser"));
    m_modBrowser->setObjectName("Browser");
    m_modMail = new DefCategoryWidget;
    m_modMail->setTitle(tr("Mail"));
    m_modMail->setObjectName(tr("Mail"));
    m_modText = new DefCategoryWidget;
    m_modText->setTitle(tr("Text"));
    m_modText->setObjectName(tr("Text"));
    m_modMusic = new DefCategoryWidget;
    m_modMusic->setTitle(tr("Music"));
    m_modMusic->setObjectName(tr("Music"));
    m_modVideo = new DefCategoryWidget;
    m_modVideo->setTitle(tr("Video"));
    m_modVideo->setObjectName(tr("Video"));
    m_modPicture = new DefCategoryWidget;
    m_modPicture->setTitle(tr("Picture"));
    m_modPicture->setObjectName(tr("Picture"));
    m_modTerminal = new DefCategoryWidget;
    m_modTerminal->setTitle(tr("Terminal"));
    m_modTerminal->setObjectName(tr("Terminal"));
    m_modCDAudio = new DefCategoryWidget;
    m_modCDAudio->hide();
    m_modCDAudio->setTitle(tr("CD Audio"));
    m_modCDAudio->setObjectName(tr("CD_Audio"));
    m_modDVDVideo = new DefCategoryWidget;
    m_modDVDVideo->hide();
    m_modDVDVideo->setTitle(tr("DVD Video"));
    m_modDVDVideo->setObjectName(tr("DVD_Video"));
    m_modMusicPlayer = new DefCategoryWidget;
    m_modMusicPlayer->hide();
    m_modMusicPlayer->setTitle(tr("Music Player"));
    m_modMusicPlayer->setObjectName(tr("MusicPlayer"));
    m_modCamera = new DefCategoryWidget;
    m_modCamera->hide();
    m_modCamera->setTitle(tr("Camera"));
    m_modCamera->setObjectName(tr("Camera"));
    m_modSoftware = new DefCategoryWidget;
    m_modSoftware->hide();
    m_modSoftware->setTitle(tr("Software"));
    m_modSoftware->setObjectName(tr("Software"));

    dcc::SwitchWidget *m_switchWidget = new dcc::SwitchWidget();
    connect(m_switchWidget, &dcc::SwitchWidget::checkedChanegd, this, &DefAppViewer::setMediaOptionVisible);


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

    m_mainwidget->setLayout(m_mainlayout);

    return m_mainwidget;
}

void DefAppViewer::setModel(DefAppModel * const model)
{
    connect(model, &DefAppModel::ListAppChanged,     this, &DefAppViewer::createAppsList);
    connect(model, &DefAppModel::DefaultAppsChanged, this, &DefAppViewer::getDefaultApp);
    createAppsList(model->setList());
    getDefaultApp(model->setDefApp());
}

void DefAppViewer::getDefaultApp(QMap<QString,QString> app) {
    //需要循环设置
    QMap<QString,QString>::iterator map = app.begin();
    for(map; map != app.end(); ++map) {
        for (int cc = m_mainlayout->layout()->count()-1; cc >= 0; --cc) {
            QLayoutItem *it = m_mainlayout->layout()->itemAt(cc);
            DefCategoryWidget *orderHistory = qobject_cast<DefCategoryWidget *>(it->widget());
            if (orderHistory != 0)
            {
                //doing something for orderHistory
                if(orderHistory->objectName() == map.key()) {
                    orderHistory->onDefaultAppSet(map.value());
                }
            }
        }
    }
}
//name,id,category
void DefAppViewer::createAppsList(QMap<QString,QStringList> list) {
    // 使用objectName来添加数据
    QMap<QString,QStringList>::iterator map = list.begin();
    for(map; map != list.end(); ++map) {
        for (int cc = m_mainlayout->layout()->count()-1; cc >= 0; --cc) {
            QLayoutItem *it = m_mainlayout->layout()->itemAt(cc);
            DefCategoryWidget *orderHistory = qobject_cast<DefCategoryWidget *>(it->widget());
            if (orderHistory != 0)
            {
                //doing something for orderHistory
                if(orderHistory->objectName() == map.key()) {
                    orderHistory->setGeometry(0,0,0,0);
                    orderHistory->setFocusPolicy(Qt::NoFocus);
                    orderHistory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                    orderHistory->add(map.value()[0],map.value()[1],map.value()[2]);
                }
            }
        }
    }

}

void DefAppViewer::setMediaOptionVisible(const bool visible) {
    qDebug() << "reset visible to " << visible;
    m_modCDAudio->setVisible(visible);
    m_modDVDVideo->setVisible(visible);
    m_modMusicPlayer->setVisible(visible);
    m_modCamera->setVisible(visible);
    m_modSoftware->setVisible(visible);
}
