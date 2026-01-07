#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

extern int data_sender_start();
extern int data_processor_start();

int main()
{
    void *handle = NULL;
    int (*func_start)() = NULL; // 函数指针
    printf("Hello World!\n");
    handle = dlopen("./libdata_processor.so", RTLD_LAZY);
    if (!handle)
    {
        printf("Failed to load data_processor library: %s\n", dlerror());
    }
    func_start = (int (*)())dlsym(handle, "data_processor_start");
    char *error = dlerror();
    if (error != NULL)
    {
        fprintf(stderr, "data_processor Failed to find add function: %s\n", error);
        dlclose(handle);
    }
    else
    {
        func_start();
    }

    handle = dlopen("./libdata_sender.so", RTLD_LAZY);
    if (!handle)
    {
        printf("Failed to load data_sender library: %s\n", dlerror());
    }
    func_start = (int (*)())dlsym(handle, "data_sender_start");
    error = dlerror();
    if (error != NULL)
    {
        fprintf(stderr, "data_sender Failed to find add function: %s\n", error);
        dlclose(handle);
    }
    else
    {
        func_start();
    }

    while (1)
    {
        usleep(1000000);
    }

    return 0;
}
