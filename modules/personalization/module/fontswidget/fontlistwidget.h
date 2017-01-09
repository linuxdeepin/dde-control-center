#ifndef FONTLISTWIDGET_H
#define FONTLISTWIDGET_H

#include "contentwidget.h"
#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>
#include <QApplication>
#include <QJsonObject>
#include <QList>
#include <QLabel>
#include <QFont>
#include <QMap>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
class SettingsGroup;
class OptionItem;
}
namespace personalization
{
class FontModel;
class FontListWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit FontListWidget(const QString &title, QWidget *parent = 0);

signals:
    void requestSetDefault(const QJsonObject &value);

public slots:
    void setModel(FontModel *const model);
    void setList(const QList<QJsonObject> &list);
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);

private:
    FontModel                               *m_model;
    QVBoxLayout                             *m_mainLayout;
    widgets::TranslucentFrame               *m_widget;
    widgets::SettingsGroup                  *m_mainGroup;
    QMap<widgets::OptionItem *, QJsonObject> m_valueMap;
};
}
}


#endif // FONTLISTWIDGET_H
