#ifndef CURRENCYFORMAT_H
#define CURRENCYFORMAT_H

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
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class CurrencyFormat : public QWidget
{
    Q_OBJECT
public:
    explicit CurrencyFormat(dcc::datetime::DatetimeModel *model, QWidget *parent = nullptr);
    ~CurrencyFormat();

    QString getFirstCurrencySymbolFormat() {return m_currencySymbolFormatPlace;}
    int getFirstPositiveCurrencyFormatPlace() {return m_positiveCurrencyFormatPlace;}
    int getFirstNegativeCurrencyPlace() {return m_negativeCurrencyFormatPlace;}

private:
    void initComboxWidgetList();

Q_SIGNALS:
    void currencySymbolFormatChanged(QString);
    void positiveCurrencyFormatChanged(int);
    void negativeCurrencyChanged(int);

public Q_SLOTS:

private:
    dcc::datetime::DatetimeModel *m_model;
    QVBoxLayout *m_layout;
    dcc::widgets::ComboxWidget *m_currencySymbolCbx;
    dcc::widgets::ComboxWidget *m_positiveCurrencyFormatCbx;
    dcc::widgets::ComboxWidget *m_negativeCurrencyFormatCbx;
    QString m_currencySymbolFormatPlace;
    int m_positiveCurrencyFormatPlace;
    int m_negativeCurrencyFormatPlace;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
#endif // CURRENCYFORMAT_H
