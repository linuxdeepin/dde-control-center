#ifndef MULTIADDCHECKBUTTON_H
#define MULTIADDCHECKBUTTON_H

#include <libdui/dimagebutton.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class MultiAddCheckButton : public DImageButton
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
public:
    explicit MultiAddCheckButton(QWidget *parent = 0);

    bool checked() const;

public slots:
    void setChecked(bool checked);

signals:
    void checkedChanged(bool checked);

private:
    bool m_checked;

    void updateIcon();
};

#endif // MULTIADDCHECKBUTTON_H
