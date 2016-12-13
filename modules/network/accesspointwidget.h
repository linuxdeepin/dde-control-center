#ifndef ACCESSPOINTWIDGET_H
#define ACCESSPOINTWIDGET_H

#include "settingsitem.h"

class QVBoxLayout;
class QLabel;

namespace Dtk {

namespace Widget {

class DImageButton;

}

}

namespace dcc {

namespace netowrk {

class AccessPointWidget : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit AccessPointWidget(QWidget *parent = 0);

signals:
    void requestEdit();

public slots:
    void setEncyrpt(const bool encyrpt);
    void setAPName(const QString &name);

private:
    QLabel *m_lockIcon;
    QLabel *m_strengthIcon;
    QLabel *m_apName;
    Dtk::Widget::DImageButton *m_detailBtn;
    QVBoxLayout *m_mainLayout;
};

}   // namespace network

}   // namespace dcc

#endif // ACCESSPOINTWIDGET_H
