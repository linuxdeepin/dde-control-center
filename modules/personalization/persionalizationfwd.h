#ifndef PERSIONALIZATIONFWD
#define PERSIONALIZATIONFWD

#include <QMetaType>

typedef QList<QMap<QString, QString>> ImageInfoList;
typedef QMap<QString, QJsonObject> JosnMapObjs;

Q_DECLARE_METATYPE(ImageInfoList)
Q_DECLARE_METATYPE(JosnMapObjs)

#endif // PERSIONALIZATIONFWD

