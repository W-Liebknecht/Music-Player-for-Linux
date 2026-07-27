#include "Player.hpp"
#include <iostream>

// 构造函数：初始化成员变量并尝试加载音乐
Player::Player(const std::string& filePath)
    : _filePath(filePath)
    , _isValid(false)
    , _errorMessage("") {
    
    // 验证文件格式
    if (!isMP3File(_filePath)) {
        _errorMessage = "错误: 文件不是MP3格式";
        return;
    }
    
    // 尝试加载音乐文件
    if (!_music.openFromFile(_filePath)) {
        _errorMessage = "错误: 无法打开音乐文件，请确保已安装libmpg123-dev并链接";
        return;
    }
    
    _isValid = true;
}

// 播放音乐
bool Player::play() {
    if (!_isValid) {
        std::cerr << "Player错误: 音乐未正确加载" << std::endl;
        return false;
    }
    
    _music.play();
    return true;
}

// 暂停音乐
void Player::pause() {
    if (_isValid) {
        _music.pause();
    }
}

// 停止音乐
void Player::stop() {
    if (_isValid) {
        _music.stop();
    }
}

// 设置播放的起始位置
void Player::setCurrentTime(float time) {
    if (_isValid) {
        _music.setPlayingOffset(sf::seconds(time));
    }
}

// 获取当前播放位置（秒）
float Player::getCurrentTime() const {
    if (_isValid) {
        return _music.getPlayingOffset().asSeconds();
    }
    return 0.0f;
}

// 获取音乐总时长（秒）
float Player::getDuration() const {
    if (_isValid) {
        return _music.getDuration().asSeconds();
    }
    return 0.0f;
}

// 检查是否正在播放
bool Player::isPlaying() const {
    if (_isValid) {
        return _music.getStatus() == sf::Music::Playing;
    }
    return false;
}

// 打印音乐信息
void Player::printInfo() const {
    if (!_isValid) {
        std::cerr << "Player错误: 音乐未正确加载" << std::endl;
        return;
    }
    
    std::cout << "正在播放: " << _filePath << std::endl;
    std::cout << "时长: " << getDuration() << " 秒" << std::endl;
    std::cout << "采样率: " << _music.getSampleRate() << " Hz" << std::endl;
    std::cout << "声道数: " << _music.getChannelCount() << std::endl;
}

// 验证MP3文件格式
bool Player::isMP3File(const std::string& filename) const {
    return filename.size() > 4 && 
           filename.substr(filename.size() - 4) == ".mp3";
}
