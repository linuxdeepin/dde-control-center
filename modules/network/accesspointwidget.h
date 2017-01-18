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

namespace network {

class AccessPointWidget : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit AccessPointWidget(QWidget *parent = 0);

    int strength() const { return m_strength; }
    bool connected() const { return m_connected; }
    const QString path() const { return m_path; }

signals:
    void requestEdit(const QString &path, const QString &ssid) const;
    void requestConnect(const QString &path, const QString &ssid) const;

public slots:
    void setStrength(const int strength);
    void setEncyrpt(const bool encyrpt);
    void setConnected(const bool connected);
    void setEditable(const bool editable);
    void setAPName(const QString &name);
    void setPath(const QString &path);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QLabel *m_lockIcon;
    QLabel *m_strengthIcon;
    QLabel *m_apName;
    Dtk::Widget::DImageButton *m_activeIcon;
    Dtk::Widget::DImageButton *m_detailBtn;
    QVBoxLayout *m_mainLayout;

    int m_strength;
    bool m_connected;
    QString m_path;
};

}   // namespace network

}   // namespace dcc

#endif // ACCESSPOINTWIDGET_H
