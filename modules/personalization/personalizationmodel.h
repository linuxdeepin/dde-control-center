#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>

namespace dcc
{
namespace personalization
{
class ThemeModel;
class FontModel;
class FontSizeModel;
class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit PersonalizationModel(QObject *parent = 0);
    ~PersonalizationModel();
    inline ThemeModel    *getWindowModel()    const {return m_windowModel;}
    inline ThemeModel    *getIconModel()      const {return m_iconModel;}
    inline ThemeModel    *getMouseModel()     const {return m_mouseModel;}
    inline FontModel     *getStandFontModel() const {return m_standFontModel;}
    inline FontModel     *getMonoFontModel()  const {return m_monoFontModel;}
    inline FontSizeModel *getFontSizeModel()  const {return m_fontSizeModel;}
    void setIs3DWm(const bool is3d);
    bool is3DWm() const;

signals:
    void wmChanged(const bool is3d);

private:
    ThemeModel    *m_windowModel;
    ThemeModel    *m_iconModel;
    ThemeModel    *m_mouseModel;
    FontModel     *m_standFontModel;
    FontModel     *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;
    bool m_is3DWm;
};
}
}
#endif // PERSONALIZATIONMODEL_H
