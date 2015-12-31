#ifndef PRESSPOWERBUTTONACTION
#define PRESSPOWERBUTTONACTION

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dheaderline.h>
#include <libdui/dbaseexpand.h>

DUI_USE_NAMESPACE
class PressPowerButtonAction: public QFrame
{
    Q_OBJECT
public:
    PressPowerButtonAction(QStringList powerReaction, QWidget* parent=0);
    ~PressPowerButtonAction();

signals:
    void powerButtonAction(QString action);
    void powerButtonIndexChanged(int index);
public slots:
    void initConnection();
    void setPowerButtonAction(int buttonId);
    void setTitle(QString titleName);
private:

    DHeaderLine* m_pressPowerReActionLine;
    DBaseExpand* m_pressPowerReActionExpand;
    DButtonGrid* m_powerActionButtonGrid;
    DSeparatorHorizontal* m_pressPowerSeparatorLine;
    QVBoxLayout* pressPowerButtonActionLayout;

};
#endif // PRESSPOWERBUTTONACTION

