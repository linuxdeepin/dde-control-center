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
#include "keyboardcontrol.h"
#include "shortcutcontent.h"
#include "customcontent.h"

#ifndef DCC_DISABLE_KBLAYOUT
#include "keyboarddetails.h"
#endif

namespace dcc {
namespace keyboard{
class CustomEdit;
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
#ifndef DCC_DISABLE_KBLAYOUT
    void onPushKeyboard();
    void onPushKBDetails();
#endif
#ifndef DCC_DISABLE_LANGUAGE
    void onPushLanguage();
    void onSetLocale(const QModelIndex &index);
#endif
    void onPushShortcut();
    void onPushCustomShortcut();
    void setCurrentLayout(const QString &value);
    void setCapsLock(bool value);

    void onShortcutChecked(bool valid, ShortcutInfo *info, const QString& shortcut);
    void onShortcutSet(const QString& shortcut);
    void onShortcutKeySet(const QString& shortcut);
    void onDelShortcut(ShortcutInfo* info);

    void onDelay(int value);
    void onSpeed(int value);

    void onShortcutEdit(ShortcutInfo* info);

private slots:
    void onSetFrameAutoHide(const bool autoHide);

private:
    ~KeyboardModule();

private:
    bool m_loaded;
    KeyboardWork* m_work;
    ShortcutModel* m_shortcutModel;
    KeyboardModel* m_model;
    KeyboardWidget* m_keyboardWidget;
#ifndef DCC_DISABLE_KBLAYOUT
    KeyboardDetails* m_kbDetails;
    KeyboardLayoutWidget* m_kbLayoutWidget;
#endif
    ShortcutWidget* m_shortcutWidget;
#ifndef DCC_DISABLE_LANGUAGE
    LangWidget* m_langWidget;
#endif
    ShortcutContent* m_scContent;
    CustomContent* m_customContent;
    CustomEdit    *m_customEdit;
};
}
}
#endif // KEYBOARDMODULE_H
