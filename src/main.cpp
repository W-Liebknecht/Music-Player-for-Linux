#include "Player.hpp"
#include "SignalInput.hpp"   // 替换原来的 SignalInput.hpp
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <MP3 file path>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // 创建播放器对象
    Player player(filepath);

    // 检查播放器是否有效
    if (!player.isValid()) {
        std::cerr << player.getError() << std::endl;
        return 1;
    }

    // 显示音乐信息
    player.printInfo();

    // 开始播放
    if (!player.play()) {
        std::cerr << "Playback failed to start" << std::endl;
        return 1;
    }

    // 进入 raw 模式（自动在退出时恢复）
    TerminalRaw term;
    bool paused = false;   // 记录暂停状态（如果 Player 无此状态查询）

    std::cout << "Now playing... Space to pause/resume, ':' command, ESC to quit." << std::endl;
    // 主循环：等待播放结束或用户中断
    while (true) {
        char ch = term.getch(50);

        if (ch != 0) {
            if (ch == 27) {                 // ESC
                std::cout << "\r⏹ The user presses ESC to stop playing.\033[K" << std::flush;
                player.stop();
                break;
            }
            else if (ch == ' ') {           // ★ 空格：切换暂停/继续
                if (!paused) {
                    player.pause();         // 假设 Player 有 pause()
                    std::cout << "\r⏸ Paused\033[K" << std::flush;
                } else {
                    player.play();          // 恢复播放
                    std::cout << "\r▶ Continue Playing\033[K" << std::flush;
                }
                paused = !paused;
            }
            else if (ch == 'h') {
                if (!paused) {player.pause();}
                float currentTime = player.getCurrentTime();
                if (currentTime >= 5.f) {currentTime -= 5.f;} else {currentTime = 0.f;};
                player.setCurrentTime(currentTime);
                if (!paused) {player.play();}
                std::cout << "\rRewind 5 seconds\033[K" << std::flush;
            }
            else if (ch == 'l') {
                if (!paused) {player.pause();}
                float currentTime = player.getCurrentTime();
                currentTime += 5.f;
                player.setCurrentTime(currentTime);
                if (!paused) {player.play();}
                std::cout << "\rForward 5 seconds\033[K" << std::flush;
            }
            else if (ch == 'k') {
                if (!paused) {player.pause();}
                float currentTime = player.getCurrentTime();
                if (currentTime >= 30.f) {currentTime -= 30.f;} else {currentTime = 0.f;};
                player.setCurrentTime(currentTime);
                if (!paused) {player.play();}
                std::cout << "\rRewind 30 seconds\033[K" << std::flush;
            }
            else if (ch == 'j') {
                if (!paused) {player.pause();}
                float currentTime = player.getCurrentTime();
                currentTime += 30.f;
                player.setCurrentTime(currentTime);
                if (!paused) {player.play();}
                std::cout << "\rForward 30 seconds\033[K" << std::flush;
            }
            else if (ch == ':') {
                std::string cmd = term.read_line("\r:\033[K");
                std::cout << "\033[F" << ":" << cmd << "\033[K" << std::flush;
            }
            else {
                std::cout << "\rKey capture: " << ch
                          << " (ASCII: " << static_cast<int>(ch) << ")" << std::flush;
            }
        }

        // 检测播放是否自然结束（如果 Player 有状态查询）
        if (!player.isPlaying() && !paused) {
            break;   // 正常播放完毕，退出循环
        }
    }
    std::cout << "\n播放结束。" << std::endl;
    return 0;
}
