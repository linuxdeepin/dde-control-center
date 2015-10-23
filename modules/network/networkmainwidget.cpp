#include <QDebug>
#include <QVBoxLayout>

#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dlistwidget.h>

#include "moduleheader.h"
#include "constants.h"
#include "normallabel.h"
#include "imagenamebutton.h"
#include "genericlistitem.h"
#include "generaladdbutton.h"

#include "networkmainwidget.h"

NetworkMainWidget::NetworkMainWidget(QWidget *parent) :
    ScrollFrame(parent)
{
    initUI();
}

void NetworkMainWidget::initUI()
{
    ModuleHeader *header = new ModuleHeader(tr("Network"), false);
    QWidget *header_right_widget = new QWidget;
    QHBoxLayout *header_right_widget_layout = new QHBoxLayout(header_right_widget);
    GeneralAddButton *add_button = new GeneralAddButton;
    ImageNameButton *info_button = new ImageNameButton("info");

    header_right_widget_layout->setMargin(0);
    header_right_widget_layout->setSpacing(5);
    header_right_widget_layout->addWidget(add_button);
    header_right_widget_layout->addWidget(info_button);
    header->setRightContent(header_right_widget);

    setHeaderWidget(header);

    mainLayout()->addWidget(new DSeparatorHorizontal);
    mainLayout()->addStretch(1);
}
