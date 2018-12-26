#include "soundeffectpage.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSound>
#include <QEvent>
#include <DHiDPIHelper>

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

DWIDGET_USE_NAMESPACE

SoundEffectPage::SoundEffectPage(SoundModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_playIcon(new QLabel(this))
    , m_hideIconTimer(new QTimer(this))
    , m_iconAni(new QVariantAnimation(this))
{
    m_hideIconTimer->setInterval(5000);
    m_hideIconTimer->setSingleShot(true);

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

    connect(m_allEffectSwitch, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onSwitchClicked);
    connect(m_allEffectSwitch, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onAllEffectSwitchBtnChanged);

    QTimer::singleShot(0, this, [=] {
        Q_EMIT requestQueryData("enabled");
    });
    m_allEffectSwitch->setChecked(model->queryEffectData("enabled"));

    auto effect_map = model->soundEffectMap();

    for (auto it = effect_map.constBegin(); it != effect_map.constEnd(); ++it) {
        SwitchWidget * widget = new SwitchWidget(tr(it.key().toUtf8()));
        widget->installEventFilter(this);
        connect(widget, &SwitchWidget::checkedChanged, this, &SoundEffectPage::onSwitchClicked);
        connect(widget, &SwitchWidget::clicked, this, &SoundEffectPage::readyPlay);
        m_effectGrp->appendItem(widget);
        m_effectSwitchList[widget] = it.value();
        QTimer::singleShot(0, this, [=] {
            Q_EMIT requestQueryData(it.value());
        });
        widget->setChecked(model->queryEffectData(it.value()));
    }

    m_playIcon->hide();

    allEffectGrp->appendItem(m_allEffectSwitch);

    layout->addWidget(allEffectGrp);
    layout->addSpacing(20);
    layout->addWidget(m_effectGrp);
    layout->addStretch();

    w->setLayout(layout);

    setContent(w);
    connect(model, &SoundModel::soundEffectDataChanged, this, &SoundEffectPage::onEffectSwitchChanged);
    connect(model, &SoundModel::playPathChanged, this, &SoundEffectPage::onPlayPathChanged);
    connect(m_hideIconTimer, &QTimer::timeout, this, [=] {
        m_playIcon->hide();
        m_iconAni->stop();
    });

    connect(m_iconAni, &QVariantAnimation::valueChanged, this, [=] (const QVariant &value) {
        m_playIcon->setPixmap(DHiDPIHelper::loadNxPixmap(QString(":/sound/themes/dark/sound_preview_%1.svg").arg(value.toInt())));
    qDebug() << value.toInt();
    });
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

void SoundEffectPage::onPlayPathChanged(const QString &name, const QString &path)
{
    QSound sound(path);
    sound.setLoops(1);
    sound.play();

    SwitchWidget *widget = m_effectSwitchList.key(convert(name, true));
    m_playIcon->move(QPoint(widget->width() - m_playIcon->width() - 60,
                            widget->mapToGlobal(QPoint(0, 0)).y() + (widget->height() - m_playIcon->height()) / 2));

    m_playIcon->show();
    m_hideIconTimer->start();
    m_iconAni->start();
}

void SoundEffectPage::readyPlay()
{
    SwitchWidget *widget = static_cast<SwitchWidget*>(sender());

    Q_EMIT requestPlay(convert(m_effectSwitchList[widget], false));
}

const QString SoundEffectPage::convert(const QString &source, bool toHump) const
{
    QString result;

    if (toHump) {
        for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
            if ((*it) == "-") {
                ++it;
                result.append(it->toUpper());
            }
            else {
                result.append((*it));
            }
        }
    }
    else {
        for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
            result.append(QString(it->isLower() ? (*it) : QString("-%1").arg(it->toLower())));
        }
    }

    return result;
}
