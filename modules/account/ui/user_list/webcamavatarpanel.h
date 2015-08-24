#ifndef WEBCAMAVATARPANEL_H
#define WEBCAMAVATARPANEL_H

#include <QWidget>
#include <QCamera>
#include <QCameraInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "generaladdbutton.h"
#include "generalremovebutton.h"
#include "libdui/dslider.h"
#include "libdui/dcameraview.h"
#include "libdui/dtextbutton.h"
#include "libdui/dconstants.h"

DUI_USE_NAMESPACE

class WebcamAvatarPanel : public QWidget
{
    Q_OBJECT
public:
    explicit WebcamAvatarPanel(QWidget *parent = 0);
    void turnOnCamera();
    void turnOffCamera();

signals:
    void selectedAvatar(const QString &path);

private slots:
    void onCameraError(QCamera::Error value);

private:
    void initCameraLayout();
    void initCameraView();
    void initScaleSlider();
    void initControlButton();
    void initStackWidget();

    void onSnapshot();
    void onConfirm();
    void onSliderValueChanged(int value);
private:
    QStackedWidget *m_stackWidget = NULL;
    QVBoxLayout *m_layout = NULL;
    QCamera *m_camera = NULL;
    DCameraView *m_cameraView = NULL;
    bool m_captureState = false;

    const int SLIDER_STEP = 1;
    const int PANEL_HEIGHT = 240;
    const int CAMERA_VIEW_SIZE = 120;
    const QString IMAGE_SAVE_PATH = "/tmp/account_camera_tmp_avatar.png";
};

#endif // WEBCAMAVATARPANEL_H
