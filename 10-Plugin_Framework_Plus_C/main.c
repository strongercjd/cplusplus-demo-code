#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include "cjson/cJSON.h"

// 加载并执行库的函数
int load_and_run_library(const char *libname, const char *func_name)
{
    void *handle = NULL;
    int (*func_start)() = NULL;
    char *error = NULL;
    int result = 0;

    // 加载动态库
    handle = dlopen(libname, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle)
    {
        printf("Failed to load library %s: %s\n", libname, dlerror());
        return -1;
    }

    // 获取函数指针
    func_start = (int (*)())dlsym(handle, func_name);
    error = dlerror();
    if (error != NULL)
    {
        fprintf(stderr, "Failed to find function %s in %s: %s\n", func_name, libname, error);
        dlclose(handle);
        return -1;
    }

    // 调用函数
    if (func_start)
    {
        func_start();
        result = 0;
    }
    else
    {
        result = -1;
    }

    // 注意：这里不关闭 handle，因为库可能需要保持加载状态
    return result;
}

int main()
{
    printf("Hello World!\n");

    // 读取 JSON 配置文件
    FILE *file = fopen("libconfig.json", "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open libconfig.json\n");
        return 1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 读取文件内容
    char *json_string = (char *)malloc(file_size + 1);
    if (!json_string)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        return 1;
    }

    fread(json_string, 1, file_size, file);
    json_string[file_size] = '\0';
    fclose(file);

    // 解析 JSON
    cJSON *json = cJSON_Parse(json_string);
    if (!json)
    {
        fprintf(stderr, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        free(json_string);
        return 1;
    }

    // 遍历 JSON 对象中的每个模块
    cJSON *module = NULL;
    cJSON_ArrayForEach(module, json)
    {
        cJSON *libname_item = cJSON_GetObjectItem(module, "libname");
        cJSON *start_func_item = cJSON_GetObjectItem(module, "start_func");

        if (cJSON_IsString(libname_item) && cJSON_IsString(start_func_item))
        {
            char libpath[256];
            snprintf(libpath, sizeof(libpath), "./%s", libname_item->valuestring);
            
            printf("Loading module: %s\n", libname_item->valuestring);
            load_and_run_library(libpath, start_func_item->valuestring);
        }
        else
        {
            fprintf(stderr, "Invalid configuration for module\n");
        }
    }

    // 清理资源
    cJSON_Delete(json);
    free(json_string);

    while (1)
    {
        usleep(1000000);
    }

    return 0;
}