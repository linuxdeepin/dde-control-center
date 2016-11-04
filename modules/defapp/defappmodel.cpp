#include "defappmodel.h"
#include <string.h>

DefAppModel::DefAppModel(QObject *parent)
{
    Q_UNUSED(parent);
    Category *modBrowser = new Category(this);
    modBrowser->setCategory("Browser");
    Category *modMail = new Category(this);
    modMail->setCategory("Mail");
    Category *modText = new Category(this);
    modText->setCategory("Text");
    Category *modMusic = new Category(this);
    modMusic->setCategory("Music");
    Category *modVideo = new Category(this);
    modVideo->setCategory("Video");
    Category *modPicture = new Category(this);
    modPicture->setCategory("Picture");
    Category *modTerminal = new Category(this);
    modTerminal->setCategory("Terminal");

    Category *modCDAudio = new Category(this);
    modCDAudio->setCategory("CD_Audio");
    Category *modDVDVideo = new Category(this);
    modDVDVideo->setCategory("DVD_Video");
    Category *modMusicPlayer = new Category(this);
    modMusicPlayer->setCategory("MusicPlayer");
    Category *modCamera = new Category(this);
    modCamera->setCategory("Camera");
    Category *modSoftware = new Category(this);
    modSoftware->setCategory("Software");


    m_categoryList.append(modBrowser);
    m_categoryList.append(modMail);
    m_categoryList.append(modText);
    m_categoryList.append(modMusic);
    m_categoryList.append(modVideo);
    m_categoryList.append(modPicture);
    m_categoryList.append(modTerminal);
    m_categoryList.append(modCDAudio);
    m_categoryList.append(modDVDVideo);
    m_categoryList.append(modMusicPlayer);
    m_categoryList.append(modCamera);
    m_categoryList.append(modSoftware);

}

DefAppModel::~DefAppModel()
{

}

Category *DefAppModel::getCategory(const QString &category)
{
    for (int cc = m_categoryList.count() - 1; cc >= 0; --cc) {
        Category *orderHistory = qobject_cast<Category *>(m_categoryList.at(cc));
        if (orderHistory != 0) {
            if (orderHistory->getName() == category) {
                return orderHistory;
            }
        }
    }
    return nullptr;
}

void DefAppModel::setAppList(const QString &category, QList<QJsonObject> &list)
{
    Category *orderHistory = getCategory(category);
    if (!orderHistory) {
        return;
    }
    orderHistory->setappList(list);
}

void DefAppModel::setUserList(const QString &category, QList<QJsonObject> &list)
{
    Category *orderHistory = getCategory(category);
    if (!orderHistory) {
        return;
    }
    orderHistory->setuserList(list);
}

void DefAppModel::setDefault(const QString &category, const QString &id)
{
    Category *orderHistory = getCategory(category);
    if (!orderHistory) {
        return;
    }
    orderHistory->setDefault(id);
}

void DefAppModel::setAutoOpen(const bool state)
{
    qDebug() << "Model state :" << state;
    if (m_autoOpen != state) {
        m_autoOpen = state;
        emit AutoOpenChanged(m_autoOpen);
    }
}


Category::Category(QObject *parent)
{
    Q_UNUSED(parent);
}

void Category::setuserList(const QList<QJsonObject> &list)
{
    m_userlist = list;
    emit userItemChanged();
}

void Category::setappList(const QList<QJsonObject> &list)
{
    m_applist = list;
    emit itemsChanged();
}

void Category::setDefault(const QString &id)
{
    if (m_id != id) {
        m_id = id;
        emit defaultChanged(id);
    }
}
