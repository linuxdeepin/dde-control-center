// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCAPP_H
#define DCCAPP_H
#include "dccobject.h"

#include <QObject>
class QWindow;

namespace dccV25 {
class DccApp : public QObject
{
    Q_OBJECT
public:
    static DccApp *instance();

    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(DccObject * root READ root NOTIFY rootChanged)
    Q_PROPERTY(DccObject * activeObject READ activeObject NOTIFY activeObjectChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)

    virtual int width() const;
    virtual int height() const;
    virtual QString path() const;
    virtual DccObject *root() const;
    virtual DccObject *activeObject() const;

public Q_SLOTS:
    virtual DccObject *object(const QString &name);
    virtual void addObject(DccObject *obj);
    virtual void removeObject(DccObject *obj);
    virtual void removeObject(const QString &name);
    virtual void showPage(const QString &url);
    virtual void showPage(DccObject *obj, const QString &cmd);
    virtual QWindow *mainWindow();
    virtual void setShowPath(const QString &path);
    virtual void addSearch(const QString &key, const QString &url);
    virtual void removeSearch(const QString &key);

Q_SIGNALS:
    void pathChanged(const QString &path);
    void rootChanged(DccObject *root);
    void activeObjectChanged(DccObject *activeObject);

protected:
    explicit DccApp(QObject *parent = nullptr);
    ~DccApp() override;
};
} // namespace dccV25

#endif // DCCAPP_H
