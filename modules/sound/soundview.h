#ifndef SOUNDVIEW_H
#define SOUNDVIEW_H


#include <QTime>
#include <QFrame>

class ModuleHeader;
class QVBoxLayout;

namespace Dtk
{
namespace Widget
{
class DSwitchLineExpand;
class DSlider;
class DBaseExpand;
class DButtonList;
class DScrollArea;
class DHeaderLine;
}
}

using namespace Dtk::Widget;

namespace Plugin
{
namespace Sound
{

class SoundControl;
struct SoundModel;

class SoundView: public QFrame
{
    Q_OBJECT

public:
    SoundView(SoundControl *control, QWidget *parent = nullptr);
    ~SoundView() Q_DECL_OVERRIDE;

public slots:
    void init(const SoundModel &model);
    void initOutputOption(const SoundModel &model);
    void initInputOption(const SoundModel &model);
    void initEffectOptions(const SoundModel &model);
    void initAdvanceOptions(const SoundModel &model);

    void updateSpeakerUI(const SoundModel &model);
    void updateMicrophoneUI(const SoundModel &model);
    void updateInputPortsUI(const SoundModel &model);
    void updateInputDevicesUI(const SoundModel &model);

    void updateOutputPortsUI(const SoundModel &model);
    void updateOutputDevicesUI(const SoundModel &model);

signals:
    void requestInit();

private:
    struct SoundModel   *m_model    = nullptr;
    struct SoundControl *m_control  = nullptr;

private://UI
    QVBoxLayout         *m_mainLayout       = nullptr;
    QFrame              *m_mainWidget       = nullptr;
    ModuleHeader        *m_moduleHeader     = nullptr;
    DSwitchLineExpand   *m_speakerExpand    = nullptr;
    DSwitchLineExpand   *m_microphoneExpand = nullptr;

    DSlider     *m_outputVolumeSlider       = nullptr;
    DSlider     *m_leftRightBalanceSlider   = nullptr;
    DSlider     *m_inputVolumeSlider        = nullptr;
    DSlider     *m_inputFeedbackSlider      = nullptr;
    DBaseExpand *m_outputPortsExpand        = nullptr;
    DButtonList *m_outputPortsList          = nullptr;
    DBaseExpand *m_inputPortsExpand         = nullptr;
    DButtonList *m_inputPortsList           = nullptr;
    DBaseExpand *m_outputDevicesExpand      = nullptr;
    DButtonList *m_outputDevicesList        = nullptr;
    DBaseExpand *m_inputDevicesExpand       = nullptr;
    DButtonList *m_inputDevicesList         = nullptr;
    QVBoxLayout *m_mainWidgetVLayout        = nullptr;
    DScrollArea *m_scrollArea               = nullptr;
    DHeaderLine *m_soundEffectsLine         = nullptr;
    QTimer      *m_SetOutputVolumeTimer     = nullptr;
    bool        blockOutputVolumeSignal     = false;
    QTimer      *m_SetBalanceTimer          = nullptr;
    bool        blockBalanceSignal          = false;
    QTimer      *m_SetInputVolumeTimer      = nullptr;
    bool        blockInputVolumeSignal      = false;

    QTime       m_timeDeltaRecorder;
    QTime       m_SetInputVolumeRecorder;
    QTime       m_SetBalanceRecorder;
    QTime       m_SetOutputVolumeRecorder;
};

}
}

#endif // SOUNDVIEW_H
