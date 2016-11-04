#ifndef OPENWITHOTHERDIALOG_H
#define OPENWITHOTHERDIALOG_H

#include <ddialog.h>
#include <QListWidget>
#include <QFileInfoList>
#include <QDir>



#include <QDebug>


DWIDGET_USE_NAMESPACE

class OpenWithOtherDialog : public DDialog
{
    Q_OBJECT
public:
    explicit OpenWithOtherDialog(QWidget *parent = 0);
    ~OpenWithOtherDialog();
    inline QString getDesktopFile(){return desktopFile;}

    void initUI();
    void initConnect();

    QListWidget *createOpenWithListWidget();


public slots:
    void handleButtonClicked(int index, QString text);

private:
    QListWidget* m_appListWidget = NULL;
    QButtonGroup* m_OpenWithButtonGroup = NULL;
    QString desktopFile;


private:
    QFileInfoList GetFileList(const QString &path);
    QString getQssFromFile(QString filename);

};

#endif // OPENWITHOTHERDIALOG_H
