#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include "modulewidget.h"

#include "keyboardlayoutwidget.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

using namespace dcc;

class KeyboardWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget();

signals:
    void keyoard();
    void language();
    void shortcut();

public slots:
    void setKBValue(const QString& value);
    void setLangValue(const QString& value);

private:
    NextPageWidget* m_keyItem;
    NextPageWidget* m_langItem;
    NextPageWidget* m_scItem;
    SwitchWidget* m_upper;
};

#endif // KEYBOARDWIDGET_H
