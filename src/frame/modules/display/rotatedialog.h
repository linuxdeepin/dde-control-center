/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>
#include <QTimer>

#include "DWindowManagerHelper"

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(Monitor *mon, QWidget *parent = 0);
    explicit RotateDialog(DisplayModel *model, QWidget *parent = 0);
    ~RotateDialog();

Q_SIGNALS:
    void requestRotateAll(const quint16 rotate) const;
    void requestRotate(Monitor *mon, const quint16 rotate) const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *e) override;

private Q_SLOTS:
    void adjustGemotry();

private:
    void init();
    void rotate();

private:
    Monitor *m_mon;
    DisplayModel *m_model;
    QTimer *m_adjustDelayTimer;
    QTimer *m_resetOperationTimer;
    Dtk::Gui::DWindowManagerHelper *m_wmHelper;

    QString m_tips;
    bool m_mouseLeftHand;
    bool m_changed;
    int m_resetTimeout;
};

} // namespace display

} // namespace dcc

#endif // ROTATEDIALOG_H
