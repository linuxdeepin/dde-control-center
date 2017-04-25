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

    Category *getModBrowser()     {return m_modBrowser;}
    Category *getModMail()        {return m_modMail;}
    Category *getModText()        {return m_modText;}
    Category *getModMusic()       {return m_modMusic;}
    Category *getModVideo()       {return m_modVideo;}
    Category *getModPicture()     {return m_modPicture;}
    Category *getModTerminal()    {return m_modTerminal;}

private:
    Category *m_modBrowser;
    Category *m_modMail;
    Category *m_modText;
    Category *m_modMusic;
    Category *m_modVideo;
    Category *m_modPicture;
    Category *m_modTerminal;
};
}
}



#endif // DEFAPPMODEL_H
