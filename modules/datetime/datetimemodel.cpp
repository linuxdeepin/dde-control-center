#include "datetimemodel.h"

namespace dcc {
namespace datetime {

DatetimeModel::DatetimeModel(QObject *parent)
    :QObject(parent),
      m_ntp(false)
{

}

bool DatetimeModel::nTP() const
{
    return m_ntp;
}

void DatetimeModel::setNTP(bool ntp)
{
    m_ntp = ntp;

    emit NTPChanged(ntp);
}

}
}
