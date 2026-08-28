#include "Player.hpp"
#include "SignalInput.hpp"
#include "DesktopLyrics.hpp"
#include "LyricParser.hpp"
#include "InputController.hpp"

#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sys/ioctl.h> // 提供 ioctl() 和 TIOCGWINSZ
#include <unistd.h>    // 提供 STDOUT_FILENO
#include <stdio.h>     // 提供 perror()
                       //
int main(int argc, char* argv[]) {
    // 1. 初始化 Qt 应用
    QApplication app(argc, argv);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <MP3 file path>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // 从文件路径中提取纯文件名/歌名 (例如 /path/to/Song.mp3 -> Song)
    QString songTitle = QFileInfo(QString::fromStdString(filepath)).baseName();

    // 创建播放器对象
    Player player(filepath);

    if (!player.isValid()) {
        std::cerr << player.getError() << std::endl;
        return 1;
    }

    LyricsParser lyricParser;
    if (!lyricParser.load(filepath)) {
        std::cout << "Lyrics not found; showing the song title instead." << std::endl;
    }
    bool showLyrics = true; // 默认开启歌词显示
                            //
    player.printInfo();
    player.setLoop(true);

    // 2. 创建悬浮歌词窗口并初始化
    DesktopLyrics lyricsWidget;
    lyricsWidget.setText("🎵 " + songTitle);
    lyricsWidget.show();

    // 开始播放
    if (!player.play()) {
        std::cerr << "Playback failed to start" << std::endl;
        return 1;
    }

    TerminalRaw term;
    InputController inputController(player, lyricsWidget, term, songTitle);

    // 3. 定时器轮询
    QTimer pollTimer;
    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        // 1. 处理键盘输入，如果返回 false 说明用户按下 ESC 退出
        if (!inputController.handleInput()) {
            app.quit();
            return;
        }

        // 2. 刷新歌词逻辑（使用 inputController 状态）
        if (inputController.isLyricsVisible() && player.isPlaying() && !inputController.isPaused()) {
            float currentTime = player.getCurrentTime();
            if (lyricParser.hasLyrics()) {
                std::string currentLyric = lyricParser.getLyricAt(currentTime);
                if (currentLyric.empty()) {
                    lyricsWidget.setText("🎵 " + songTitle);
                } else {
                    lyricsWidget.setText(QString::fromStdString(currentLyric));
                }
            } else {
                lyricsWidget.setText("🎵 " + songTitle);
            }
        }

        // 3. 终端进度条渲染
        struct winsize w;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            perror("ioctl: failed to get terminal size");
        } else {
            int curSeconds = static_cast<int>(player.getCurrentTime());
            int durSeconds = static_cast<int>(player.getDuration());

            std::ostringstream timeText;
            timeText << std::setfill('0') << std::setw(2) << curSeconds / 60 << ":"
                     << std::setw(2) << curSeconds % 60 << "/"
                     << std::setw(2) << durSeconds / 60 << ":"
                     << std::setw(2) << durSeconds % 60;

            float ratio = (durSeconds > 0) ? static_cast<float>(curSeconds) / durSeconds : 0.0f;
            int barWidth = std::max(0, static_cast<int>(w.ws_col) - static_cast<int>(timeText.str().size()) - 1);
            int starCount = std::min(static_cast<int>(ratio * barWidth), barWidth);
            int slashCount = std::max(0, barWidth - starCount);

            std::cout << "\033[A\r"
                      << std::string(starCount, '*')
                      << std::string(slashCount, '-')
                      << " " << timeText.str()
                      << "\033[B" << std::flush;
        }
        // 4. 自然结束检测
        if (!player.isPlaying() && !inputController.isPaused()) {
            std::cout << "\nEnd of Playing." << std::endl;
            app.quit();
        }
    });

    pollTimer.start(30);
    return app.exec();
}
