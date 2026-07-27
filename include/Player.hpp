#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Audio.hpp>
#include <string>

/**
 * @brief 音乐播放器类，封装了MP3播放的核心功能
 */
class Player {
    public:
        /**
         * @brief 构造函数
         * @param filePath MP3文件的路径
         */
        Player(const std::string& filePath);
        
        /**
         * @brief 播放音乐
         * @return 是否成功开始播放
         */
        bool play();
        
        /**
         * @brief 暂停音乐
         */
        void pause();
        
        /**
         * @brief 停止音乐
         */
        void stop();
        
        /**
         * @brief 设置播放的起始位置
         */
        void setCurrentTime(float time);

        /**
         * @brief 获取当前播放位置（秒）
         * @return 当前播放进度（秒）
         */
        float getCurrentTime() const;
        
        /**
         * @brief 获取音乐总时长（秒）
         * @return 总时长（秒）
         */
        float getDuration() const;
        
        /**
         * @brief 检查音乐是否正在播放
         * @return true表示正在播放
         */
        bool isPlaying() const;
        
        /**
         * @brief 检查音乐是否加载成功
         * @return true表示加载成功
         */
        bool isValid() const { return _isValid; }
        
        /**
         * @brief 获取错误信息
         * @return 错误描述字符串
         */
        std::string getError() const { return _errorMessage; }
        
        /**
         * @brief 获取音乐信息（用于显示）
         */
        void printInfo() const;

    private:
        sf::Music _music;              // SFML音乐对象
        std::string _filePath;         // 文件路径
        std::string _errorMessage;     // 错误信息
        bool _isValid;                 // 是否有效
        
        /**
         * @brief 验证文件是否为MP3格式
         */
        bool isMP3File(const std::string& filename) const;
};

#endif // PLAYER_HPP
