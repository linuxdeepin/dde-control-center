// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccrepeater.h"
#include "dccobject.h"
#include "dccapp.h"

#include <QtQml>
#include <private/qqmldelegatemodel_p.h>
#include <private/qqmlobjectmodel_p.h>

namespace dccV25 {

class DccRepeaterPrivate
{
public:
    DccRepeaterPrivate(DccRepeater *qq)
        : q_ptr(qq)
        , model(nullptr)
        , ownModel(false)
        , dataSourceIsObject(false)
        , delegateValidated(false)
        , itemCount(0) {

    }

    void requestItems()
    {
        if (!model)
            return;

        for (int i = 0; i < itemCount; i++) {
            QObject *object = model->object(i, QQmlIncubator::AsynchronousIfNested);
            if (object)
                model->release(object);
        }
    }

public:
    DccRepeater *q_ptr = nullptr;
    QPointer<QQmlInstanceModel> model;
    QVariant dataSource;
    QPointer<QObject> dataSourceAsObject;
    bool ownModel : 1;
    bool dataSourceIsObject : 1;
    bool delegateValidated : 1;
    int itemCount = 0;

    QVector<QPointer<DccObject> > deletables;

    Q_DECLARE_PUBLIC(DccRepeater)
};

DccRepeater::DccRepeater(QObject *parent)
    : QObject(parent)
    , d_ptr(new DccRepeaterPrivate(this))
{

}

DccRepeater::~DccRepeater()
{

}

QVariant DccRepeater::model() const
{
    Q_D(const DccRepeater);
    if (d->dataSourceIsObject) {
        QObject *o = d->dataSourceAsObject;
        return QVariant::fromValue(o);
    }

    return d->dataSource;
}

void DccRepeater::setModel(const QVariant &m)
{
    Q_D(DccRepeater);
    QVariant model = m;
    if (model.userType() == qMetaTypeId<QJSValue>())
        model = model.value<QJSValue>().toVariant();

    if (d->dataSource == model)
        return;

    clear();
    if (d->model) {
        qmlobject_disconnect(d->model, QQmlInstanceModel, SIGNAL(modelUpdated(QQmlChangeSet,bool)),
                             this, DccRepeater, SLOT(modelUpdated(QQmlChangeSet,bool)));
        qmlobject_disconnect(d->model, QQmlInstanceModel, SIGNAL(createdItem(int,QObject*)),
                             this, DccRepeater, SLOT(createdItem(int,QObject*)));
        qmlobject_disconnect(d->model, QQmlInstanceModel, SIGNAL(initItem(int,QObject*)),
                             this, DccRepeater, SLOT(initItem(int,QObject*)));
    }
    d->dataSource = model;
    QObject *object = qvariant_cast<QObject*>(model);
    d->dataSourceAsObject = object;
    d->dataSourceIsObject = object != nullptr;
    QQmlInstanceModel *vim = nullptr;
    if (object && (vim = qobject_cast<QQmlInstanceModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QQmlDelegateModel(qmlContext(this), this);
            d->ownModel = true;
            // if (isComponentComplete())
            static_cast<QQmlDelegateModel *>(d->model.data())->componentComplete();
        }
        if (QQmlDelegateModel *dataModel = qobject_cast<QQmlDelegateModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        qmlobject_connect(d->model, QQmlInstanceModel, SIGNAL(modelUpdated(QQmlChangeSet,bool)),
                          this, DccRepeater, SLOT(modelUpdated(QQmlChangeSet,bool)));
        qmlobject_connect(d->model, QQmlInstanceModel, SIGNAL(createdItem(int,QObject*)),
                          this, DccRepeater, SLOT(createdItem(int,QObject*)));
        qmlobject_connect(d->model, QQmlInstanceModel, SIGNAL(initItem(int,QObject*)),
                          this, DccRepeater, SLOT(initItem(int,QObject*)));
        regenerate();
    }
    emit modelChanged();
    emit countChanged();
}

QQmlComponent *DccRepeater::delegate() const
{
    Q_D(const DccRepeater);
    if (d->model) {
        if (QQmlDelegateModel *dataModel = qobject_cast<QQmlDelegateModel*>(d->model))
            return dataModel->delegate();
    }

    return nullptr;
}

void DccRepeater::setDelegate(QQmlComponent *delegate)
{
    Q_D(DccRepeater);
    if (QQmlDelegateModel *dataModel = qobject_cast<QQmlDelegateModel*>(d->model))
        if (delegate == dataModel->delegate())
            return;

    if (!d->ownModel) {
        QQmlDelegateModel *dataModel = new QQmlDelegateModel(qmlContext(this), this);
        d->model = dataModel;
        d->ownModel = true;
        dataModel->componentComplete();
    }

    if (QQmlDelegateModel *dataModel = qobject_cast<QQmlDelegateModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        regenerate();
        emit delegateChanged();
        d->delegateValidated = false;
    }
}

int DccRepeater::count() const
{
    Q_D(const DccRepeater);
    if (d->model)
        return d->model->count();
    return 0;
}

void DccRepeater::resetModel()
{
    modelUpdated(QQmlChangeSet(), true);
}

void DccRepeater::createdItem(int index, QObject *)
{
    Q_D(DccRepeater);
    QObject *object = d->model->object(index, QQmlIncubator::AsynchronousIfNested);

    DccObject *dccObj = qmlobject_cast<DccObject*>(object);
    dccObj->setParent(parent());
    DccApp::instance()->addObject(dccObj);

    emit objAdded(index, object);
}

void DccRepeater::initItem(int index, QObject *object)
{
    Q_D(DccRepeater);
    if (index >= d->deletables.size())
        d->deletables.resize(d->model->count() + 1);

    DccObject *dccObj = qmlobject_cast<DccObject*>(object);

    if (!d->deletables.at(index)) {
        if (!dccObj) {
            if (object) {
                d->model->release(object);
                if (!d->delegateValidated) {
                    d->delegateValidated = true;
                    QObject* delegate = this->delegate();
                    qmlWarning(delegate ? delegate : this) << "Delegate must be of `DccObject` type";
                }
            }
            return;
        }
        d->deletables[index] = dccObj;
    }
}

void DccRepeater::modelUpdated(const QQmlChangeSet &changeSet, bool reset)
{
    Q_D(DccRepeater);

    if (reset) {
        regenerate();
        if (changeSet.difference() != 0)
            emit countChanged();
        return;
    }

    int difference = 0;
    QHash<int, QVector<QPointer<DccObject> > > moved;
    for (const QQmlChangeSet::Change &remove : changeSet.removes()) {
        int index = qMin(remove.index, d->deletables.size());
        int count = qMin(remove.index + remove.count, d->deletables.size()) - index;
        if (remove.isMove()) {
            moved.insert(remove.moveId, d->deletables.mid(index, count));
            d->deletables.erase(
                    d->deletables.begin() + index,
                    d->deletables.begin() + index + count);
        } else while (count--) {
                DccObject *item = d->deletables.at(index);
                d->deletables.remove(index);
                emit objRemoved(index, item);
                if (item) {
                    d->model->release(item);
                    // item->setParentItem(nullptr);
                }
                --d->itemCount;
            }

        difference -= remove.count;
    }

    for (const QQmlChangeSet::Change &insert : changeSet.inserts()) {
        int index = qMin(insert.index, d->deletables.size());
        if (insert.isMove()) {
            QVector<QPointer<DccObject> > items = moved.value(insert.moveId);
            d->deletables = d->deletables.mid(0, index) + items + d->deletables.mid(index);

        } else for (int i = 0; i < insert.count; ++i) {
                int modelIndex = index + i;
                ++d->itemCount;
                d->deletables.insert(modelIndex, nullptr);
                QObject *object = d->model->object(modelIndex, QQmlIncubator::AsynchronousIfNested);
                if (object)
                    d->model->release(object);
            }
        difference += insert.count;
    }

    if (difference != 0)
        emit countChanged();
}

void DccRepeater::regenerate()
{
    Q_D(DccRepeater);
    clear();

    if (!d->model || !d->model->count() || !d->model->isValid() || !parent()/* || !isComponentComplete()*/)
        return;

    d->itemCount = count();
    d->deletables.resize(d->itemCount);
    d->requestItems();
}

void DccRepeater::clear()
{
    Q_D(DccRepeater);
    if (d->model) {
        for (int i = d->deletables.size() - 1; i >= 0; --i) {
            if (DccObject *obj = d->deletables.at(i)) {
                // reomve from dccApp
                DccApp::instance()->removeObject(obj);
                emit objRemoved(i, obj);
                d->model->release(obj);
            }
        }
    }
    d->deletables.clear();
    d->itemCount = 0;
}

}
