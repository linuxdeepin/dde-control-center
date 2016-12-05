#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

enum UpdateType{
    CheckUpdate,
    StartDownload,
    StopDownload,
    RebootInstall
};

class DownloadProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressBar(QWidget* parent = 0);

    void setCurState(UpdateType type);

signals:
    void action(UpdateType type);

protected:
    void paintEvent(QPaintEvent*);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QString m_state;
    UpdateType m_type;
};

#endif // DOWNLOADPROGRESSBAR_H
