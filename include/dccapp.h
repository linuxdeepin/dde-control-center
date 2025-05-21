// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCAPP_H
#define DCCAPP_H
#include "dccobject.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QWindow;
class QAbstractItemModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

namespace dccV25 {
class DccApp : public QObject
{
    Q_OBJECT
public:
    enum UosEdition {
        UosEditionUnknown,
        UosProfessional,
        UosHome,
        UosCommunity,
        UosMilitary,
        UosEnterprise,
        UosEnterpriseC,
        UosEuler,
        UosMilitaryS, // for Server
        UosDeviceEdition,
        UosEducation,

        UosEditionCount // must at last
    };
    Q_ENUM(UosEdition)

    enum AnimationMode {
        AnimationPush,
        AnimationPop
    };
    Q_ENUM(AnimationMode)

    static DccApp *instance();

    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(DccObject * root READ root NOTIFY rootChanged)
    Q_PROPERTY(DccObject * activeObject READ activeObject NOTIFY activeObjectChanged)
    Q_PROPERTY(AnimationMode animationMode READ animationMode WRITE setAnimationMode NOTIFY animationModeChanged)

    virtual int width() const;
    virtual int height() const;
    virtual DccObject *root() const;
    virtual DccObject *activeObject() const;
    virtual AnimationMode animationMode() const {
        return m_animationMode;
    }
    virtual void setAnimationMode(AnimationMode mode);

public Q_SLOTS:
    virtual DccObject *object(const QString &name);
    virtual void addObject(DccObject *obj);
    virtual void removeObject(DccObject *obj);
    virtual void removeObject(const QString &name);
    virtual void showPage(const QString &url);
    virtual void showPage(DccObject *obj, const QString &cmd);
    virtual QWindow *mainWindow() const;
    virtual QAbstractItemModel *navModel() const;
    virtual QSortFilterProxyModel *searchModel() const;

Q_SIGNALS:
    void rootChanged(DccObject *root);
    void activeObjectChanged(DccObject *activeObject);
    void activeItemChanged(QQuickItem *item);
    void animationModeChanged(AnimationMode mode);

protected:
    explicit DccApp(QObject *parent = nullptr);
    ~DccApp() override;
private:
    AnimationMode m_animationMode;
};
} // namespace dccV25

#endif // DCCAPP_H
