#ifndef NUMBERFORMAT_H
#define NUMBERFORMAT_H

#include "interface/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace datetime {
class DatetimeModel;
}
}

namespace dcc {
namespace widgets {
class ComboxWidget;
class TipsLabel;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class NumberFormat : public QWidget
{
    Q_OBJECT
public:
    explicit NumberFormat(dcc::datetime::DatetimeModel *model, QWidget *parent = nullptr);
    ~NumberFormat();

private:
    void initComboxWidgetList();
    void onComboxChanged();

Q_SIGNALS:
    void decimalSymbolChanged(QString);
    void digitGroupingSymbolChanged(QString);
    void digitGroupingChanged(QString);

public Q_SLOTS:
    void SetCurrencySymbolFormat(QString value);
    void SetPositiveCurrencyFormat(int value);
    void SetNegativeCurrency(int value);
    void updateExample(int numplace = -1);

private:
    dcc::datetime::DatetimeModel *m_model;
    QVBoxLayout *m_layout;
    dcc::widgets::ComboxWidget *m_decimalSymbolCbx;
    dcc::widgets::ComboxWidget *m_digitGroupingSymbolCbx;
    dcc::widgets::ComboxWidget *m_digitGroupingCbx;
    dcc::widgets::TipsLabel *m_exampleTips;
    QString m_currencySymbolFormat;
    int m_positiveCurrencyFormat;
    int m_negativeCurrency;
    QStringList m_digitGroupingKeepList;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
#endif // NUMBERFORMAT_H
