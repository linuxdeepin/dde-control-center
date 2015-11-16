#ifndef GENERATINGVIEW_H
#define GENERATINGVIEW_H

#include "abstractview.h"

class GeneratingView : public AbstractView
{
    Q_OBJECT
public:
    GeneratingView(QWidget* p=nullptr);

signals:
    void cancel();

private slots:
    void onCancelButtonClicked();

private:
    QWidget* createMainWidget();
};

#endif // GENERATINGVIEW_H
