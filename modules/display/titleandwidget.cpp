#include <QVBoxLayout>
#include <QDebug>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

#include "titleandwidget.h"

DUI_USE_NAMESPACE

TitleAndWidget::TitleAndWidget(QWidget *w, const QString &title, QWidget *parent) :
    QLabel(title, parent)
{
    D_THEME_INIT_WIDGET(TitleAndWidget);

    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addSpacing(15);
    layout->addWidget(w, 0, Qt::AlignHCenter | Qt::AlignTop);
    layout->addWidget(new DSeparatorHorizontal);

    setLayout(layout);
}

void TitleAndWidget::setTitleVisible(bool arg)
{
    if (m_titleVisible == arg) {
        return;
    }

    m_titleVisible = arg;

    if (arg) {
        setText(m_title);
    } else {
        m_title = text();
        setText("");
    }
}
