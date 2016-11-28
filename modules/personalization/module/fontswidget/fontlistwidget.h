#ifndef FONTLISTWIDGET_H
#define FONTLISTWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "optionitem.h"
#include "../../widget/fontitem.h"
#include "personalization/personalizationmodel.h"
#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>
#include <QApplication>
#include <QJsonObject>
#include <QList>
#include <QLabel>
#include <QFont>
#include <QMap>
using namespace dcc;
using namespace dcc::widgets;
class FontListWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit FontListWidget(const QString &title, QWidget *parent =0);

signals:
    void requestSetDefault(const QJsonObject &value);


public slots:
    void setModel(FontModel * const model);
    void setList(const QList<QJsonObject> &list);
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);

private:
    QVBoxLayout                 *m_mainLayout;
    TranslucentFrame            *m_widget;
    SettingsGroup               *m_mainGroup;
    QMap<OptionItem *, QJsonObject> m_valueMap;
};

#endif // FONTLISTWIDGET_H
