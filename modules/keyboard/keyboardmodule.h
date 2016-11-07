#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

#include <QObject>
#include "moduleinterface.h"
#include "keyboardwidget.h"
#include "keyboardmodel.h"
#include "keyboardwork.h"

class KeyboardModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit KeyboardModule(FrameProxyInterface* frame, QObject* parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();

    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

private:
    ~KeyboardModule();

private:
    KeyboardWork* m_work;
    KeyboardModel* m_model;
    KeyboardWidget* m_keyboardWidget;
};

#endif // KEYBOARDMODULE_H
