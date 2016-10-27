#ifndef NEXTPAGEWIDGET_H
#define NEXTPAGEWIDGET_H

#include "settingsitem.h"

#include <QLabel>
#include <dimagebutton.h>

namespace dcc {

class NextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit NextPageWidget(QFrame *parent = 0);

    void setTitle(const QString &title);
    void setValue(const QString &value);
//    void setDisplay(bool display);
//    bool display() const;

signals:
    void clicked() const;

private:
    QLabel *m_title;
    QLabel *m_value;
    Dtk::Widget::DImageButton *m_nextPageBtn;
};

}
#endif // NEXTPAGEWIDGET_H
