#include <QVBoxLayout>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

#include "titleandwidget.h"

DUI_USE_NAMESPACE

TitleAndWidget::TitleAndWidget(QWidget *w, const QString &title, QWidget *parent) :
    QLabel(title, parent)
{
    D_THEME_INIT_WIDGET(TitleAndWidget);

    setAlignment(Qt::AlignLeft|Qt::AlignTop);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addSpacing(15);
    layout->addWidget(w, 0, Qt::AlignHCenter|Qt::AlignTop);

    setLayout(layout);
}
