#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
// 确保传入的文件是MP3格式
bool isMP3File(const std::string& filename) {
    return filename.size() > 4 && filename.substr(filename.size() - 4) == ".mp3";
}

int main(int argc, char* argv[]) {
    // 检查是否提供了文件名
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file path to MP3>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    if (!isMP3File(filepath)) {
        std::cerr << "错误: 请输入一个.mp3文件" << std::endl;
        return 1;
    }

    // 从文件加载音乐
    // sf::Music 适用于大文件，它会流式加载，避免占用过多内存 [citation:3]
    sf::Music music;
    if (!music.openFromFile(filepath)) {
        // 如果打开失败，SFML内部会尝试使用已注册的解码器，但我们需要确保mpg123支持已被添加
        std::cerr << "错误: 无法打开音乐文件，请确保已安装libmpg123-dev并链接" << std::endl;
        return 1;
    }

    // 打印音乐信息
    std::cout << "正在播放: " << filepath << std::endl;
    std::cout << "时长: " << music.getDuration().asSeconds() << " 秒" << std::endl;
    std::cout << "采样率: " << music.getSampleRate() << " Hz" << std::endl;
    std::cout << "声道数: " << music.getChannelCount() << std::endl;

    while (true) {
        music.play();

        // 在播放过程中，我们可以执行其他操作，或者简单地等待播放结束
        std::cout << "正在播放... 按 Ctrl+C 停止" << std::endl;
        while (music.getStatus() == sf::Music::Playing) {
            // 可以在这里添加歌词同步等逻辑
            // 例如: 获取当前播放位置 music.getPlayingOffset().asSeconds();
            sf::sleep(sf::milliseconds(100)); // 短暂休眠，减少CPU占用
        }

        std::cout << "播放结束。" << std::endl;
    }
    // 开始播放
    return 0;
}
