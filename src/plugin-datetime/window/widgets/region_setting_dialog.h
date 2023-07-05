#pragma once

#include <DDialog>

#include <QListWidget>
#include <QObject>
#include <QTextEdit>

class RegionDialog final : Dtk::Widget::DDialog
{
    Q_OBJECT

public:
    explicit RegionDialog(QMap<QString, QString> regions, QWidget *parent = nullptr);

private slots:
    void onFilterChanged(const QString &filter);

private:
    QTextEdit *m_edit;
    QListWidget *m_listWidget;
    QMap<QString, QString> m_regions;
};
