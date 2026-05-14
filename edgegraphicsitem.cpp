#include "edgegraphicsitem.hpp"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "nodegraphicsitem.hpp"

EdgeGraphicsItem::EdgeGraphicsItem(EdgeId id, NodeGraphicsItem *fromNode, NodeGraphicsItem *toNode)
    : m_id(id)
    , m_fromNode(fromNode)
    , m_toNode(toNode)
{
    setZValue(-1.0);

    if (m_fromNode)
        m_fromNode->addConnectedEdge(this);
    if (m_toNode)
        m_toNode->addConnectedEdge(this);

    setFlags(ItemIsSelectable);
    updatePosition();
}

EdgeGraphicsItem::~EdgeGraphicsItem()
{
    if (m_fromNode) {
        m_fromNode->removeConnectedEdge(this);
    }
    if (m_toNode) {
        m_toNode->removeConnectedEdge(this);
    }
}
void EdgeGraphicsItem::updateFromModel(const Edge &data)
{
    m_length = data.length;
    m_isEnabled = data.isEnabled;
    m_isManual = data.isLengthManual;
    m_isSolverOutput = data.isSolverOutput;
    update();
}

void EdgeGraphicsItem::updatePosition()
{
    if (!m_fromNode || !m_toNode) [[unlikely]] {
        return;
    }

    prepareGeometryChange();

    const QPointF p1{mapFromScene(m_fromNode->scenePos())};
    const QPointF p2{mapFromScene(m_toNode->scenePos())};

    m_line = QLineF{p1, p2};

    const qreal margin{StrokeWidth + 40.0};
    m_boundingRect = QRectF{p1, p2}.normalized().adjusted(-margin, -margin, margin, margin);
}

QRectF EdgeGraphicsItem::boundingRect() const
{
    return m_boundingRect;
}

QPainterPath EdgeGraphicsItem::shape() const
{
    QPainterPath path{};
    if (m_line.isNull()) {
        return path;
    }

    path.moveTo(m_line.p1());
    path.lineTo(m_line.p2());

    QPainterPathStroker stroker{};
    stroker.setWidth(SelectionTolerance);
    stroker.setCapStyle(Qt::RoundCap);

    return stroker.createStroke(path);
}

void EdgeGraphicsItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(widget);

    if (m_line.isNull()) [[unlikely]] {
        return;
    }

    painter->setPen(determinePen(option));
    painter->drawLine(m_line);

    if (!m_isSolverOutput)
        drawLabel(painter);
}

QPen EdgeGraphicsItem::determinePen(const QStyleOptionGraphicsItem *option) const
{
    QPen pen{Qt::black, StrokeWidth, Qt::SolidLine, Qt::RoundCap};

    if (!(option->state & QStyle::State_Enabled) || !m_isEnabled) {
        pen.setColor(Qt::gray);
        pen.setStyle(Qt::DashLine);
    } else if (m_isSolverOutput) {
        pen.setColor(Qt::darkYellow);
    } else if (m_isManual) {
        pen.setColor(Qt::darkCyan);
    }

    if (option->state & QStyle::State_Selected) {
        pen.setColor(Qt::red);
        pen.setWidthF(StrokeWidth + 2.0);
    }

    return pen;
}

void EdgeGraphicsItem::drawLabel(QPainter *painter) const
{
    if (m_line.isNull()) [[unlikely]] {
        return;
    }

    const QString text{QString::number(m_length, 'f', 1)};
    const QFontMetricsF fm{painter->font()};
    const QRectF textRect = fm.boundingRect(text).adjusted(-2, -2, 2, 2);

    const qreal len{m_line.length()};
    const qreal offset{20.0};

    const QPointF normal{-m_line.dy() / len * offset, m_line.dx() / len * offset};
    const QPointF center{m_line.center()};

    qreal angle{-m_line.angle()};
    if (m_line.angle() > 90.0 && m_line.angle() < 270.0) {
        angle += 180.0;
    }

    painter->save();
    painter->translate(center + normal);
    painter->rotate(angle);
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF{-textRect.width() / 2.0,
                             -textRect.height() / 2.0,
                             textRect.width(),
                             textRect.height()},
                      Qt::AlignCenter,
                      text);
    painter->restore();
}