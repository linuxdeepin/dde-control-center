#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QFrame>
#include <QLineEdit>
#include <dimagebutton.h>

#include "settingsitem.h"
#include "labels/normallabel.h"

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

class DateWidget : public SettingsItem
{
    Q_OBJECT

public:
    enum Type{
        Year,
        Month,
        Day
    };

public:
    explicit DateWidget(Type type, QFrame *parent = 0);

    int data() const;

public slots:
    void slotAdd();
    void slotReduced();

private:
    Type m_type;
    int m_minimum;
    int m_maximum;
    QLineEdit *m_lineEdit;
    NormalLabel *m_label;
    DImageButton* m_addBtn;
    DImageButton* m_reducedBtn;
};

class TimeWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimeWidget(QFrame* parent = 0);

    int hour() const;
    int minute() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLineEdit *m_hourEdit;
    QLineEdit *m_minuteEdit;
};
}
}

#endif // DATETIMEWIDGET_H
