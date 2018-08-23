#ifndef ABSTRACTSECTION_H
#define ABSTRACTSECTION_H

#include "settingsgroup.h"

namespace dcc {
namespace widgets {

class AbstractSection : public SettingsGroup
{
    Q_OBJECT

public:
    explicit AbstractSection(QFrame *parent = 0);
    explicit AbstractSection(const QString &title, QFrame *parent = 0);
    virtual ~AbstractSection();

    virtual bool allInputValid() = 0;
    virtual void saveSettings() = 0;

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* ABSTRACTSECTION_H */
