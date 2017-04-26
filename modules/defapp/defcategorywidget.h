#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modulewidget.h"
#include "translucentframe.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QJsonObject>
#include <QFileInfo>

namespace dcc
{
namespace widgets {
class SettingsHead;
class SettingsGroup;
}
namespace defapp
{
class DefCategoryAddWidget;
class Category;
class OptionWidget;
class DefCategoryWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit DefCategoryWidget(const QString &name, QWidget *parent);
    void setCategory(Category * const category);

signals:
    void requestSetDefaultApp(const QString &category, const QJsonObject &item);
    void requestDelUserApp(const QString &name, const QJsonObject &item);
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

public slots:
    void setDefault();
    void onDefaultAppSet(const QJsonObject &json);
    void slotEditMode(bool edit);
    void setCategoryName(const QString &name);

private:
    void AppsItemChanged(const QList<QJsonObject> &list);
    void UserItemChanged(const QList<QJsonObject> &list);
    void addItem(const QJsonObject &item);
    void addUserItem(const QJsonObject &item);
    void removeItem(const QJsonObject &item);

private:
    QVBoxLayout           *m_centralLayout;
    widgets::SettingsGroup    *m_userGroup;
    QWidget               *m_listWidget;
    QVBoxLayout           *m_listLayout;
    OptionWidget          *m_optionWidget;
    QLayoutItem           *m_userGroupLayout;
    DefCategoryAddWidget  *m_addWidget;
    widgets::SettingsHead *m_headWidget;
    QString                m_categoryName;
    Category              *m_category;
    QMap<QString, OptionWidget *> m_mainMap;
    QMap<QString, OptionWidget *> m_userMap;
    QMap<OptionWidget *, QJsonObject>  m_valueMap;
};

}
}

#endif // DEFCATEGORYWIDGET_H
