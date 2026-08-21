//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PERSIONALIZATIONINTERFACE_H
#define PERSIONALIZATIONINTERFACE_H

#include <QAbstractItemModel>

#include "personalizationworker.h"
#include "personalizationmodel.h"

class PersonalizationInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PersonalizationModel *model MEMBER m_model CONSTANT)
    Q_PROPERTY(PersonalizationWorker *worker MEMBER m_work CONSTANT)
    Q_PROPERTY(bool pickerAvailable READ isPickerAvailable CONSTANT)

public:
    explicit PersonalizationInterface(QObject *parent = nullptr);

    bool isPickerAvailable() const { return m_pickerAvailable; }
    Q_INVOKABLE QString platformName();
    Q_INVOKABLE void handleCmdParam(PersonalizationExport::ModuleType type, const QString &cmdParam);
    Q_INVOKABLE void startPicker();

private:
    bool checkPickerService();

private Q_SLOTS:
    void onPickerColorPicked(const QString &uuid, const QString &colorName);

signals:
    void colorPicked(const QString &color);
    void pickerError(const QString &error);

private:
    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
    QString m_pickerId;
    bool m_pickerAvailable;
};

#endif // PERSIONALIZATIONINTERFACE_H
