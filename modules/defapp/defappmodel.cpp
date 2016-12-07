#include "defappmodel.h"
#include <string.h>

DefAppModel::DefAppModel(QObject *parent)
    :QObject(parent)
{
    m_modBrowser = new Category(this);
    m_modMail = new Category(this);
    m_modText = new Category(this);
    m_modMusic = new Category(this);
    m_modVideo = new Category(this);
    m_modPicture = new Category(this);
    m_modTerminal = new Category(this);
    m_modCDAudio = new Category(this);
    m_modDVDVideo = new Category(this);
    m_modMusicPlayer = new Category(this);
    m_modCamera = new Category(this);
    m_modSoftware = new Category(this);

}

DefAppModel::~DefAppModel()
{
    m_modBrowser->deleteLater();
    m_modMail->deleteLater();
    m_modText->deleteLater();
    m_modMusic->deleteLater();
    m_modVideo->deleteLater();
    m_modPicture->deleteLater();
    m_modTerminal->deleteLater();
    m_modCDAudio->deleteLater();
    m_modDVDVideo->deleteLater();
    m_modMusicPlayer->deleteLater();
    m_modCamera->deleteLater();
    m_modSoftware->deleteLater();
}

void DefAppModel::setAutoOpen(const bool state)
{
    qDebug() << "Model state :" << state;
    if (m_autoOpen != state) {
        m_autoOpen = state;
        emit AutoOpenChanged(m_autoOpen);
    }
}



