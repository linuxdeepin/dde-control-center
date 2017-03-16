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
class SettingsGroup;
}
namespace personalization
{
class ThemeModel;
class ThemeItem;
class Theme : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit Theme(const QString &title, QWidget *parent = 0);
    void setModel(ThemeModel *const model);

signals:
    void requestSetDefault(const QJsonObject &value);

public slots:
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);

private:
    QVBoxLayout                             *m_mainLayout;
    widgets::SettingsGroup                  *m_mainGroup;
    QMap<ThemeItem *, QJsonObject> m_valueMap;
    ThemeModel                              *m_model;
};
}
}

#endif // THEME_H
