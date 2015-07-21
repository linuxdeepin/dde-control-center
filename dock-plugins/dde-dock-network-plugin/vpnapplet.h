#ifndef VPNAPPLET_H
#define VPNAPPLET_H

#include <QFrame>

class VPNComponent;
class VPNApplet : public QFrame
{
    Q_OBJECT
public:
    VPNApplet(VPNComponent * host);

private:
    VPNComponent * m_host;
    QFrame * m_mainFrame;

private slots:
    void updateContent();
};

#endif // VPNAPPLET_H
