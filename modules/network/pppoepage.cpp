#include "pppoepage.h"
#include "settingsgroup.h"
#include "networkmodel.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::network;

PppoePage::PppoePage(QWidget *parent) : ContentWidget(parent)
{
    setTitle(tr("PPP"));
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    qDebug() << model->pppoes();
}
