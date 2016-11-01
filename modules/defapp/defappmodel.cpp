#include "defappmodel.h"


DefAppModel::DefAppModel(QObject *parent) {
    Q_UNUSED(parent);
    m_modBrowser = new Category(this);
    m_modBrowser->setCategory("Browser");
    m_modMail = new Category(this);
    m_modMail->setCategory("Mail");
    m_modText = new Category(this);
    m_modText->setCategory("Text");
    m_modMusic = new Category(this);
    m_modMusic->setCategory("Music");
    m_modVideo = new Category(this);
    m_modVideo->setCategory("Video");
    m_modPicture = new Category(this);
    m_modPicture->setCategory("Picture");
    m_modTerminal = new Category(this);
    m_modTerminal->setCategory("Terminal");

    m_modCDAudio = new Category(this);
    m_modCDAudio->setCategory("CD_Audio");
    m_modDVDVideo = new Category(this);
    m_modDVDVideo->setCategory("DVD_Video");
    m_modMusicPlayer = new Category(this);
    m_modMusicPlayer->setCategory("MusicPlayer");
    m_modCamera = new Category(this);
    m_modCamera->setCategory("Camera");
    m_modSoftware = new Category(this);
    m_modSoftware->setCategory("Software");


    m_categoryList.append(m_modBrowser);
    m_categoryList.append(m_modMail);
    m_categoryList.append(m_modText);
    m_categoryList.append(m_modMusic);
    m_categoryList.append(m_modVideo);
    m_categoryList.append(m_modPicture);
    m_categoryList.append(m_modTerminal);
    m_categoryList.append(m_modCDAudio);
    m_categoryList.append(m_modDVDVideo);
    m_categoryList.append(m_modMusicPlayer);
    m_categoryList.append(m_modCamera);
    m_categoryList.append(m_modSoftware);

}

Category *DefAppModel::getCategory(const QString &category) {
    for (int cc = m_categoryList.count()-1; cc >= 0; --cc) {
        Category *orderHistory = qobject_cast<Category *>(m_categoryList.at(cc));
        if (orderHistory != 0) {
            if(orderHistory->getName() == category) {
                return orderHistory;
            }
        }
    }
    return nullptr;
}

void DefAppModel::setAppList(const QString &category, QList<QStringList> &list) {
    if (!getCategory(category)) {
        return;
    }
    getCategory(category)->setList(list);
}

void DefAppModel::setDefault(const QString &category, const QString &id) {
    if (!getCategory(category)) {
        return;
    }
    getCategory(category)->setDefault(id);
}

void DefAppModel::setAutoOpen(const bool state) {
    qDebug()<<"Model state :"<<state;
    if (m_autoOpen != state) {
        m_autoOpen = state;
        emit AutoOpenChanged(m_autoOpen);
    }
}

Category::Category(QObject *parent) {
    Q_UNUSED(parent);
}

void Category::setDefault(const QString &id)
{
    if (m_id != id){
        m_id = id;
        emit defaultChanged(id);
    }
}

void Category::setList(const QList<QStringList> &list) {
    //对比list，进行内容的对比，并发出信号，然后保存最新的list
    if (m_list != list) {
        if (m_list.isEmpty()){
            m_list = list;
        } else {
            if (m_list.count() > list.count()) {
                //如果原数据比新数据大，则删除了部分程序，发出删除的信号
                QMap<QStringList,int> listmap;
                for (QStringList s : m_list) {
                    listmap.insert(s,1);
                }
                for (QStringList s : list) {
                    int cc  = listmap.value(s);
                    if(cc != 0) {
                        listmap.insert(s,++cc);
                        continue;
                    }
                    listmap.insert(s,1);
                }
                QMap<QStringList,int>::const_iterator it = listmap.constBegin();
                while (it != listmap.constEnd()) {
                    if (it.value() == 1) {
                        emit itemsRemoved(it.key());
                    }
                    ++it;
                }
                qDebug()<<"delete "<<it.key();
            } else {
                QMap<QStringList,int> listmap;
                for (QStringList s : list) {
                    listmap.insert(s,1);
                }
                for (QStringList s : m_list) {
                    int cc  = listmap.value(s);
                    if (cc != 0) {
                        listmap.insert(s,++cc);
                        continue;
                    }
                    listmap.insert(s,1);
                }
                QMap<QStringList,int>::const_iterator it = listmap.constBegin();
                while (it != listmap.constEnd()) {
                    if (it.value() == 1) {
                        emit itemsAdded(it.key());
                    }
                    ++it;
                }
            }
            m_list = list;
        }
    }
}
