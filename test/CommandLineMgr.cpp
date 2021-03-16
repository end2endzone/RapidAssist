/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "CommandLineMgr.h"

#include "rapidassist/cli.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing_utf8.h"
#include "rapidassist/timing.h"

#ifdef _WIN32
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#include <signal.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <cstring>
#endif

namespace ra { namespace test
{
  static const std::string separator = ra::filesystem::GetPathSeparatorStr();

  std::string GetFunctionNameWithoutNamespace(const char * full_name) {
    ra::strings::StringVector values;
    ra::strings::Split(values, full_name, ":");
    if (values.empty())
      return "";
    const std::string & last = values[values.size()-1];
    return last;
  }

  //DisableConsoleInterruptSignals()
#ifdef _WIN32
  BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    default:
      return FALSE; //Do not exit program
    }
    return TRUE;
  }
  void DisableConsoleInterruptSignals() {
    signal(SIGINT, SIG_IGN);    //Disable CTRL+C
    signal(SIGBREAK, SIG_IGN);  //Disable CTRL+BREAK
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  }
#elif defined(__linux__) || defined(__APPLE__)
  void DisableConsoleInterruptSignals() {
    //Ignore all signals except KILL and STOP
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    for(int i = 1 ; i < 65 ; i++) {
      // 9 and 19 cannot be caught or ignored
      // 32 and 33 do not exist
      if((i != SIGKILL) && (i != SIGSTOP) && (i != 32) && (i != 33)) {
        sigaction(i, &act, NULL);
      }
    }
  }
#endif

#ifdef _WIN32
  #define IDT_TIMER1 24
  int timer_counter = 0;

  LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg)   {
    case WM_CREATE:
      timer_counter = 0;
      SetTimer(hWnd,              // handle to main window 
          IDT_TIMER1,             // timer identifier 
          500,                    // 0.5-second interval 
          (TIMERPROC) NULL);      // no timer callback 
      break;
    case WM_DESTROY:
      KillTimer(hWnd, IDT_TIMER1);
      PostQuitMessage(0);
      return 0;
    case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER1:
        if (timer_counter%2 == 0)
        {
          printf("Counting: %d\n", timer_counter);
          fflush(NULL);
        }
        timer_counter++;
        return 0;
      }
      break;
    default:
      return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
    return 0;
  };

  void WaitForTerminateSignal() {
    DisableConsoleInterruptSignals();

    WNDCLASSEXA window_class = {0};
    window_class.cbSize          = sizeof(window_class);
    window_class.style           = CS_DBLCLKS;
    window_class.lpfnWndProc     = WndProc;
    window_class.hInstance       = GetModuleHandle(NULL);
    window_class.lpszClassName   = "WindowClass";

    printf("Waiting for TERM signal...\n");
    fflush(NULL);

    if(RegisterClassExA(&window_class))
    {
      HWND WindowHandle = CreateWindowExA(0, "WindowClass", "Window Title", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, GetModuleHandle(NULL), NULL);
      if(WindowHandle)
      {
        MSG msg = {NULL};
        while(GetMessageA(&msg, NULL, 0, 0))
        {
          TranslateMessage(&msg);
          DispatchMessageA(&msg);
        }
      }
    }

    printf("Leaving...\n");
    fflush(NULL);
  }
#elif defined(__linux__) || defined(__APPLE__)
  volatile bool is_interrupted;

  void term_handler(int signum) {
    printf("Caught SIGTERM!\n");
    fflush(NULL);
    is_interrupted = true;
  }

  void WaitForTerminateSignal() {
    is_interrupted = false;
  
    DisableConsoleInterruptSignals();

    //Register our custom TERM signal handler
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = term_handler;
    sigaction(SIGTERM, &act, NULL);  
    
    printf("Waiting for TERM signal...\n");
    fflush(NULL);
    int counter = 0;
    while(!is_interrupted) {
      printf("Counting: %d\n", counter);
      fflush(NULL);
      counter++;

      //Sleep 1 second, in small increments
      static const unsigned int MILLI_TO_MICRO = 1000;
      for(int j=0; j<10 && !is_interrupted; j++) {
        usleep(100 * MILLI_TO_MICRO);
      }
    }
    printf("Leaving...\n");
    fflush(NULL);
  }
#endif

  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessPathUtf8() {
    std::string current_process_path_utf8 = ra::process::GetCurrentProcessPathUtf8();
    printf("%s\n", current_process_path_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessDirUtf8() {
    std::string current_process_dir_utf8 = ra::process::GetCurrentProcessDirUtf8();
    printf("%s\n", current_process_dir_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentDirectoryUtf8() {
    std::string current_dir_utf8 = ra::filesystem::GetCurrentDirectoryUtf8();
    printf("%s\n", current_dir_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessPath() {
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    printf("%s\n", current_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessDir() {
    std::string current_process_dir = ra::process::GetCurrentProcessDir();
    printf("%s\n", current_process_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentDirectory() {
    std::string current_dir = ra::filesystem::GetCurrentDirectory();
    printf("%s\n", current_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessPathUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_path_utf8 = ra::process::GetCurrentProcessPathUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_process_path_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessDirUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_dir_utf8 = ra::process::GetCurrentProcessDirUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_process_dir_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentDirectoryUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_dir_utf8 = ra::filesystem::GetCurrentDirectoryUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_dir_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessPath() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_path_ = ra::process::GetCurrentProcessPath();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_process_path_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessDir() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_dir_ = ra::process::GetCurrentProcessDir();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_process_dir_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentDirectory() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_dir_ = ra::filesystem::GetCurrentDirectory();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_dir_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  void WaitForKillSignal() {
    DisableConsoleInterruptSignals();
  
    printf("Waiting for KILL signal...\n");
    fflush(NULL);
    int loop_counter = 0;
    while(true) {
      printf("Counting: %d\n", loop_counter);
      fflush(NULL);
      loop_counter++;

      //Sleep 1 second
      ra::timing::Millisleep(1000);
    }
    printf("Leaving...\n");
    fflush(NULL);
  }
  //--------------------------------------------------------------------------------------------------
  void SleepTime(int sleep_time_ms)
  {
    printf("Sleeping for %d milliseconds...\n", sleep_time_ms);
    fflush(NULL);
    ra::timing::Millisleep(sleep_time_ms);
    printf("Done sleeping!\n");
    fflush(NULL);
  }
  //--------------------------------------------------------------------------------------------------
  int ExitCode(int exit_code)
  {
    //Sleep to simulate process execution
    SleepTime(5000);
    printf("Exiting with code %d...\n", exit_code);
    fflush(NULL);
    return exit_code;
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace ra
