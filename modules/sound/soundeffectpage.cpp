#include "soundeffectpage.h"
#include "translucentframe.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::sound;

#if 0
QT_TRANSLATE_NOOP("SoundEffectPage", "Boot up"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Shut down"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Logout"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Wake up"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Volume +/-"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Notification"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Low battery"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Send icon in Launcher to Desktop"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Empty Trash"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Plug in"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Plug out"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Removable device connected"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Removable device removed"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Error"),
#endif

SoundEffectPage::SoundEffectPage(SoundModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
{
    TranslucentFrame *w = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    SettingsGroup *allEffectGrp = new SettingsGroup;

    m_allEffectSwitch = new SwitchWidget(tr("System Sound Effect"));
    m_effectGrp = new SettingsGroup;

    connect(m_allEffectSwitch, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onSwitchClicked);
    connect(m_allEffectSwitch, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onAllEffectSwitchBtnChanged);

    QTimer::singleShot(0, this, [=] {
        Q_EMIT requestQueryData("enabled");
    });
    m_allEffectSwitch->setChecked(model->queryEffectData("enabled"));

    auto effect_map = model->soundEffectMap();

    for (auto it = effect_map.constBegin(); it != effect_map.constEnd(); ++it) {
        SwitchWidget * widget = new SwitchWidget(tr(it.key().toUtf8()));
        connect(widget, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onSwitchClicked);
        m_effectGrp->appendItem(widget);
        m_effectSwitchList[widget] = it.value();
        QTimer::singleShot(0, this, [=] {
            Q_EMIT requestQueryData(it.value());
        });
        widget->setChecked(model->queryEffectData(it.value()));
    }

    allEffectGrp->appendItem(m_allEffectSwitch);

    layout->addWidget(allEffectGrp);
    layout->addSpacing(20);
    layout->addWidget(m_effectGrp);

    w->setLayout(layout);

    setContent(w);
    connect(model, &SoundModel::soundEffectDataChanged, this, &SoundEffectPage::onEffectSwitchChanged);
}

void SoundEffectPage::onSwitchClicked(const bool enable)
{
    SwitchWidget *widget = static_cast<SwitchWidget*>(sender());
    Q_EMIT requestSetEffectEnable(m_effectSwitchList[widget], enable);
}

void SoundEffectPage::onEffectSwitchChanged(const QString &name, const bool enable)
{
    if (name == "enabled") {
        m_allEffectSwitch->setChecked(enable);
        onAllEffectSwitchBtnChanged(enable);
        return;
    }

    for (auto it = m_effectSwitchList.constBegin(); it != m_effectSwitchList.constEnd(); ++it) {
        if (it.value() == name) {
            it.key()->setChecked(enable);
            return;
        }
    }
}

void SoundEffectPage::onAllEffectSwitchBtnChanged(const bool enable)
{
    m_effectGrp->setVisible(enable);
}
