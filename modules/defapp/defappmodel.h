#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include <QObject>
#include <QDebug>
namespace dcc
{
namespace defapp
{
class Category;
class DefAppModel : public QObject
{
    Q_OBJECT
public:
    explicit DefAppModel(QObject *parent = 0);
    ~DefAppModel();
    inline bool setAutoOpen() {return m_autoOpen;}
    void setAutoOpen(const bool state);
    Category *getModBrowser()     {return m_modBrowser;}
    Category *getModMail()        {return m_modMail;}
    Category *getModText()        {return m_modText;}
    Category *getModMusic()       {return m_modMusic;}
    Category *getModVideo()       {return m_modVideo;}
    Category *getModPicture()     {return m_modPicture;}
    Category *getModTerminal()    {return m_modTerminal;}
    Category *getModCDAudio()     {return m_modCDAudio;}
    Category *getModDVDVideo()    {return m_modDVDVideo;}
    Category *getModMusicPlayer() {return m_modMusicPlayer;}
    Category *getModCamera()      {return m_modCamera;}
    Category *getModSoftware()    {return m_modSoftware;}

signals:
    void AutoOpenChanged(bool state);

private:
    bool m_autoOpen;
    Category *m_modBrowser;
    Category *m_modMail;
    Category *m_modText;
    Category *m_modMusic;
    Category *m_modVideo;
    Category *m_modPicture;
    Category *m_modTerminal;
    Category *m_modCDAudio;
    Category *m_modDVDVideo;
    Category *m_modMusicPlayer;
    Category *m_modCamera;
    Category *m_modSoftware;
};
}
}



#endif // DEFAPPMODEL_H
