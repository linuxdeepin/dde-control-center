//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FILECHOOSEWIDGET_H
#define FILECHOOSEWIDGET_H

#include "widgets/settingsitem.h"
#include <DFileChooserEdit>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class FileChooseWidget : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT

public:
    explicit FileChooseWidget(QWidget *parent = nullptr);

    DTK_WIDGET_NAMESPACE::DFileChooserEdit *edit() const { return m_fileChooserEdit; }
    void setTitle(const QString &title);
    virtual void setIsErr(const bool err = true) override;

Q_SIGNALS:
    void requestFrameKeepAutoHide(const bool autoHide) const;

private:
    DTK_WIDGET_NAMESPACE::DFileChooserEdit *m_fileChooserEdit;
    QLabel *m_title;
};
#endif // FILECHOOSEWIDGET_H
