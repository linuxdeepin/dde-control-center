#ifndef SUMMARYITEM_H
#define SUMMARYITEM_H

#include "settingsitem.h"

#include <QLabel>
#include <dloadingindicator.h>
using namespace dcc;

DWIDGET_USE_NAMESPACE

class CheckUpdateItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit CheckUpdateItem(QFrame* parent = 0);
    void setIndicatorVisible(bool visible);
    void setImageSource(const QString& src);
    void setLoading(bool value);
    void setTitle(const QString& title);

private:
    QLabel* m_title;
    DLoadingIndicator* m_indicator;
};

class SummaryItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit SummaryItem(QFrame * parent = 0);

    void setTitle(const QString& title);
    void setDetails(const QString& details);

private:
    QLabel* m_title;
    QLabel* m_details;
};

#endif // SUMMARYITEM_H
