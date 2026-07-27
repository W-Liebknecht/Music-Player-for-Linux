#ifndef TERMINAL_RAW_HPP
#define TERMINAL_RAW_HPP

#include <string>
#include <termios.h>

class TerminalRaw {
public:
    TerminalRaw();                         // 进入 raw 模式，保存原始终端属性
    ~TerminalRaw();                        // 恢复原始终端属性

    // 禁止拷贝，保证 RAII 安全
    TerminalRaw(const TerminalRaw&) = delete;
    TerminalRaw& operator=(const TerminalRaw&) = delete;

    // 非阻塞读取一个字符，timeout_ms 为等待时间（毫秒），无输入返回 0
    char getch(int timeout_ms = 100);

    // 显示 prompt，然后阻塞读取一行（支持退格），回车结束
    std::string read_line(const std::string& prompt = "");

private:
    struct termios orig_;
    bool saved_ = false;
};

#endif
