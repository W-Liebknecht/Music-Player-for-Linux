#ifndef LYRICS_PARSER_HPP
#define LYRICS_PARSER_HPP

#include <string>
#include <vector>
#include <map>

// 单行歌词数据结构
struct LyricLine {
    float timePoint;     // 时间戳（单位：秒，如 14.940f）
    std::string text;    // 歌词内容
};

class LyricsParser {
public:
    LyricsParser() = default;

    /**
     * @brief 加载歌词（自动识别：优先读 TagLib 内嵌歌词，失败则读同名 .lrc 文件）
     * @param audioFilePath 音频文件路径 (如 "/path/to/song.mp3")
     * @return 是否成功加载并解析出歌词
     */
    bool load(const std::string& audioFilePath);

    /**
     * @brief 直接从字符串解析歌词内容
     * @param rawLyricText LRC 格式的歌词文本
     */
    bool parseString(const std::string& rawLyricText);

    /**
     * @brief 根据当前播放时间获取对应的歌词
     * @param currentTimeSeconds 当前播放时间（秒，如 15.2f）
     * @return 当前应该显示的歌词字符串，如果未匹配到则返回空或占位符
     */
    std::string getLyricAt(float currentTimeSeconds) const;

    /**
     * @brief 判断是否包含有效歌词
     */
    bool hasLyrics() const { return !m_lyrics.empty(); }

    /**
     * @brief 清空已加载的歌词
     */
    void clear() { m_lyrics.clear(); }

private:
    // 解析时间戳字符串 "[01:14.940]" -> 转换为秒数 float
    float parseTimestamp(const std::string& timeStr) const;

    // 从磁盘本地读取 .lrc 文件内容
    std::string readLrcFile(const std::string& lrcFilePath) const;

    // 从 TagLib 读取内嵌歌词
    std::string readEmbeddedLyrics(const std::string& audioFilePath) const;

    // 按时间顺序升序排列的歌词列表
    std::vector<LyricLine> m_lyrics;
};

#endif // LYRICS_PARSER_HPP
