//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WACOMMODEL_H
#define WACOMMODEL_H

#include <QObject>

class WacomModelPrivate;
class WacomModel : public QObject
{
    Q_OBJECT
public:
    explicit WacomModel(QObject *parent = nullptr);
    ~WacomModel();

    Q_PROPERTY(bool Exist READ exist NOTIFY ExistChanged)
    bool exist() const;

    Q_PROPERTY(bool CursorMode READ CursorMode WRITE setCursorMode NOTIFY CursorModeChanged)
    bool CursorMode() const;
    void setCursorMode(bool value);

    Q_PROPERTY(uint EraserPressureSensitive READ eraserPressureSensitive WRITE setEraserPressureSensitive NOTIFY EraserPressureSensitiveChanged)
    uint eraserPressureSensitive();
    void setEraserPressureSensitive(uint value);

Q_SIGNALS:
    void ExistChanged(bool exist);
    void CursorModeChanged(const bool cursorMode);
    void EraserPressureSensitiveChanged(const uint value);

private:
    QScopedPointer<WacomModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), WacomModel)
};

#endif // WACOMMODEL_H
