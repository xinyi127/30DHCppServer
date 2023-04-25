#include "util.h"
#include <stdio.h> // perror 函数
#include <stdlib.h> // exit 函数和 宏 EXIT_FAILURE

void errif(bool condition, const char *errmsg){
    if(condition){
        // void  preeor(const char*) 把一个描述性错误消息输出到标准错误 stderr。
        perror(errmsg);
        // void exit(int) 使程序在 main 以外的函数中终止。实参是程序返回到计算机操作系统的退出代码。
        // 参数为 0 或 EXIT_SUCCESS 表示程序正常终止，否则表示因异常程序终止。EXIT_FAILURE 是 <stdilb.h> 中定义的宏，值为 1。表示程序不成功执行。
        exit(EXIT_FAILURE);
    }
}
