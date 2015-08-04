#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <libdui/dheaderline.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class GrubWidget : public QFrame
{
public:
    explicit GrubWidget(QWidget *parent = 0);
    ~GrubWidget();

private:
    QVBoxLayout *m_layout;
    DHeaderLine *m_header;
};

#endif // GRUBWIDGET_H
