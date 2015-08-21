#ifndef INPUTLINE_H
#define INPUTLINE_H

#include <QWidget>
#include "libdui/dlineedit.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class InputLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit InputLine(QWidget *parent = 0);
    QString text() const;
    DLineEdit *lineEdit() const;

signals:
    void textChanged(const QString &text);

private:
    void setContent(QWidget *content);

private:
    DLineEdit *m_lineEdit = NULL;
};

#endif // INPUTLINE_H
