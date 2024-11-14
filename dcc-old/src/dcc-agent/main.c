// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int main(int argc, char *argv[])
{
    int (*main_func)(int, char **);
    char *error;
    bool isOldDcc = false;
    if (argc >= 1 && strcmp(argv[argc - 1], "n") == 0) {
        isOldDcc = false;
        argc = argc - 1;
    } else if (argc >= 1 && strcmp(argv[argc - 1], "o") == 0) {
        isOldDcc = true;
        argc = argc - 1;
    }

    // 加载共享库
    char *dccSoPath = isOldDcc ? OLD_DCC_SO_PATH : NEW_DCC_SO_PATH;
    char *dccPath = isOldDcc ? OLD_DCC_PATH : NEW_DCC_PATH;
    if (!dlopen(dccSoPath, RTLD_LAZY | RTLD_GLOBAL)) {
        fprintf(stderr, "%s\n", dlerror());
    }
    dlerror();
    void *handle = dlopen(dccPath, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    // 清除之前可能存在的错误
    dlerror();

    // 查找函数
    *(void **)(&main_func) = dlsym(handle, "main");

    // 检查错误
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        dlclose(handle);
        return 1;
    }

    // 使用函数
    int ret = (*main_func)(argc, argv);

    // 关闭共享库
    dlclose(handle);
    return ret;
}
