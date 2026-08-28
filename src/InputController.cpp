#include "InputController.hpp"
#include <iostream>
#include <cmath>

InputController::InputController(Player& player, DesktopLyrics& lyricsWidget, TerminalRaw& term, const QString& songTitle)
    : m_player(player), m_lyricsWidget(lyricsWidget), m_term(term), m_songTitle(songTitle) {}

bool InputController::handleInput() {
    char ch = m_term.getch(10);
    if (ch == 0) {
        return true; // 没有输入，继续循环
    }

    switch (ch) {
        case 27: // ESC 键退出
            std::cout << "\r⏹ Stopped (ESC pressed)\033[K\n" << std::flush;
            m_player.stop();
            return false;

        case ' ': // 空格键：暂停/播放
            m_paused = !m_paused;
            if (m_paused) {
                m_player.pause();
                std::cout << "\r⏸ Paused\033[K" << std::flush;
                m_lyricsWidget.setText("⏸ " + m_songTitle);
            } else {
                m_player.play();
                std::cout << "\r▶ Continue Playing\033[K" << std::flush;
            }
            break;

        case 'h': // 快退 5 秒
            seekRelative(-5.0f);
            std::cout << "\rRewind 5 seconds\033[K" << std::flush;
            break;

        case 'l': // 快进 5 秒
            seekRelative(5.0f);
            std::cout << "\rForward 5 seconds\033[K" << std::flush;
            break;

        case 'k': // 快退 30 秒
            seekRelative(-30.0f);
            std::cout << "\rRewind 30 seconds\033[K" << std::flush;
            break;

        case 'j': // 快进 30 秒
            seekRelative(30.0f);
            std::cout << "\rForward 30 seconds\033[K" << std::flush;
            break;

        case ':': // 命令模式
            handleCommandMode();
            break;

        default:
            std::cout << "\rKey capture: " << ch
                      << " (ASCII: " << static_cast<int>(ch) << ")" << std::flush;
            break;
    }

    return true;
}

void InputController::seekRelative(float seconds) {
    if (!m_paused) { m_player.pause(); }

    float targetTime = m_player.getCurrentTime() + seconds;
    if (targetTime < 0.0f) {
        targetTime = 0.0f;
    } else if (targetTime > m_player.getDuration()) {
        targetTime = m_player.getDuration();
    }

    m_player.setCurrentTime(targetTime);
    if (!m_paused) { m_player.play(); }
}

void InputController::handleCommandMode() {
    std::string cmd = m_term.read_line("\r:\033[K");
    std::cout << "\033[F" << ":" << cmd << "\033[K" << std::flush;

    if (cmd == "set lyrics") {
        m_showLyrics = true;
        m_lyricsWidget.show();
    } else if (cmd == "set nolyrics") {
        m_showLyrics = false;
        m_lyricsWidget.hide();
    }
}
