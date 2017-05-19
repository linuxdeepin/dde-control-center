#ifndef KEYBOARDDETAILS_H
#define KEYBOARDDETAILS_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "settingshead.h"
#include "checkitem.h"
#include "indexmodel.h"
#include "keyboardmodel.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
class KeyboardDetails : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardDetails(QWidget* parent = 0);
    void setModel(KeyboardModel * model);

signals:
    void layoutAdded();
    void requestCurLayoutAdded(const QString& value);
    void curLang(const QString& value);
    void delUserLayout(const QString& value);
    void requestSwitchKBLayout(int value);

public slots:
    void onAddKeyboard(const QString &id, const QString &value);
    void onEdit(bool value);
    void onRemoveLayout(CheckItem* item);
    void onDefault(const QString &value);
    void onSwitchKBChanged();
    void onSwitchKB(int kbSwitch);

private:
    bool m_bEdit;
    SettingsGroup* m_group;
    SettingsHead* m_head;
    QList<MetaData> m_datas;
    QMap<QString, CheckItem*> m_maps;
    KeyboardModel *m_model;
    SettingsGroup *m_switchKBLayout;
    QMap<CheckItem*, int> m_switchCheckItem;
};
}
}
#endif // KEYBOARDDETAILS_H
