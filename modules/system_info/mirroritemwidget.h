/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MIRRORITEMWIDGET_H
#define MIRRORITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QDebug>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class MirrorItemWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ selected WRITE setSelected NOTIFY selectStateChanged)

signals:
    void selectStateChanged(bool state) const;
    void clicked() const;

public:
    explicit MirrorItemWidget(QWidget *parent = 0);

    inline bool selected() const {return m_selected;}
    void setSelected(bool state);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    inline void setMirrorName(const QString &name) {m_mirrorName->setText(name);}
    void testMirrorSpeed(const QString &mirrorAdr);

public slots:
    void testMirrorSpeed_finish(int ret);

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    DImageButton *m_selectedBtn;
    QLabel *m_mirrorName;
    QLabel *m_mirrorSpeed;

    bool m_selected = false;
};

#endif // MIRRORITEMWIDGET_H
