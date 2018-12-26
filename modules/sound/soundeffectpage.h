#ifndef SOUNDEFFECTPAGE_H
#define SOUNDEFFECTPAGE_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "soundmodel.h"

#include <QLabel>
#include <QVariantAnimation>

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
    void requestPlay(const QString &name);

public Q_SLOTS:
    void onSwitchClicked(const bool enable);
    void onEffectSwitchChanged(const QString &name, const bool enable);
    void onAllEffectSwitchBtnChanged(const bool enable);
    void onPlayPathChanged(const QString &name, const QString &path);
    void readyPlay();

private:
    const QString convert(const QString &source, bool toHump) const;

private:
    SoundModel *m_model;
    widgets::SwitchWidget *m_allEffectSwitch;
    widgets::SettingsGroup *m_effectGrp;
    QLabel *m_playIcon;
    QTimer *m_hideIconTimer;
    QVariantAnimation *m_iconAni;
    QMap<widgets::SwitchWidget*, QString> m_effectSwitchList;
};
}
}

#endif // SOUNDEFFECTPAGE_H
