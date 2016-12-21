#ifndef THEME_H
#define THEME_H

#include "settingsitem.h"
#include "translucentframe.h"
#include <settingsgroup.h>
#include <QObject>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QMap>

namespace dcc
{
namespace widgets
{
class OptionItem;
class SettingsHead;
class SettingsGroup;
}
namespace personalization
{
class ThemeModel;
class Theme : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit Theme(const QString &title, QWidget *parent = 0);
    void setModel(ThemeModel *const model);

signals:
    void requestSetDefault(const QJsonObject &value);

public slots:
    void setList(const QList<QJsonObject> &list);
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);

private:
    QVBoxLayout          *m_mainLayout;
    widgets::SettingsGroup        *m_mainGroup;
    widgets::SettingsHead         *m_head;
    QMap<widgets::OptionItem *, QJsonObject> m_valueMap;
    ThemeModel           *m_model;
};
}
}

#endif // THEME_H
