#ifndef PRESSPOWERBUTTONACTION
#define PRESSPOWERBUTTONACTION

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dheaderline.h>
#include <libdui/dbaseexpand.h>

DUI_USE_NAMESPACE
class PressPowerButtonAction: public QFrame {
    Q_OBJECT
public:
    PressPowerButtonAction(QStringList powerReaction, QWidget* parent=0);
    ~PressPowerButtonAction();
    QStringList powerReaction;
signals:
    void powerButtonAction(QString action);

public slots:
    void initConnection();
    void setPowerButtonAction(int buttonId);
    void setTitle(QString titleName);
private:
    int m_contentHeight;
    QStringList m_buttonsId;
    QVBoxLayout* pressPowerButtonActionLayout;
    DSeparatorHorizontal* m_pressPowerReactionLine;
    DButtonGrid* m_powerActionButtonGrid;

    DHeaderLine* m_pressPowerReActionLine;
    DBaseExpand* m_pressPowerReActionExpand;
};
#endif // PRESSPOWERBUTTONACTION

