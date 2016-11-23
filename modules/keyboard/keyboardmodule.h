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

public slots:
    void onPushKeyboard();
    void onPushKBDetails();
    void onPushLanguage();
    void onPushShortcut();
    void onPushShortcutControl(const QString& shortcut);
    void onPushCustonShortcut();

    void onKeyboardBack();
    void onParseFinish();

    void setCurrentLayout(const QString &value);
    void setCurrentLang();
    void setUILayout(const QString& value);
    void setUILang(const QString& value);

    void onSetLocale(const QModelIndex &index);
    void onShortcutChecked(bool valid, ShortcutInfo *info, const QString& shortcut);
    void onShortcutSet(const QString& shortcut);

    void onAdded(const QString& in0, int in1);
    void onDelShortcut(ShortcutInfo* info);
private:
    ~KeyboardModule();
    void append(const MetaData& md);

private:
    bool m_loaded;
    QList<MetaData> m_datas;
    QList<QString> m_letters;
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

#endif // KEYBOARDMODULE_H
