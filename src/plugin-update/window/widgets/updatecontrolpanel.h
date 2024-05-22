//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
//
// Created by swq on 2021/9/7.
//

#ifndef UPDATECONTROLPANEL_H
#define UPDATECONTROLPANEL_H

#include <dtkwidget_global.h>
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QLabel;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DCommandLinkButton;
class DLabel;
class DIconButton;
class DProgressBar;
DWIDGET_END_NAMESPACE

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

class updateControlPanel: public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit updateControlPanel(QWidget *parent = nullptr);

    void initUi();
    void initConnect();

    ButtonStatus getButtonStatus() const;
    void setButtonStatus(const ButtonStatus &value);

    void setTitle(QString title);
    void setVersion(QString version);
    void setDetail(QString detail);
    void setDate(QString date);
    void setProgressText(const QString &text, const QString &toolTip = "");
    void setShowMoreButtomText(QString text);

    int getCurrentValue() const;
    void setCurrentValue(int currentValue);

    void showButton(bool visible);
    void setCtrlButtonEnabled(bool enabled);
    void setDetailEnable(bool enable);
    void setShowMoreButtonVisible(bool visible);
    void setDetailLabelVisible(bool visible);
    void setVersionVisible(bool visible);
    void setDatetimeVisible(bool  visible);

    const QString getElidedText(QWidget *widget, QString data, Qt::TextElideMode mode = Qt::ElideRight, int width = 100, int flags = 0, int line = 0);


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

private Q_SLOTS:
    void onThemeChanged();

private:
    DTK_WIDGET_NAMESPACE::DLabel *m_titleLable;
    DTK_WIDGET_NAMESPACE::DLabel *m_versionLabel;
    DTK_WIDGET_NAMESPACE::DLabel *m_detailLabel;
    DTK_WIDGET_NAMESPACE::DLabel *m_dateLabel;
    DTK_WIDGET_NAMESPACE::DLabel *m_progressLabel;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_showMoreBUtton;

    DTK_WIDGET_NAMESPACE::DIconButton *m_startButton;
    DTK_WIDGET_NAMESPACE::DProgressBar *m_Progess;

    ButtonStatus m_buttonStatus;
    UpdateDProgressType m_progressType;
    int m_currentValue;
};

#endif //UPDATECONTROLPANEL_H
