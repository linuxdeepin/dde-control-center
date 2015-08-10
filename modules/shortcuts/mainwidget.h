#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <libdui/libdui_global.h>
#include <libdui/dheaderline.h>

DUI_USE_NAMESPACE

class MainWidget : public QFrame
{
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    QVBoxLayout *m_layout;
    DHeaderLine *m_header;

    void init();
};

#endif // GRUBWIDGET_H
