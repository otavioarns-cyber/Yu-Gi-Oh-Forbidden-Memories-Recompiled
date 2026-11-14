/* Own WinAPI automation driver: touches only the child created by this process. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
static DWORD owned;static int windows_seen,close_sent;
static BOOL CALLBACK visit(HWND hwnd,LPARAM ignored){
 DWORD pid=0;GetWindowThreadProcessId(hwnd,&pid);if(pid!=owned||!IsWindowVisible(hwnd))return TRUE;
 wchar_t title[512];GetWindowTextW(hwnd,title,512);RECT box;GetWindowRect(hwnd,&box);
 printf("OWNED_WINDOW pid=%lu title=%ls bounds=%ld,%ld,%ld,%ld\n",pid,title,box.left,box.top,box.right,box.bottom);fflush(stdout);windows_seen++;
 if(!close_sent){close_sent=1;PostMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);PostMessageW(hwnd,WM_KEYUP,VK_ESCAPE,0);PostMessageW(hwnd,WM_CLOSE,0,0);puts("SIMULATED_INPUT escape-and-close to child-owned window only");}
 return TRUE;
}
int main(int argc,char**argv){
 if(argc!=2)return 90;wchar_t file[4096],cmd[8192];MultiByteToWideChar(CP_UTF8,0,argv[1],-1,file,4096);swprintf(cmd,8192,L"\"%ls\"",file);
 STARTUPINFOW s={0};s.cb=sizeof(s);PROCESS_INFORMATION p={0};
 if(!CreateProcessW(file,cmd,0,0,FALSE,0,0,0,&s,&p)){printf("CREATE_FAILED error=%lu\n",GetLastError());return 91;}
 owned=p.dwProcessId;DWORD elapsed=0;
 while(WaitForSingleObject(p.hProcess,100)==WAIT_TIMEOUT&&elapsed<10000){elapsed+=100;EnumWindows(visit,0);}
 DWORD code=0;GetExitCodeProcess(p.hProcess,&code);int timed=code==STILL_ACTIVE;
 if(timed){TerminateProcess(p.hProcess,92);WaitForSingleObject(p.hProcess,1000);GetExitCodeProcess(p.hProcess,&code);}
 printf("CHILD_RESULT pid=%lu exit=%lu exit_hex=%08lx windows=%d input=%d timeout=%d\n",owned,code,code,windows_seen,close_sent,timed);
 CloseHandle(p.hThread);CloseHandle(p.hProcess);return timed?92:code?1:0;
}
