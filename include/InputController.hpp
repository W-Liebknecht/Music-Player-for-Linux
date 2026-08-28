#ifndef INPUT_CONTROLLER_HPP
#define INPUT_CONTROLLER_HPP

#include "Player.hpp"
#include "DesktopLyrics.hpp"
#include "SignalInput.hpp"

#include <QString>
#include <string>

class InputController {
public:
    InputController(Player& player, DesktopLyrics& lyricsWidget, TerminalRaw& term, const QString& songTitle);

    /**
     * @brief 处理一次键盘轮询事件
     * @return 如果返回 false，说明用户按下了 ESC 退出程序；返回 true 则继续运行
     */
    bool handleInput();

    /**
     * @brief 获取当前歌词开关状态
     */
    bool isLyricsVisible() const { return m_showLyrics; }

    /**
     * @brief 获取当前暂停状态
     */
    bool isPaused() const { return m_paused; }

private:
    void handleCommandMode();
    void seekRelative(float seconds);

private:
    Player& m_player;
    DesktopLyrics& m_lyricsWidget;
    TerminalRaw& m_term;
    QString m_songTitle;

    bool m_paused = false;
    bool m_showLyrics = true;
};

#endif // INPUT_CONTROLLER_HPP
