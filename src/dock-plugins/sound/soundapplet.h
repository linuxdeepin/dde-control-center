// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDAPPLET_H
#define SOUNDAPPLET_H

#include "dockslider.h"
#include "jumpsettingbutton.h"
#include "slidercontainer.h"
#include "soundcardport.h"
#include "soundcontroller.h"
#include "pluginlistview.h"

#include "audio1interface.h"
#include "audio1sinkinterface.h"

#include <DIconButton>

#include <QLabel>
#include <QScrollArea>
#include <QSlider>
#include <QVBoxLayout>
#include <QSpacerItem>

DWIDGET_USE_NAMESPACE

using DBusAudio = org::deepin::dde::Audio1;
using DBusSink = org::deepin::dde::audio1::Sink;

namespace Dock {
class TipsWidget;
}

class SoundApplet : public QWidget
{
    Q_OBJECT
public:
    enum volumeSliderStatus {
        Enabled = 0,
        Disabled,
        Hidden
    };

    explicit SoundApplet(QWidget* parent = 0);

    DockSlider* mainSlider();
    void setMinHeight(int minHeight);

private slots:
    void onDefaultSinkChanged();
    void onVolumeChanged(int volume);
    void volumeSliderValueChanged();
    void maxUiVolumeChanged();
    void updatePorts();
    void removePort(const QString& key);
    void addPort(const SoundCardPort* port);

signals:
    void requestHideApplet();

private:
    void initUi();
    void initConnections();
    void refreshIcon();
    void enableDevice(bool flag);
    void removeDisabledDevice(QString portId, unsigned int cardId);
    void updateVolumeSliderStatus(int status);
    void resizeApplet();
    PluginStandardItem* findItem(const QString &uniqueKey) const;
    void selectItem(PluginStandardItem *targetItem);

protected:
    bool eventFilter(QObject *watcher, QEvent *event);

private:
    DockSlider* m_volumeSlider;
    QLabel* m_volumeLabel;
    QLabel* m_deviceLabel;
    QLabel* m_outputLabel;
    QWidget* m_deviceWidget;
    QVBoxLayout* m_centralLayout;
    SliderContainer* m_sliderContainer;
    DBusAudio* m_audioInter;
    DBusSink* m_defSinkInter;
    PluginListView* m_listView;
    JumpSettingButton* m_settingButton;
    QStandardItemModel* m_itemModel;
    Dtk::Core::DConfig *m_dConfig;
    int m_minHeight;
    QSpacerItem *m_spacerItem;
};

#endif // SOUNDAPPLET_H
