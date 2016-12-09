#ifndef KEYBOARDDETAILS_H
#define KEYBOARDDETAILS_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "settingshead.h"
#include "checkitem.h"
#include "indexmodel.h"

using namespace dcc;

namespace dcc {
namespace keyboard{
class KeyboardDetails : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardDetails(QWidget* parent = 0);
    void setDefaultString(const MetaData& layout);

signals:
    void layoutAdded();
    void curLayout(const QString& value);
    void curLang(const QString& value);
    void delUserLayout(const QString& value);

public slots:
    void onAddKeyboard(const MetaData& value);
    void onEdit(bool value);
    void setCurrentLayout(bool value);
    void onRemoveLayout(CheckItem* item);

private:
    bool m_bEdit;
    SettingsGroup* m_group;
    SettingsHead* m_head;
    CheckItem* m_default;
    QList<MetaData> m_datas;
    QMap<QString, QString> m_maps;
};
}
}
#endif // KEYBOARDDETAILS_H
