#ifndef PANNINGVIEW_HPP
#define PANNINGVIEW_HPP

#include <QGraphicsView>
#include <QMouseEvent>
#include <QScrollBar>

class PanningView : public QGraphicsView
{
public:
    using QGraphicsView::QGraphicsView;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_lastPanPoint;
};

#endif // PANNINGVIEW_HPP
