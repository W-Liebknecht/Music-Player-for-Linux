#include "DesktopLyrics.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QGuiApplication>
#include <QScreen>
#include <QFontMetrics>

DesktopLyrics::DesktopLyrics(QWidget *parent)
    : QWidget(parent)
    , m_lyricText("✨ 悬浮歌词组件已就绪 ✨")
    , m_font("Sans-Serif", 28, QFont::Bold)
{
    // 1. 窗口属性设置：无边框 | 置顶 | 工具窗口
    setWindowFlags(Qt::FramelessWindowHint | 
                   Qt::WindowStaysOnTopHint | 
                   Qt::Tool);

    // 2. 背景透明
    setAttribute(Qt::WA_TranslucentBackground, true);

    // 3. 根据初始文字调整窗口尺寸并定位到底部
    updateWidgetSize();
    positionAtBottom();
}

void DesktopLyrics::setText(const QString &text) {
    if (m_lyricText != text) {
        m_lyricText = text;
        updateWidgetSize(); // 文字变化时重新计算窗口宽高
        update();           // 触发重绘 (paintEvent)
    }
}

void DesktopLyrics::updateWidgetSize() {
    // 使用 QFontMetrics 测量字体渲染后的实际像素宽高
    QFontMetrics fm(m_font);
    int textWidth = fm.horizontalAdvance(m_lyricText);
    int textHeight = fm.height();

    // 留出适当边距（防止描边被裁剪）
    int newWidth = textWidth + 60;
    int newHeight = textHeight + 40;

    resize(newWidth, newHeight);
}

void DesktopLyrics::positionAtBottom(int paddingBottom) {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QRect screenGeometry = screen->availableGeometry();

    // 计算水平居中与底部边距位置
    int x = screenGeometry.x() + (screenGeometry.width() - width()) / 2;
    int y = screenGeometry.y() + screenGeometry.height() - height() - paddingBottom;

    move(x, y);
}

void DesktopLyrics::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if (m_lyricText.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算基线位置，使文字居中绘制
    QFontMetrics fm(m_font);
    int baselineY = (height() + fm.capHeight()) / 2;
    int startX = 30; // 左侧边距

    QPainterPath path;
    path.addText(startX, baselineY, m_font, m_lyricText);

    // 绘制黑色外发光/描边
    QPen outlinePen(QColor(0, 0, 0, 220), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(outlinePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    // 填充渐变颜色
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, QColor(255, 110, 196));
    gradient.setColorAt(0.5, QColor(120, 115, 245));
    gradient.setColorAt(1.0, QColor(0, 210, 255));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawPath(path);
}

void DesktopLyrics::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void DesktopLyrics::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}
