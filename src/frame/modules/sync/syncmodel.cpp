#include "syncmodel.h"

using namespace dcc;
using namespace dcc::sync;

SyncModel::SyncModel(QObject *parent) : QObject(parent)
{

}

void SyncModel::setUserinfo(const QVariantMap &userinfo)
{
    if (m_userinfo == userinfo) return;

    m_userinfo = userinfo;

    Q_EMIT userInfoChanged(userinfo);
}
