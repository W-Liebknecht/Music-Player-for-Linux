#ifndef DESKTOP_LYRICS_HPP
#define DESKTOP_LYRICS_HPP

#include <QWidget>
#include <QString>
#include <QPoint>
#include <QFont>

class DesktopLyrics : public QWidget {
    Q_OBJECT

public:
    explicit DesktopLyrics(QWidget *parent = nullptr);
    ~DesktopLyrics() override = default;

    /**
     * @brief 设置当前显示的歌词文本并自动更新画面
     * @param text 要显示的歌词内容
     */
    void setText(const QString &text);

    /**
     * @brief 获取当前显示的歌词文本
     */
    QString text() const { return m_lyricText; }

    /**
     * @brief 将窗口移动并居中在屏幕底部
     * @param paddingBottom 距离底部的像素边距
     */
    void positionAtBottom(int paddingBottom = 30);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void updateWidgetSize();

    QString m_lyricText;
    QFont m_font;
    QPoint m_dragPosition;
};

#endif // DESKTOP_LYRICS_HPP
