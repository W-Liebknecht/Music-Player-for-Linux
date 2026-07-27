#include "SignalInput.hpp"
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <stdexcept>

TerminalRaw::TerminalRaw() {
    if (tcgetattr(STDIN_FILENO, &orig_) < 0)
        throw std::runtime_error("tcgetattr failed");

    struct termios raw = orig_;
    raw.c_lflag &= ~(ICANON | ECHO);   // 关闭规范模式和回显
    raw.c_cc[VMIN] = 1;                // 阻塞等待至少1个字符
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0)
        throw std::runtime_error("tcsetattr raw mode failed");

    saved_ = true;
}

TerminalRaw::~TerminalRaw() {
    if (saved_) {
        tcsetattr(STDIN_FILENO, TCSADRAIN, &orig_);
    }
}

char TerminalRaw::getch(int timeout_ms) {
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    if (poll(&pfd, 1, timeout_ms) <= 0)
        return 0;

    char c = 0;
    if (read(STDIN_FILENO, &c, 1) <= 0)
        return 0;
    return c;
}

std::string TerminalRaw::read_line(const std::string& prompt) {
    if (!prompt.empty())
        std::cout << prompt << std::flush;

    std::string line;
    while (true) {
        char c = 0;
        if (read(STDIN_FILENO, &c, 1) <= 0)
            continue;

        if (c == 3)
            throw std::runtime_error("Ctrl-C pressed");

        if (c == '\n' || c == '\r') {
            std::cout << "\r\n" << std::flush;
            break;
        }
        else if (c == 127 || c == '\b') {   // 退格
            if (!line.empty()) {
                line.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        }
        else if (c >= 32 && c < 127) {      // 可打印字符
            line += c;
            std::cout << c << std::flush;   // ★ 关键：立刻刷新
        }
    }
    return line;
}
