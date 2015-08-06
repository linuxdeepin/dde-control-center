#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <libdui/dheaderline.h>
#include <libdui/libdui_global.h>
#include "grubbackground.h"
#include "dbusgrub.h"
#include <libdui/darrowlineexpand.h>

DUI_USE_NAMESPACE

class GrubWidget : public QFrame
{
public:
    explicit GrubWidget(QWidget *parent = 0);
    ~GrubWidget();

private:
    QVBoxLayout *m_layout;
    DHeaderLine *m_header;
    DArrowLineExpand *m_arrowDefaultBoot;
    DArrowLineExpand *m_arrowBootDelay;
    DArrowLineExpand *m_arrowTextColor;
    DArrowLineExpand *m_arrowSelectedTextColor;
    GrubThemeDbus *m_themeDbus;
    GrubDbus *m_grubDbus;
    GrubBackground *m_grubBackground;

    void init();
};

#endif // GRUBWIDGET_H
