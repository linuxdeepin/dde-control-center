#ifndef ADDRMDONELINE_H
#define ADDRMDONELINE_H

#include <libdui/dheaderline.h>
#include <libdui/libdui_global.h>
#include <libdui/dtextbutton.h>

DUI_USE_NAMESPACE

class ImageNameButton;
class AddRmDoneLine : public DHeaderLine
{
    Q_OBJECT

    Q_PROPERTY(QString rmButtonToolTip READ rmButtonToolTip WRITE setRmButtonToolTip)
    Q_PROPERTY(QString addButtonToolTip READ addButtonToolTip WRITE setAddButtonToolTip)
    Q_PROPERTY(QString doneButtonToolTip READ doneButtonToolTip WRITE setDoneButtonToolTip)
public:
    explicit AddRmDoneLine(QWidget *parent = 0);

    ImageNameButton *removeButton() const;
    ImageNameButton *addButton() const;
    DTextButton *doneButton() const;
    QString rmButtonToolTip() const;
    QString addButtonToolTip() const;
    QString doneButtonToolTip() const;

public slots:
    void setRmButtonToolTip(QString rmButtonToolTip);
    void setAddButtonToolTip(QString addButtonToolTip);
    void setDoneButtonToolTip(QString doneButtonToolTip);

signals:
    void addClicked();
    void removeClicked();
    void doneClicked();
    void setAddHidden(bool hidden);
    void setRemoveHidden(bool hidden);
    void setDoneHidden(bool hidden);

private:
    ImageNameButton *m_removeButton;
    ImageNameButton *m_addButton;
    DTextButton *m_doneButton;
    QString m_rmButtonToolTip;
    QString m_addButtonToolTip;
    QString m_doneButtonToolTip;
};

#endif // ADDRMDONELINE_H
