#ifndef PPPSECTION_H
#define PPPSECTION_H

#include "abstractsection.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "comboboxwidget.h"

#include <networkmanagerqt/pppsetting.h>

namespace dcc {
namespace widgets {

class PPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPSection(NetworkManager::PppSetting::Ptr pppSetting, QFrame *parent = 0);
    virtual ~PPPSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);

private:
    NetworkManager::PppSetting::Ptr m_pppSetting;

    SwitchWidget *m_mppeEnable;
    SwitchWidget *m_mppe128;
    SwitchWidget *m_mppeStateful;
    SwitchWidget *m_refuseEAP;
    SwitchWidget *m_refusePAP;
    SwitchWidget *m_refuseCHAP;
    SwitchWidget *m_refuseMSCHAP;
    SwitchWidget *m_refuseMSCHAP2;
    SwitchWidget *m_noBSDComp;
    SwitchWidget *m_noDeflate;
    SwitchWidget *m_noVJComp;
    SwitchWidget *m_lcpEchoInterval;
};

} /* widgets */
} /* dcc */

#endif /* PPPSECTION_H */
