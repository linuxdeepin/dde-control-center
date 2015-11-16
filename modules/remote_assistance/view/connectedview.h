#ifndef SHARINGVIEW_H
#define SHARINGVIEW_H

#include "abstractview.h"

class QLabel;

class ConnectedView : public AbstractView
{
    Q_OBJECT
public:
    ConnectedView(QWidget*p = nullptr);
    ConnectedView* setText(const QString&);

signals:
    void disconnect();

private:
    QWidget* createMainWidget() Q_DECL_OVERRIDE;

private slots:
    void onDisconnectButtonClicked();

private:
    QLabel* m_text;
};

#endif // SHARINGVIEW_H
