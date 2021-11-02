//
// Created by swq on 2021/9/7.
//

#ifndef UPDATECONTROLPANEL_H
#define UPDATECONTROLPANEL_H

#include <QWidget>
#include <QLabel>
#include <DFloatingButton>
#include <DCommandLinkButton>
#include <DLabel>
#include <DLineEdit>
#include <DTextEdit>
#include <DIconButton>
#include <DTipLabel>
#include <DProgressBar>
#include <DSysInfo>

#include "widgets/settingsitem.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace update {

enum ButtonStatus {
    invalid,
    start,
    pause,
    retry
};

enum UpdateDProgressType {
    InvalidType,
    Download,
    Paused,
    Install,
    Backup
};

class updateControlPanel: public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit updateControlPanel(QWidget *parent = nullptr);

    ButtonStatus getButtonStatus() const;
    void setButtonStatus(const ButtonStatus &value);

    void setTitle(QString title);
    void setVersion(QString version);
    void setDetail(QString detail);
    void setDate(QString date);
    void setProgressText(QString text);
    void setShowMoreButtomText(QString text);

    int getCurrentValue() const;
    void setCurrentValue(int currentValue);

    void showButton(bool visible);
    void setCtrlButtonEnabled(bool enabled);
    void setDetailEnable(bool enable);
    void setShowMoreButtonVisible(bool visible);
    void setDetailLabelVisible(bool visible);
    void setVersionVisible(bool visible);
    void setUpdateButtonVisible(bool visible);
    void setUpdateButtonEnable(bool enable);


    UpdateDProgressType getProgressType() const;
    void setProgressType(const UpdateDProgressType &progressType);
    void showUpdateProcess(bool visible);
    void requestRetry();

Q_SIGNALS:
    void showDetail();
    void startUpdate();
    void StartDownload();
    void PauseDownload();
    void RetryUpdate();

public Q_SLOTS:
    void onStartUpdate();
    void onButtonClicked();

    void setProgressValue(int value);
    void setButtonIcon(ButtonStatus status);

private:
    DLabel *m_titleLable;
    DLabel *m_versionLabel;
    DLabel *m_detailLabel;
    DLabel *m_dateLabel;
    DLabel *m_progressLabel;
    DCommandLinkButton *m_updateButton;
    DCommandLinkButton *m_showMoreBUtton;

    DIconButton *m_startButton;
    DProgressBar *m_Progess;

    ButtonStatus m_buttonStatus;
    UpdateDProgressType m_progressType;
    int m_currentValue;
};

}
}
#endif //UPDATECONTROLPANEL_H
