#ifndef PANNINGVIEW_HPP
#define PANNINGVIEW_HPP

#include <QGraphicsView>
#include <QPoint>

class QWheelEvent;
class QMouseEvent;

class PanningView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PanningView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isPanning{false};
    QPoint m_lastMousePos;

    static constexpr float zoomFactor = 1.15f;
};

#endif // PANNINGVIEW_HPP