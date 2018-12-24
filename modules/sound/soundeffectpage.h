#ifndef SOUNDEFFECTPAGE_H
#define SOUNDEFFECTPAGE_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "soundmodel.h"

namespace dcc {
namespace sound {
class SoundEffectPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit SoundEffectPage(SoundModel *model, QWidget *parent = nullptr);

Q_SIGNALS:
    void requestQueryData(const QString &name);
    void requestSetEffectEnable(const QString &name, bool enable);

public Q_SLOTS:
    void onSwitchClicked(const bool enable);
    void onEffectSwitchChanged(const QString &name, const bool enable);
    void onAllEffectSwitchBtnChanged(const bool enable);

private:
    SoundModel *m_model;
    widgets::SwitchWidget *m_allEffectSwitch;
    widgets::SettingsGroup *m_effectGrp;
    QMap<widgets::SwitchWidget*, QString> m_effectSwitchList;
};
}
}

#endif // SOUNDEFFECTPAGE_H
