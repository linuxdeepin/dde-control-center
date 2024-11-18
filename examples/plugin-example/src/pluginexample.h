// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PLUGINEXAMPLE_H
#define PLUGINEXAMPLE_H

#include <QObject>

class PluginExample : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
public:
    explicit PluginExample(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    Q_INVOKABLE int calc(int a, int b);

public Q_SLOTS:
    void setCalcType(int type);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void calcTypeChanged(int calcType);

private:
    QString m_name;
    int m_calcType;
};

#endif // PLUGINEXAMPLE_H
