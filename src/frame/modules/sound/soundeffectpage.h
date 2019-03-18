#ifndef SOUNDEFFECTPAGE_H
#define SOUNDEFFECTPAGE_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "soundmodel.h"

#include <QLabel>
#include <QVariantAnimation>
#include <DDesktopServices>
#include <QSound>
#include <QScopedPointer>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace sound {
class SoundEffectPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit SoundEffectPage(SoundModel *model, QWidget *parent = nullptr);

Q_SIGNALS:
    void requestEnableAllEffect(bool enable);
    void requestSetEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable);

public Q_SLOTS:
    void onSwitchClicked(const bool enable);
    void onEffectSwitchChanged(DDesktopServices::SystemSoundEffect effect, const bool enable);
    void onAllEffectSwitchBtnChanged(const bool enable);
    void readyPlay();

private:
    SoundModel *m_model;
    widgets::SwitchWidget *m_allEffectSwitch;
    widgets::SettingsGroup *m_effectGrp;
    QLabel *m_playIcon;
    QTimer *m_hideIconTimer;
    QVariantAnimation *m_iconAni;
    widgets::SwitchWidget *m_currentPlayItem;
    QMap<widgets::SwitchWidget*, DDesktopServices::SystemSoundEffect> m_effectSwitchList;
    QScopedPointer<QSound> m_sound;
};
}
}

#endif // SOUNDEFFECTPAGE_H
