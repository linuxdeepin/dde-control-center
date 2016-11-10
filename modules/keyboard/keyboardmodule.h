#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

#include <QObject>
#include "moduleinterface.h"
#include "keyboardwidget.h"
#include "keyboardmodel.h"
#include "keyboardwork.h"
#include "langwidget.h"
#include "shortcutwidget.h"
#include "keyboarddetails.h"

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

public slots:
    void onPushKeyboard();
    void onPushKBDetails();
    void onPushLanguage();
    void onPushShortcut();
    void onKeyboardBack();
    void setCurrentLayout(const QString &value);
    void setCurrentLang();

private:
    ~KeyboardModule();
    void append(const MetaData& md);

private:
    bool m_loaded;
    QList<MetaData> m_datas;
    QList<QString> m_letters;
    KeyboardWork* m_work;
    KeyboardModel* m_model;
    KeyboardWidget* m_keyboardWidget;
    KeyboardDetails* m_kbDetails;
    KeyboardLayoutWidget* m_kbLayoutWidget;
    ShortcutWidget* m_shortcutWidget;
    LangWidget* m_langWidget;
};

#endif // KEYBOARDMODULE_H
