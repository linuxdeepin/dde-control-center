#ifndef ACCOUNTEXPAND_H
#define ACCOUNTEXPAND_H

#include <QWidget>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include "libdui/dbaseexpand.h"
#include "userexpandheader.h"
#include "userexpandcontent.h"

DUI_USE_NAMESPACE

class UserExpand : public DBaseExpand
{
    Q_OBJECT
    Q_PROPERTY(int height READ height WRITE setHeight)
public:
    explicit UserExpand(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool v);
    void setExpand(bool v);
    void showNormal();
    void hide();
    QString userPath() const;
    QString userName() const;
    bool userLocked() const;

signals:
    void requestPreDestroy();
    void changeToSetting(bool setting);
    void requestDelete(bool flag);
    void cancelDelete();
    void userLockChanged();

private:
    void setHeight(int height);

private:
    QString m_userPath = "";
    UserExpandHeader *m_header = NULL;
    UserExpandContent *m_content = NULL;
    QPropertyAnimation *m_animation = NULL;
    bool m_inSetting = false;
    bool m_isCurrentUser = false;

    const int HEADER_HEIGHT = 100;
    const int HEADER_CURRENT_USER_HEIGHT = 118;
    const int ANIMATION_DURATION = 100;
    const QEasingCurve ANIMATION_CURVE = QEasingCurve::OutQuad;
};

#endif // ACCOUNTEXPAND_H
