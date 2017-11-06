#ifndef LOADINGNEXTPAGEWIDGET_H
#define LOADINGNEXTPAGEWIDGET_H

#include "nextpagewidget.h"

#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class LoadingNextPageWidget : public NextPageWidget
{
    Q_OBJECT

public:
    explicit LoadingNextPageWidget(QFrame *parent = nullptr);

public slots:
    void setLoading(const bool loading);

private:
    DPictureSequenceView *m_loadingIndicator;
};

}

}

#endif // LOADINGNEXTPAGEWIDGET_H
