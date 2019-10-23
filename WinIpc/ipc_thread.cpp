#include "ipc_thread.h"

namespace WinIpc {

  bool Thread::Start()
  {
    return Init(0, 1);
  }

  void Thread::OnError(DWORD error_code_win32, ULONG_PTR context, LPOVERLAPPED ol)
  {

  }

  void Thread::OnComplete(DWORD bytes_transfer, ULONG_PTR context, LPOVERLAPPED ol)
  {

  }

}