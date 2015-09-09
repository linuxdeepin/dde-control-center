#ifndef INPUTLINE_H
#define INPUTLINE_H

#include <QWidget>
#include "inputwarningframe.h"
#include "libdui/dlineedit.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class InputLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit InputLine(QWidget *parent = 0);
    ~InputLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DLineEdit *lineEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    void setContent(QWidget *content);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DLineEdit *m_lineEdit = NULL;
};

#endif // INPUTLINE_H
