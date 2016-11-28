#ifndef THEME_H
#define THEME_H

#include "settingsitem.h"
#include "translucentframe.h"
#include "settingsheaderitem.h"
#include "labels/largelabel.h"
#include "../../widget/themeitempic.h"
#include "../../model/thememodel.h"
#include "optionitem.h"
#include "personalization/personalizationmodel.h"
#include "settingshead.h"
#include <settingsgroup.h>
#include <QObject>
#include <QVBoxLayout>


using namespace dcc;
using namespace dcc::widgets;

class Theme : public TranslucentFrame
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
    SettingsGroup        *m_mainGroup;
    SettingsHead         *m_head;
    QMap<OptionItem *, QJsonObject> m_valueMap;
    ThemeModel           *m_model;
};

#endif // THEME_H
