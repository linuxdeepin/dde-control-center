#ifndef DEFCATEGORYADDWIDGET_H
#define DEFCATEGORYADDWIDGET_H

#include <QObject>
#include "settingsitem.h"
#include "funcbutton.h"
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QTimer>

using namespace dcc::widgets;

class DefCategoryAddWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit DefCategoryAddWidget(QWidget *parent = 0);
    inline void setCategory(const QString &category){ m_category = category;}

signals:
    void addUserItem(const QString &category, const QJsonObject &item);
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void clicked();

private:
    FuncButton *m_add;
    QString m_category;
};


#endif // DEFCATEGORYADDWIDGET_H
