#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"

class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit PersonalizationModel(QObject *parent = 0);
    ~PersonalizationModel();
    inline ThemeModel*    getWindowModel()    const {return m_windowModel;}
    inline ThemeModel*    getIconModel()      const {return m_iconModel;}
    inline ThemeModel*    getMouseModel()     const {return m_mouseModel;}
    inline FontModel*     getStandFontModel() const {return m_standFontModel;}
    inline FontModel*     getMonoFontModel()  const {return m_monoFontModel;}
    inline FontSizeModel* getFontSizeModel()  const {return m_fontSizeModel;}

private:
    ThemeModel    *m_windowModel;
    ThemeModel    *m_iconModel;
    ThemeModel    *m_mouseModel;
    FontModel     *m_standFontModel;
    FontModel     *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;

};

#endif // PERSONALIZATIONMODEL_H
