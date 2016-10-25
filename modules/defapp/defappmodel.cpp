#include "defappmodel.h"


DefAppModel::DefAppModel(QObject *parent)
{
    Q_UNUSED(parent);
}

void DefAppModel::setList(QMultiMap<QString, QStringList> List)
{
    m_appsList.clear();
    m_appsList = List;
    emit ListAppChanged(m_appsList);
}

void DefAppModel::setDefApp(QMultiMap<QString, QString> app)
{
    m_defapp.clear();
    m_defapp = app;
    emit DefaultAppsChanged(m_defapp);
}
