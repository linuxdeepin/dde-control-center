#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "widgets/optionwidget.h"
#include "defappmodel.h"
#include "widgets/defcategoryheadwidget.h"
#include "widgets/defcategoryaddwidget.h"
#include <QWidget>
#include <QVBoxLayout>

namespace dcc {

class DefCategoryWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit DefCategoryWidget(const QString &name);
    void setCategory(Category * const category);

signals:
    void setDefaultApp(const QString &category, const QJsonObject &item);
    void addUserApp(const QString &category, const QJsonObject &item);
    void delUserApp(const QJsonObject &item);
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void onSetRunner(const QString &category, const QString &value);
    void onDefaultAppSet(const QString &id);
    void slotEditMode(const bool editable);

private:
    void AppsItemChanged();
    void UserItemChanged();
    void addItem(const QJsonObject &item);
    void addUserItem(const QJsonObject &item);
    void removeItem(const QJsonObject &item);
    dcc::OptionWidget * getObject(const QString &id);
    int itemCount();

private:
    dcc::SettingsGroup    *m_userGroup;
    QWidget               *m_listWidget;
    QVBoxLayout           *m_listLayout;
    dcc::OptionWidget     *m_optionWidget;
    QLayoutItem           *m_userGroupLayout;
    DefCategoryHeadWidget *m_headWidget;
    DefCategoryAddWidget  *m_addWidget;
    QString                m_categoryName;
    Category              *m_category;

};

}
#endif // DEFCATEGORYWIDGET_H
