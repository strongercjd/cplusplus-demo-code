#include <stdio.h>
#include <string.h>
#include <dlfcn.h>  // 添加动态加载库的头文件

int main()
{
    printf("Hello World!\n");
    printf("Please enter a command (enter 'q' and press Enter to execute the add function, enter 'exit' to quit the program):\n");

    // Buffer to store input
    char input[100];

    void *handle = NULL;        // 动态库句柄
    int (*add_func)(int, int) = NULL;  // 函数指针
    int library_loaded = 0;     // 标记库是否已加载

    // Infinite loop to monitor input
    while (1) {
        printf("> ");  // Input prompt

        if (fgets(input, sizeof(input), stdin) == NULL) { // Read user input
            printf("Input error!\n"); // Handle input errors
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "q") == 0) {
            // 如果库还没加载，先加载动态库
            if (!library_loaded) {
                printf("Loading dynamic library...\n");

                // 打开动态库（根据你的库名修改）
                handle = dlopen("./libmath.so", RTLD_LAZY);
                if (!handle) {
                    fprintf(stderr, "Failed to load library: %s\n", dlerror());
                    continue;
                }

                // 获取add函数地址
                add_func = dlsym(handle, "add");
                char *error = dlerror();
                if (error != NULL) {
                    fprintf(stderr, "Failed to find add function: %s\n", error);
                    dlclose(handle);
                    continue;
                }

                library_loaded = 1;
                printf("Dynamic library loaded successfully.\n");
            }

            printf("start add(5,5)...\n");
            int result = add_func(5, 5);  // 通过函数指针调用
            printf("end add(5,5)=%d...\n", result);
        }

        else if (strcmp(input, "exit") == 0) { // Exit command
            printf("Exiting program...\n");

            // 关闭动态库
            if (library_loaded) {
                dlclose(handle);
                printf("Dynamic library unloaded.\n");
            }

            break;
        }

        else if (strlen(input) > 0) {    // Prompt for unknown input
            printf("Unknown command: %s\n", input);
            printf("Tip: Enter 'q' to execute the add function, enter 'exit' to quit the program\n");
        }
    }

    return 0;
}