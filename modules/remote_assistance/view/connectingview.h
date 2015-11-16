#ifndef CONNECTINGVIEW_H
#define CONNECTINGVIEW_H

#include "abstractview.h"

class ConnectingView : public AbstractView
{
    Q_OBJECT
public:
    ConnectingView(QWidget*p=nullptr);

signals:
    void cancel();

private:
    QWidget* createMainWidget() Q_DECL_OVERRIDE;

private slots:
    void onCancelButtonClicked();
};

#endif // CONNECTINGVIEW_H
