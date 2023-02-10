#ifndef ADDBUTTONWIDGET_H
#define ADDBUTTONWIDGET_H

#include "defappworker.h"
#include "interface/namespace.h"

#include <DFloatingButton>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QFileDialog;
QT_END_NAMESPACE

class DefAppWorker;

class AddButtonWidget : public QWidget
{
    Q_OBJECT
public:
    AddButtonWidget(DefAppWorker::DefaultAppsCategory category = DefAppWorker::Browser,
                    QWidget *parent = nullptr);
    ~AddButtonWidget();

    void setModel(DefAppModel *const model);
    void setCategory(Category *const category);
    void setDefaultAppsCategory(DefAppWorker::DefaultAppsCategory category);

Q_SIGNALS:
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

public Q_SLOTS:
    void setCategoryName(const QString &name);
    void onAddBtnClicked();

private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_addBtn;
    DefAppWorker::DefaultAppsCategory m_categoryValue;
    QString m_categoryName;
    Category *m_category;
    QFileDialog *m_createFile;
};

#endif // ADDBUTTONWIDGET_H
