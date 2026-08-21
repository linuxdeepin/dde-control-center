//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHARAMANGERMODEL_H
#define CHARAMANGERMODEL_H
#include <QObject>

#define FACE_CHARA 4
#define IRIS_CHARA 64

class CharaMangerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool faceDriverVaild READ faceDriverVaild WRITE setFaceDriverVaild NOTIFY vaildFaceDriverChanged)
    Q_PROPERTY(bool fingerDriverVaild READ fingerVaild WRITE setFingerVaild NOTIFY vaildFingerChanged)
    Q_PROPERTY(bool irisDriverVaild READ irisDriverVaild WRITE setIrisDriverVaild NOTIFY vaildIrisDriverChanged)

    Q_PROPERTY(QStringList facesList READ facesList WRITE setFacesList NOTIFY facesListChanged)
    Q_PROPERTY(bool charaVaild READ charaVaild WRITE setCharaVaild NOTIFY charaVaildChanged)

    Q_PROPERTY(QStringList thumbsList READ thumbsList WRITE setThumbsList NOTIFY thumbsListChanged)
    Q_PROPERTY(QStringList irisList READ irisList WRITE setIrisList NOTIFY irisListChanged)

public:
    /**
     * @brief The EnrollStatusType enum 人脸录入状态
     */
    enum EnrollFaceStatusType {
        STATUS_SUCCESS = 0,   // 成功
        STATUS_NOT_REAL_HUMAN,  // 非人类
        STATUS_FACE_NOT_CENTER,  // 非中心
        STATUS_FACE_TOO_BIG,  // 脸太大
        STATUS_FACE_TOO_SMALL,
        STATUS_NO_FACE,
        STATUS_FACE_TOO_MANY,  // 多人
        STATUS_FACE_NOT_CLEARITY,  // 不清晰
        STATUS_FACE_BRIGHTNESS,  // 亮度
        STATUS_FACE_COVERD,  // 遮挡
        STATUS_CANCELED, // 取消
        STATUS_OVERTIME, // 超时
        STATUS_COLLAPSE // 崩溃
    };

    /**
     * @brief The EnrollIrisStatusType enum
     */
    enum EnrollIrisStatusType {
        STATUS_IRIS_SUCCESS = 0,   // 成功
        STATUS_IRIS_TOO_BIG,  // 太大
        STATUS_IRIS_TOO_SMALL,
        STATUS_IRIS_NO_FACE,
        STATUS_IRIS_NOT_CLEARITY,  // 不清晰
        STATUS_IRIS_BRIGHTNESS,  // 亮度
        STATUS_IRIS_EYES_CLOSE,  // 闭目
        STATUS_IRIS_CANCELED, // 取消
        STATUS_IRIS_Error, // 崩溃
        STATUS_IRIS_OVERTIME // 超时
    };

    /**
     * @brief The AddInfoState enum 录入的四种状态
     */
    enum AddInfoState {
        StartState,
        Success,
        Fail,
        Processing,
    };

    Q_ENUM(AddInfoState)

    /**
     * @brief The EnrollResult enum 指纹
     */
    enum EnrollResult {
        Enroll_AuthFailed,
        Enroll_ClaimFailed,
        Enroll_Failed,
        Enroll_AuthSuccess,
        Count
    };

    Q_ENUM(EnrollResult)

    enum EnrollStatusType {
        ET_Completed = 0,
        ET_Failed,
        ET_StagePass,
        ET_Retry,
        ET_Disconnect
    };

    enum EnrollFailedCode {
        FC_UnkownError = 1,
        FC_RepeatTemplet,
        FC_EnrollBroken,
        FC_DataFull
    };

    enum EnrollRetryCode {
        RC_TouchTooShort = 1,
        RC_ErrorFigure,
        RC_RepeatTouchData,
        RC_RepeatFingerData,
        RC_SwipeTooShort,
        RC_FingerNotCenter,
        RC_RemoveAndRetry,
        RC_CannotRecognize
    };

    enum EnrollType {
        Type_Face = 0,
        Type_Finger,
        Type_Iris
    };

    Q_ENUM(EnrollType)

public:
    explicit CharaMangerModel(QObject *parent = nullptr);

    inline int faceCharaType() const { return  FACE_CHARA; }
    inline int irisCharaType() const { return  IRIS_CHARA; }

    inline bool faceDriverVaild() const {  return m_isFaceDriverVaild; }
    void setFaceDriverVaild(bool isVaild);

    inline QString faceDriverName() const { return  m_faceDriverName; }
    void setFaceDriverName(const QString &driverName);

    inline QStringList facesList() const { return  m_facesList; }
    void setFacesList(const QStringList &faces);

    inline bool irisDriverVaild() const {  return m_isIrisDriverVaild; }
    void setIrisDriverVaild(bool isVaild);

    inline QString irisDriverName() const { return  m_irisDriverName; }
    void setIrisDriverName(const QString &driverName);

    inline QStringList irisList() const { return  m_irisList; }
    void setIrisList(const QStringList &iris);

    /**
     * @brief onEnrollStatusChanged 录入状态信号，在录入过程中通过此信号提示当前录入状态
     * @param code
     * @param msg
     */
    void onEnrollStatusChanged(int code, const QString& msg);
    void onEnrollIrisStatusChanged(int code, const QString& msg);

    /**
     * @brief onRefreshEnrollDate  用于刷新用户已录入的数据（ eg： 重命名失败后）
     * @param charaType 对应类型
     */
    void onRefreshEnrollDate(const int &charaType);

    void setInputFaceFD(const int &fd);
    void setInputIrisFD(CharaMangerModel::AddInfoState state);

    // Finger
    void initFingerModel();
    inline QList<QString> getPredefineThumbsName() const { return m_predefineThumbsNames; }

    inline bool fingerVaild() const {  return m_isFingerVaild; }
    void setFingerVaild(bool isVaild);

    inline QString userName() const { return m_userName; }
    void setUserName(const QString &name);

    inline QStringList thumbsList() const { return  m_thumbsList; }
    void setThumbsList(const QStringList &thumbs);

    void onFingerEnrollStatusChanged(int code, const QString& msg);
    void onTouch(const QString &id, bool pressed);

    void resetProgress() { m_progress = 0; }

    void refreshEnrollResult(EnrollResult enrollRes);

    bool charaVaild() const;
    void setCharaVaild(bool newCharaVaild = true);

Q_SIGNALS:
    void vaildFaceDriverChanged(const bool isVaild);
    void vaildIrisDriverChanged(const bool isVaild);

    void facesListChanged(const QStringList &faces);
    void irisListChanged(const QStringList &iris);

    /**
     * @brief enrollInfoState 注册录入状态  用于区分页面显示状态  注：仅处理成功录入失败状态
     * @param state 录入的状态
     * @param tips 对应的提示语
     */
    void enrollInfoState(AddInfoState state, const QString &tips);
    void enrollStatusTips(QString title);

    /**
     * @brief enrollIrisInfoState 注册虹膜录入状态
     * @param state 录入状态
     * @param tips 提示信息
     */
    void enrollIrisInfoState(AddInfoState state, const QString &tips);
    void enrollIrisStatusTips(QString title);

    /**
     * @brief tryStartInputFace tryStartInputIris  获取人脸虹膜文件标识符
     * @param fd
     */
    void tryStartInputFace(const int &fd);
    void tryStartInputIris(CharaMangerModel::AddInfoState state);

    // FInger
    void vaildFingerChanged(const bool isVaild);
    void thumbsListChanged(const QStringList &thumbs);

    void enrollFailed(QString title, QString msg);
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollRetry(QString title, QString msg);
    void enrollDisconnected();
    void enrollResult(EnrollResult enrollRes);

    void lockedChanged(bool locked);

    //charaVaild
    void charaVaildChanged(const bool isVaild);

private:
    void checkCharaVaild() {
        if (m_isIrisDriverVaild || m_isFaceDriverVaild || m_isFingerVaild)
            setCharaVaild();
        else
            setCharaVaild(false);
    }

    // 人脸
    QString m_faceDriverName;
    bool m_isFaceDriverVaild;
    QStringList m_facesList;

    // 虹膜
    QString m_irisDriverName;
    bool m_isIrisDriverVaild;
    QStringList m_irisList;

    // 指纹
    QString m_userName;
    bool m_isFingerVaild{false};
    int m_progress;
    QStringList m_thumbsList;
    QList<QString> m_predefineThumbsNames;
    bool m_charaVaild;
};

#endif // CHARAMANGERMODEL_H
