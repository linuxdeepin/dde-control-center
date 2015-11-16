#ifndef GENERATEDVIEW_H
#define GENERATEDVIEW_H

#include "abstractview.h"

class QLabel;
class QTimer;

class GeneratedView : public AbstractView
{
    Q_OBJECT
public:
    GeneratedView(const QString& token, QWidget* p=nullptr);

signals:
    void cancel();

private:
    QWidget* createMainWidget() Q_DECL_OVERRIDE;
    QLabel* m_token;
    QLabel* m_copyTip;
    QTimer* m_copyTipVisableTimer;
};

#endif // GENERATEDVIEW_H
