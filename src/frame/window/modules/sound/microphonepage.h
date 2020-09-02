/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MICROPHONEPAGE_H_V20
#define MICROPHONEPAGE_H_V20

#include "interface/namespace.h"
#include "modules/sound/soundmodel.h"

#include <QStandardItemModel>
#include <QWidget>
#include <QLabel>

#define ICON_SIZE   20

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {
class TitledSliderItem;
class SwitchWidget;
class ComboxWidget;
class Port;
}

namespace sound {
class SoundModel;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class SoundLabel : public QLabel
{
    Q_OBJECT
public:
    explicit SoundLabel(QWidget *parent = nullptr);
    void mouseReleaseEvent(QMouseEvent *e) override;
    virtual ~SoundLabel() {}

private:
    bool m_mute;

Q_SIGNALS:
    void clicked(bool checked);
};

class MicrophonePage : public QWidget
{
    Q_OBJECT
public:
    MicrophonePage(QWidget *parent = nullptr);

    ~MicrophonePage();
    void resetUi();

public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchMicrophone(bool on);
    void requestSetMicrophoneVolume(double vol);
    void requestSetPort(const dcc::sound::Port *);
    //请求降噪
   void requestReduceNoise(bool value);
   //请求静音切换
   void requestMute();

private Q_SLOTS:
    void removePort(const QString &portId, const uint &cardId);
    void addPort(const dcc::sound::Port *port);
    void toggleMute();

private:
    void initSlider();
    void refreshIcon();
    const QPixmap loadSvg(const QString &iconName, const QString &localPath, const int size, const qreal ratio);

private:
    dcc::sound::SoundModel *m_model{nullptr};
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::SwitchWidget *m_sw{nullptr};
    dcc::widgets::TitledSliderItem *m_inputSlider{nullptr};
    dcc::widgets::TitledSliderItem *m_feedbackSlider{nullptr};
    QMetaObject::Connection m_conn;
    //输入列表的下拉框列表
    dcc::widgets::ComboxWidget *m_inputSoundCbx;
    //噪音抑制
    dcc::widgets::SwitchWidget *m_noiseReductionsw{nullptr};

    QStandardItemModel *m_inputModel{nullptr};
    const dcc::sound::Port  *m_currentPort{nullptr};

    SoundLabel *m_volumeBtn;
    bool m_mute;
};

}
}

#endif // MICROPHONEPAGE_H_V20
