#ifndef PPPOESECTION_H
#define PPPOESECTION_H

#include "abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"

#include <networkmanagerqt/pppoesetting.h>

namespace dcc {
namespace widgets {

class PPPOESection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPOESection(NetworkManager::PppoeSetting::Ptr pppoeSetting, QFrame *parent = 0);
    virtual ~PPPOESection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

private:
    NetworkManager::PppoeSetting::Ptr m_pppoeSetting;

    LineEditWidget *m_userName;
    LineEditWidget *m_service;
    LineEditWidget *m_password;
};

} /* widgets */
} /* dcc */

#endif /* PPPOESECTION_H */
