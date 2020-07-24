#ifndef TOOLERRORTYPE
#define TOOLERRORTYPE

/**
 * @brief restore-tool工具返回的错误值
 */
enum ToolErrorType {
    NoError = 0,
    ToolError,
    GrubError,
    SpaceError
};

#endif