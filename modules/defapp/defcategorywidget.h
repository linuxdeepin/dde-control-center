#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "widgets/optionwidget.h"
#include "defappmodel.h"
#include "widgets/defcategoryaddwidget.h"
#include "settingshead.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>

using namespace dcc;

class DefCategoryWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit DefCategoryWidget(const QString &name, QWidget *parent);
    void setCategory(Category * const category);

signals:
    void requestSetDefaultApp(const QString &category, const QJsonObject &item);
    void addUserApp(const QString &category, const QJsonObject &item);
    void delUserApp(const QJsonObject &item);
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void setDefault();
    void onDefaultAppSet(const QString &id);
    void slotEditMode(bool editable);

private:
    void AppsItemChanged(const QList<QJsonObject> &list);
    void UserItemChanged(const QList<QJsonObject> &list);
    void addItem(const QJsonObject &item);
    void addUserItem(const QJsonObject &item);
    void removeItem(const QJsonObject &item);

private:
    dcc::SettingsGroup    *m_userGroup;
    QWidget               *m_listWidget;
    QVBoxLayout           *m_listLayout;
    OptionWidget          *m_optionWidget;
    QLayoutItem           *m_userGroupLayout;
    DefCategoryAddWidget  *m_addWidget;
    SettingsHead          *m_headWidget;
    QString                m_categoryName;
    Category              *m_category;
    QMap<QString, OptionWidget *> m_mainMap;
    QMap<QString, OptionWidget *> m_userMap;
    QMap<OptionWidget *, QJsonObject>  m_valueMap;
};

#endif // DEFCATEGORYWIDGET_H
