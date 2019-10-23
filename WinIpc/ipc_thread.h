#pragma once

#include "../HelperForWin32/Helper/iocp_server.h"

namespace WinIpc {

  class Thread
    :public helper_lib::IocpServer
  {
  public:
    bool Start();


  protected:
    virtual void OnError(DWORD error_code_win32, ULONG_PTR context, LPOVERLAPPED ol) override;

    virtual void OnComplete(DWORD bytes_transfer, ULONG_PTR context, LPOVERLAPPED ol) override;

  private:
    bool is_started_{ false };
  };
}


