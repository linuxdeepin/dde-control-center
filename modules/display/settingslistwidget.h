#ifndef SETTINGSLISTWIDGET_H
#define SETTINGSLISTWIDGET_H

#include "settingsoptionitem.h"

#include <QWidget>
#include <QLabel>

#include <QVBoxLayout>

namespace dcc {

namespace display {

class SettingsListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsListWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void appendOption(const QString &option);
    void setSelectedIndex(const int selected);

public slots:
    void clear();

signals:
    void clicked(const int index) const;

private slots:
    void onItemClicked();

private:
    QLabel *m_title;
    QVBoxLayout *m_listLayout;
};

} // namespace display

} // namespace dcc

#endif // SETTINGSLISTWIDGET_H
