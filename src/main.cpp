#include "Player.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <MP3文件路径>" << std::endl;
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
        std::cerr << "启动播放失败" << std::endl;
        return 1;
    }
    
    // 主循环：等待播放结束
    std::cout << "正在播放... 按 Ctrl+C 停止" << std::endl;
    while (player.isPlaying()) {
        // 在这里可以添加歌词同步逻辑
        // float currentTime = player.getCurrentTime();
        // 更新歌词显示...
        
        sf::sleep(sf::milliseconds(100));
    }
    
    std::cout << "播放结束。" << std::endl;
    return 0;
}
