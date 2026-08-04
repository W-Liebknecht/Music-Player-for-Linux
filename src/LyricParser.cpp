#include "LyricParser.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

// TagLib 相关头文件
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/unsynchronizedlyricsframe.h>

bool LyricsParser::load(const std::string& audioFilePath) {
    clear();

    // 策略 1: 尝试读取同名 .lrc 外挂歌词文件
    std::string lrcPath = audioFilePath;
    size_t lastDot = lrcPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        lrcPath = lrcPath.substr(0, lastDot) + ".lrc";
    } else {
        lrcPath += ".lrc";
    }

    std::string rawLyrics = readLrcFile(lrcPath);

    // 策略 2: 如果找不到 .lrc 文件，尝试使用 TagLib 读取文件内嵌歌词
    if (rawLyrics.empty()) {
        rawLyrics = readEmbeddedLyrics(audioFilePath);
    }

    if (rawLyrics.empty()) {
        return false;
    }

    return parseString(rawLyrics);
}

bool LyricsParser::parseString(const std::string& rawLyricText) {
    m_lyrics.clear();

    std::istringstream stream(rawLyricText);
    std::string line;

    // 正则表达式匹配格式：[分:秒.毫秒] 或 [分:秒]
    // 捕获组 1: 时间戳内容，捕获组 2: 歌词文本
    std::regex lyricRegex(R"(\[([0-9]{2}:[0-9]{2}(?:\.[0-9]+)?)\](.*))");

    while (std::getline(stream, line)) {
        // 去除回车符 \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::smatch match;
        if (std::regex_match(line, match, lyricRegex)) {
            std::string timeStr = match[1].str();
            std::string textStr = match[2].str();

            float timeInSeconds = parseTimestamp(timeStr);

            // 过滤掉只有时间戳但文本为空的预备行（可选）
            m_lyrics.push_back({timeInSeconds, textStr});
        }
    }

    // 按时间顺序升序排序
    std::sort(m_lyrics.begin(), m_lyrics.end(), [](const LyricLine& a, const LyricLine& b) {
        return a.timePoint < b.timePoint;
    });

    return !m_lyrics.empty();
}

std::string LyricsParser::getLyricAt(float currentTimeSeconds) const {
    if (m_lyrics.empty()) {
        return "";
    }

    // 如果播放时间早于第一句歌词，返回第一句之前的内容或空
    if (currentTimeSeconds < m_lyrics.front().timePoint) {
        return "";
    }

    // 使用 std::lower_bound 进行二分查找
    // 寻找第一个时间点“大于”当前时间的歌词行
    auto it = std::lower_bound(m_lyrics.begin(), m_lyrics.end(), currentTimeSeconds,
        [](const LyricLine& line, float time) {
            return line.timePoint <= time;
        });

    // it 返回的是第一个 timePoint > currentTimeSeconds 的位置
    // 所以当前应该显示的歌词是前一行 (it - 1)
    if (it != m_lyrics.begin()) {
        return (it - 1)->text;
    }

    return m_lyrics.back().text;
}

float LyricsParser::parseTimestamp(const std::string& timeStr) const {
    // 传入格式类似于 "01:14.940" 或 "00:05.52"
    int minutes = 0;
    float seconds = 0.0f;

    sscanf(timeStr.c_str(), "%d:%f", &minutes, &seconds);

    return minutes * 60.0f + seconds;
}

std::string LyricsParser::readLrcFile(const std::string& lrcFilePath) const {
    std::ifstream file(lrcFilePath);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string LyricsParser::readEmbeddedLyrics(const std::string& audioFilePath) const {
    TagLib::MPEG::File audioFile(audioFilePath.c_str());
    if (!audioFile.isValid() || !audioFile.ID3v2Tag()) {
        return "";
    }

    TagLib::ID3v2::FrameList frameList = audioFile.ID3v2Tag()->frameList("USLT");
    if (frameList.isEmpty()) {
        return "";
    }

    for (auto* frame : frameList) {
        auto* lyricsFrame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frame);
        if (lyricsFrame) {
            return lyricsFrame->text().to8Bit(true);
        }
    }

    return "";
}
