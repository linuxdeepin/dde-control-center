#ifndef PARAMETERSCONTAINER_H
#define PARAMETERSCONTAINER_H

#include <QObject>
#include <QMap>
#include <QVariant>

class ParametersContainer : public QObject
{
    Q_OBJECT

public:
    typedef QSharedPointer<ParametersContainer> Ptr;
    typedef enum {
        Uuid = 0, DevicePath, AccessPath, isHidden
    } ParamType;

    explicit ParametersContainer(QObject *parent = nullptr);
    ~ParametersContainer();
    void saveParameters(const ParamType &key, const QVariant &getValue);
    QVariant getValue(const ParamType &key);

private:
    QMap<ParamType, QVariant> m_pamameters;
};

#endif // CONNECTIONPARAMETERS_H
