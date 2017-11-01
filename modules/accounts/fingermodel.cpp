#include "fingermodel.h"

using namespace dcc;
using namespace dcc::accounts;

FingerModel::FingerModel(QObject *parent) : QObject(parent)
{
    m_isVaild = false;
}

bool FingerModel::isVaild() const
{
    return m_isVaild;
}

void FingerModel::setIsVaild(bool isVaild)
{
    if (m_isVaild == isVaild)
        return;

    m_isVaild = isVaild;

    emit vaildChanged(isVaild);
}

FingerModel::EnrollStatus FingerModel::enrollStatus() const
{
    return m_enrollStatus;
}

void FingerModel::setEnrollStatus(const EnrollStatus &enrollStatus)
{
    m_enrollStatus = enrollStatus;

    emit enrollStatusChanged(enrollStatus);
}

void FingerModel::addUserThumbs(const FingerModel::UserThumbs &thumbs)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList.at(i).username == thumbs.username) {
            m_thumbsList.removeAt(i);
            break;
        }
    }

    m_thumbsList << thumbs;

    emit thumbsListChanged(m_thumbsList);
}

void FingerModel::cleanUserThumbs(const QString &user)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList[i].username == user) {
            m_thumbsList.removeAt(i);
            emit thumbsListChanged(m_thumbsList);
            return;
        }
    }
}

QList<FingerModel::UserThumbs> FingerModel::thumbsList() const
{
    return m_thumbsList;
}
