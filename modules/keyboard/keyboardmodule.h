#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

#include <QObject>
#include "moduleinterface.h"
#include "keyboardwidget.h"
#include "keyboardmodel.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include "langwidget.h"
#include "shortcutwidget.h"
#include "keyboarddetails.h"
#include "keyboardcontrol.h"
#include "shortcutcontent.h"
#include "customcontent.h"

namespace dcc {
namespace keyboard{
class KeyboardModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit KeyboardModule(FrameProxyInterface* frame, QObject* parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

    ShortcutInfo* checkConflict(const QString& shortcut, QStringList &list);
    QString converKey(const QString &key);

public slots:
    void onPushKeyboard();
    void onPushKBDetails();
    void onPushLanguage();
    void onPushShortcut();
    void onPushCustonShortcut();

    void onKeyboardLayoutSelected();

    void setCurrentLayout(const QString &value);
    void setCurrentLang();
    void setCapsLock(bool value);

    void onSetLocale(const QModelIndex &index);
    void onShortcutChecked(bool valid, ShortcutInfo *info, const QString& shortcut);
    void onShortcutSet(const QString& shortcut);
    void onShortcutKeySet(const QString& shortcut);
    void onDelShortcut(ShortcutInfo* info);

    void onDelay(int value);
    void onSpeed(int value);

private:
    ~KeyboardModule();

private:
    bool m_loaded;
    KeyboardWork* m_work;
    ShortcutModel* m_shortcutModel;
    KeyboardModel* m_model;
    KeyboardWidget* m_keyboardWidget;
    KeyboardDetails* m_kbDetails;
    KeyboardLayoutWidget* m_kbLayoutWidget;
    ShortcutWidget* m_shortcutWidget;
    LangWidget* m_langWidget;
    ShortcutContent* m_scContent;
    CustomContent* m_customContent;
};
}
}
#endif // KEYBOARDMODULE_H
