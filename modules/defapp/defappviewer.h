#ifndef DEFAPPVIEWER_H
#define DEFAPPVIEWER_H

#include "contentwidget.h"
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QFileInfo>

namespace dcc
{
namespace widgets {
class SwitchWidget;
class SettingsGroup;
class TranslucentFrame;
}
namespace defapp
{
class DefCategoryWidget;
class DefAppModel;
class DefAppViewer : public ContentWidget
{
    Q_OBJECT

public:
    explicit DefAppViewer(QWidget *parent = 0);
    QWidget *defappDetail();
    void setModel(DefAppModel * const model);

signals:
    void requestSetDefaultApp(const QString &category, const QJsonObject &item);
    void requestDelUserApp(const QString &name, const QJsonObject &item);
    void autoOpenChanged(const bool state);
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

private:

    DefCategoryWidget *m_modBrowser = nullptr;
    DefCategoryWidget *m_modMail = nullptr;
    DefCategoryWidget *m_modText = nullptr;
    DefCategoryWidget *m_modMusic = nullptr;
    DefCategoryWidget *m_modVideo = nullptr;
    DefCategoryWidget *m_modPicture = nullptr;
    DefCategoryWidget *m_modTerminal = nullptr;
};
}
}

#endif // DEFAPPVIEWER_H
