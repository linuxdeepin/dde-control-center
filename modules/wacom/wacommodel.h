#ifndef WACOMMODEL_H
#define WACOMMODEL_H

#include <QObject>

namespace  dcc
{
namespace wacom
{
class WacomModelBase;
class WacomModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit WacomModel(QObject *parent = 0);
    ~WacomModel();
    WacomModelBase *getWacomModelBase();

    bool exist() const;
    void setExist(bool exist);

    bool getCursorMode() const;
    void setCursorMode(bool cursorMode);

signals:
    void existChanged(bool exist);
    void cursorModeChanged(const bool cursorMode);

private:
    WacomModelBase *m_wacomModelBase;
    bool m_exist;
    bool m_cursorMode;
};
}
}

#endif // WACOMMODEL_H
