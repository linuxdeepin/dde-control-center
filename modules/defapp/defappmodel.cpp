#include "defappmodel.h"
#include <string.h>
#include "model/category.h"
using namespace dcc;
using namespace dcc::defapp;
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
}


