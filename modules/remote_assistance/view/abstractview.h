#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QFrame>
 #include <libdui_global.h>

class QVBoxLayout;
class QHBoxLayout;

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

class AbstractView : public QWidget
{
    Q_OBJECT
public:
    AbstractView(QWidget* p=nullptr);
    AbstractView* addWidget(QWidget*);
    AbstractView* addSpacing(int);
    AbstractView* addLayout(QLayout*, int=0);
    AbstractView* addButton(DUI_NAMESPACE::DTextButton* btn, int stretch=0, Qt::Alignment alignment=0);

protected:
    QWidget* m_mainWidget;
    QVBoxLayout* m_viewLayout;
    QHBoxLayout* m_buttonLayout;

    virtual void initialize();
    virtual QWidget* createMainWidget() = 0;
};

#endif // ABSTRACTVIEW_H
