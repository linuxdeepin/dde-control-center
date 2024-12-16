// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "avatarlistview.h"
#include "interface/namespace.h"
#include "widgets/accessibleinterface.h"

#include <DFrame>

#include <QScrollArea>
#include <QSpacerItem>

DWIDGET_BEGIN_NAMESPACE
class DSlider;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QPoint;
class QColor;
class QTimer;
class QFileDialog;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class AvatarCropBox;
class AvatarListView;

class AvatarListFrame : public QFrame
{
    Q_OBJECT
public:
    struct AvatarRoleItem
    {
        int role;
        int type;
        QString path;
        bool isLoader;

        AvatarRoleItem(const int &_role,
                       const int &_type,
                       const QString &_path,
                       const bool &_isLoader)
            : role(_role)
            , type(_type)
            , path(_path)
            , isLoader(_isLoader)
        {
        }
    };

    explicit AvatarListFrame(User * user, const int &role, QWidget *parent = nullptr);
    virtual ~AvatarListFrame() = default;

    inline int getCurrentRole() { return m_role; }
    inline QString getCurrentPath() { return m_path; }
    inline AvatarListView *getCurrentListView() { return m_currentAvatarLsv; }

    QString getAvatarPath() const;

    bool isExistCustomAvatar(const QString &path, const QString &userName);

public Q_SLOTS:
    void updateListView(bool isSave, const int &role, const int &type);

private:
    QString getCurrentAvatarPath(const int role, const int type);

private:
    int m_role;
    QString m_path;
    QMap<int, AvatarListView *> m_avatarViewMap;
    AvatarListView *m_currentAvatarLsv;
};

class CustomAddAvatarWidget : public AvatarListFrame
{
    Q_OBJECT
public:
    explicit CustomAddAvatarWidget(User *user, const int &role, QWidget *parent = nullptr);
    virtual ~CustomAddAvatarWidget();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void requestUpdateCustomWidget(const QString &path);

private:
    void saveCustomAvatar(const QString &path);

private:
    Dtk::Widget::DFrame *m_addAvatarFrame;
    QLabel *m_hintLabel;
    QSpacerItem *m_addAvatarIconSpacer;
    Dtk::Gui::DDciIcon m_addAvatarDciIcon;
    bool m_isDragIn;
    bool m_isHover;
    bool m_isPress;
};

class CustomAvatarView : public QWidget
{
    Q_OBJECT

public:
    explicit CustomAvatarView(QWidget *parent = nullptr);
    ~CustomAvatarView();

    void setAvatarPath(const QString &avatarPath);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void enableAvatarScaledItem(const bool enabled);
    void requestSaveCustomAvatar(const QString &path);

public Q_SLOTS:
    void startAvatarModify();
    void endAvatarModify();
    void stopAutoExitTimer();
    void setZoomValue(const int value);
    QString getCroppedImage();

private:
    int m_xPtInterval = 0;
    int m_yPtInterval = 0;
    int m_offset = 0;
    int m_currentScaledValue = 0;
    bool m_Pressed = false;
    QTimer *m_autoExitTimer;
    QImage m_image;
    AvatarCropBox *m_cropBox;
    qreal m_zoomValue = 1.0;
    QPoint m_OldPos;
    QString m_path;

private slots:
    void onZoomInImage(void);
    void onZoomOutImage(void);
    void onPresetImage(void);
};

class CustomAvatarWidget : public AvatarListFrame
{
    Q_OBJECT
public:
    explicit CustomAvatarWidget(User *user, const int &role, QWidget *parent = nullptr);
    ~CustomAvatarWidget() override = default;

    void enableAvatarScaledItem(bool enabled);
    void stopAvatarModify();

    inline CustomAvatarView *getCustomAvatarView() { return m_avatarView; }

private:
    Dtk::Widget::DSlider *m_avatarScaledItem;
    CustomAvatarView *m_avatarView;
};

} // namespace DCC_NAMESPACE
