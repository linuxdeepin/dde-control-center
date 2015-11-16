#ifndef ERRORVIEW_H
#define ERRORVIEW_H

#include "abstractview.h"

class QLabel;

class ErrorView : public AbstractView
{
    Q_OBJECT
public:
    ErrorView(QWidget*p=nullptr);
    ErrorView* setText(const QString& text);

private slots:
    // void onCancelButtonClicked();
    // void onRetryButtonClicked();

private:
    QLabel* m_text;

    QWidget* createMainWidget() Q_DECL_OVERRIDE;
};

#endif // ERRORVIEW_H
