#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>

#include <libdui/dseparatorhorizontal.h>

#include "datetime.h"

DUI_USE_NAMESPACE

Datetime::Datetime() :
    QObject(),
    m_frame(new QFrame)
{
    initBackend();
    initUI();
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";
    delete m_frame;
}

void Datetime::initBackend()
{

}

void Datetime::initUI()
{

}

QFrame* Datetime::getContent()
{
    return m_frame;
}
