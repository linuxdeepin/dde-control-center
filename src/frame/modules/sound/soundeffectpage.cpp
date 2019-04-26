#include "soundeffectpage.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSound>
#include <QEvent>
#include <DHiDPIHelper>
#include <QApplication>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::sound;

#if 0
QT_TRANSLATE_NOOP("SoundEffectPage", "Boot up"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Shut down"),
QT_TRANSLATE_NOOP("SoundEffectPage", "Log out"),
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

DWIDGET_USE_NAMESPACE

SoundEffectPage::SoundEffectPage(SoundModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_playIcon(new QLabel(this))
    , m_hideIconTimer(new QTimer(this))
    , m_iconAni(new QVariantAnimation(this))
    , m_currentPlayItem(nullptr)
    , m_sound(nullptr)
{
    setTitle(tr("Sound Effects"));

    m_hideIconTimer->setInterval(1000);
    m_hideIconTimer->setSingleShot(false);

    m_playIcon->setFixedSize(20, 20);

    m_iconAni->setDuration(1000);
    m_iconAni->setLoopCount(-1);
    m_iconAni->setStartValue(1);
    m_iconAni->setEndValue(5);

    TranslucentFrame *w = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    SettingsGroup *allEffectGrp = new SettingsGroup;

    m_allEffectSwitch = new SwitchWidget(tr("System Sound Effect"));
    m_effectGrp = new SettingsGroup;

    connect(m_allEffectSwitch, &SwitchWidget::checkedChanged, this, &SoundEffectPage::requestEnableAllEffect);
    connect(m_model, &SoundModel::enableSoundEffectChanged, m_allEffectSwitch, &SwitchWidget::setChecked);
    connect(m_model, &SoundModel::enableSoundEffectChanged, this, &SoundEffectPage::onAllEffectSwitchBtnChanged);

    m_allEffectSwitch->setChecked(m_model->enableSoundEffect());
    onAllEffectSwitchBtnChanged(m_model->enableSoundEffect());

    auto effect_map = model->soundEffectMap();

    QList<DDesktopServices::SystemSoundEffect> disable_list {
        DDesktopServices::SystemSoundEffect::SSE_LowBattery,
        DDesktopServices::SystemSoundEffect::SSE_PlugIn,
        DDesktopServices::SystemSoundEffect::SSE_PlugOut,
    };

    for (auto it : effect_map) {
        if (!m_model->isLaptop() && disable_list.contains(it.second)) continue;

        SwitchWidget * widget = new SwitchWidget(qApp->translate("SoundEffectPage", it.first.toUtf8()));
        widget->installEventFilter(this);
        connect(widget, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onSwitchClicked);
        connect(widget, &SwitchWidget::clicked, this, &SoundEffectPage::readyPlay);
        m_effectGrp->appendItem(widget);
        m_effectSwitchList[widget] = it.second;
        widget->setChecked(model->queryEffectData(it.second));
    }

    m_playIcon->hide();

    allEffectGrp->appendItem(m_allEffectSwitch);

    layout->addSpacing(10);
    layout->addWidget(allEffectGrp);
    layout->addSpacing(20);
    layout->addWidget(m_effectGrp);
    layout->addStretch();

    w->setLayout(layout);

    setContent(w);
    connect(model, &SoundModel::soundEffectDataChanged, this, &SoundEffectPage::onEffectSwitchChanged);
    connect(m_hideIconTimer, &QTimer::timeout, this, [=] {
        if (m_sound->isFinished()) {
            m_playIcon->hide();
            m_iconAni->stop();
            m_currentPlayItem = nullptr;
            m_hideIconTimer->stop();
        }
    });

    connect(m_iconAni, &QVariantAnimation::valueChanged, this, [=] (const QVariant &value) {
        m_playIcon->setPixmap(DHiDPIHelper::loadNxPixmap(QString(":/sound/themes/dark/sound_preview_%1.svg").arg(value.toInt())));
    });

    connect(m_model, &SoundModel::isLaptopChanged, this, [=] (bool isLaptop) {
        if (!isLaptop) {
            for (auto it = m_effectSwitchList.constBegin(); it != m_effectSwitchList.constEnd(); ++it) {
                if (disable_list.contains(it.value())) {
                    m_effectGrp->removeItem(it.key());
                }
            }
        }
    });
}

void SoundEffectPage::onSwitchClicked(const bool enable)
{
    SwitchWidget *widget = static_cast<SwitchWidget*>(sender());
    Q_EMIT requestSetEffectEnable(m_effectSwitchList[widget], enable);
}

void SoundEffectPage::onEffectSwitchChanged(DDesktopServices::SystemSoundEffect effect, const bool enable)
{
    for (auto it = m_effectSwitchList.constBegin(); it != m_effectSwitchList.constEnd(); ++it) {
        if (it.value() == effect) {
            it.key()->setChecked(enable);
            return;
        }
    }
}

void SoundEffectPage::onAllEffectSwitchBtnChanged(const bool enable)
{
    m_effectGrp->setVisible(enable);
    m_playIcon->setVisible(enable && m_hideIconTimer->isActive());
}

void SoundEffectPage::readyPlay()
{
    SwitchWidget *widget = static_cast<SwitchWidget*>(sender());
    if (widget != m_currentPlayItem) {
        m_sound.reset(new QSound(m_model->soundEffectPathByType(m_effectSwitchList[widget])));

        m_sound->stop();
        m_sound->play();

        m_currentPlayItem = widget;
        m_playIcon->move(QPoint(widget->width() - m_playIcon->width() - 60,
                                widget->mapToGlobal(QPoint(0, 0)).y() + (widget->height() - m_playIcon->height()) / 2));

        m_playIcon->show();
        m_hideIconTimer->start();
        m_iconAni->stop();
        m_iconAni->start();
    }
}
