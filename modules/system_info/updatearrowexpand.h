#ifndef UPDATEARROWEXPAND_H
#define UPDATEARROWEXPAND_H

#include <libdui/darrowlineexpand.h>
#include <libdui/dimagebutton.h>

#include <QLabel>

DUI_USE_NAMESPACE

class UpdateArrowExpand : public DArrowLineExpand
{
    Q_OBJECT

public:
    explicit UpdateArrowExpand(QWidget *parent = 0);

    void setUpdatableNums(int nums);

signals:
    void configButtonClicked() const;

private:
    QLabel *m_updateCountLabel;
    DImageButton *m_configBtn;
};

#endif // UPDATEARROWEXPAND_H
