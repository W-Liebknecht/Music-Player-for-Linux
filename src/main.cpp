#include "Player.hpp"
#include "SignalInput.hpp"
#include "DesktopLyrics.hpp"
#include "LyricParser.hpp"

#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <iostream>
#include <string>

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
        std::cout << "未找到外挂或内嵌歌词，将默认显示歌曲名。" << std::endl;
    }

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
    bool paused = false;

    std::cout << "Now playing... Space to pause/resume, ':' command, ESC to quit." << std::endl;

    // 3. 定时器轮询
    QTimer pollTimer;
    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        // --- Part A: 处理终端按键输入 ---
        char ch = term.getch(10);

        if (ch != 0) {
            if (ch == 27) {                   // ESC
                std::cout << "\r⏹ The user presses ESC to stop playing.\033[K\n" << std::flush;
                player.stop();
                app.quit();
                return;
            }
            else if (ch == ' ') {             // 空格切换暂停
                if (!paused) {
                    player.pause();         
                    std::cout << "\r⏸ Paused\033[K" << std::flush;
                    lyricsWidget.setText("⏸ " + songTitle);
                } else {
                    player.play();          
                    std::cout << "\r▶ Continue Playing\033[K" << std::flush;
                }
                paused = !paused;
            }
            else if (ch == 'h') {
                if (!paused) { player.pause(); }
                float currentTime = player.getCurrentTime();
                if (currentTime >= 5.f) { currentTime -= 5.f; } else { currentTime = 0.f; };
                player.setCurrentTime(currentTime);
                if (!paused) { player.play(); }
                std::cout << "\rRewind 5 seconds\033[K" << std::flush;
            }
            else if (ch == 'l') {
                if (!paused) { player.pause(); }
                float currentTime = player.getCurrentTime();
                currentTime += 5.f;
                player.setCurrentTime(currentTime);
                if (!paused) { player.play(); }
                std::cout << "\rForward 5 seconds\033[K" << std::flush;
            }
            else if (ch == 'k') {
                if (!paused) { player.pause(); }
                float currentTime = player.getCurrentTime();
                if (currentTime >= 30.f) { currentTime -= 30.f; } else { currentTime = 0.f; };
                player.setCurrentTime(currentTime);
                if (!paused) { player.play(); }
                std::cout << "\rRewind 30 seconds\033[K" << std::flush;
            }
            else if (ch == 'j') {
                if (!paused) { player.pause(); }
                float currentTime = player.getCurrentTime();
                currentTime += 30.f;
                player.setCurrentTime(currentTime);
                if (!paused) { player.play(); }
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

        // --- Part B: 实时刷新桌面歌词（必须放在定时器回调内！）---
        if (player.isPlaying() && !paused) {
            float currentTime = player.getCurrentTime(); // 实时获取播放时间

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

        // --- Part C: 检测播放是否自然结束 ---
        if (!player.isPlaying() && !paused) {
            std::cout << "\nEnd of Playing." << std::endl;
            app.quit();
        }
    });

    pollTimer.start(30);

    return app.exec();
}
