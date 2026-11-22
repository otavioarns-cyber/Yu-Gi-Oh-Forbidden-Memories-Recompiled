#include <windows.h>
#include <stdio.h>
int main(void){printf("WINDOWS_CONTROL pid=%lu pointer_bytes=%u\n",GetCurrentProcessId(),(unsigned)sizeof(void*));return 0;}
