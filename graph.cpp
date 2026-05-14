#include "graph.hpp"
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QString>
#include <cmath>
#include <queue>

namespace {
QString nodeTypeToString(NodeType type)
{
    switch (type) {
    case NodeType::Customer:
        return QStringLiteral("Customer");
    case NodeType::PMedianCandidate:
        return QStringLiteral("PMedianCandidate");
    case NodeType::ChosenMedian:
        return QStringLiteral("ChosenMedian");
    default:
        return QStringLiteral("Customer");
    }
}

NodeType stringToNodeType(const QString &str)
{
    if (str == QStringLiteral("PMedianCandidate"))
        return NodeType::PMedianCandidate;
    if (str == QStringLiteral("ChosenMedian"))
        return NodeType::ChosenMedian;
    return NodeType::Customer;
}
} // namespace

NodeId Graph::addNode(QPointF pos)
{
    NodeId id = static_cast<NodeId>(m_nextNodeId++);
    m_nodes[id] = Node{.id = id, .pos = pos, .type = NodeType::Customer, .visited = false};
    emit nodeAdded(id);
    return id;
}

void Graph::removeNode(NodeId id)
{
    if (!m_nodes.contains(id))
        return;

    std::vector<EdgeId> edgesToRemove;
    for (const auto &[edgeId, edge] : m_edges) {
        if (edge.from == id || edge.to == id) {
            edgesToRemove.push_back(edgeId);
        }
    }

    for (EdgeId eId : edgesToRemove) {
        removeEdge(eId);
    }

    m_nodes.erase(id);
    emit nodeRemoved(id);
}

void Graph::moveNode(NodeId id, QPointF newPos)
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end())
        return;

    it->second.pos = newPos;
    emit nodeMoved(id, newPos);

    for (auto &[edgeId, edge] : m_edges) {
        if ((edge.from == id || edge.to == id) && !edge.isLengthManual) {
            edge.length = calculateDistance(m_nodes[edge.from].pos, m_nodes[edge.to].pos);
            emit edgeUpdated(edgeId);
        }
    }
}

void Graph::updateNode(const Node &node)
{
    auto it = m_nodes.find(node.id);
    if (it == m_nodes.end())
        return;

    bool posChanged = (it->second.pos != node.pos);
    it->second = node;

    if (posChanged) {
        moveNode(node.id, node.pos);
    } else {
        emit nodeUpdated(node.id);
    }
}

void Graph::setNodeType(NodeId id, NodeType type)
{
    auto it = m_nodes.find(id);
    if (it != m_nodes.end() && it->second.type != type) {
        it->second.type = type;
        emit nodeUpdated(id);
    }
}

EdgeId Graph::addEdge(NodeId from, NodeId to, std::optional<float> manualLength)
{
    if (from == to || !m_nodes.contains(from) || !m_nodes.contains(to)) {
        return EdgeId::Invalid;
    }

    for (const auto &[id, edge] : m_edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return id;
        }
    }

    EdgeId id = static_cast<EdgeId>(m_nextEdgeId++);
    float length = manualLength.has_value() ? *manualLength
                                            : calculateDistance(m_nodes[from].pos, m_nodes[to].pos);

    m_edges[id] = Edge{.id = id,
                       .from = from,
                       .to = to,
                       .length = length,
                       .isLengthManual = manualLength.has_value()};

    emit edgeAdded(id);
    return id;
}

void Graph::addSolutionEdge(NodeId from, NodeId to)
{
    m_solEdges.emplace_back(EdgeId::Invalid, from, to, 0.0, true, true, true);
    emit solutionEdgeAdded();
}

void Graph::removeEdge(EdgeId id)
{
    if (m_edges.erase(id)) {
        emit edgeRemoved(id);
    }
}

void Graph::updateEdge(const Edge &edge)
{
    if (m_edges.contains(edge.id)) {
        m_edges[edge.id] = edge;
        emit edgeUpdated(edge.id);
    }
}

void Graph::removeSolution()
{
    std::erase_if(m_edges, [](const auto &item) { return item.second.isSolverOutput; });
    for (auto &[id, node] : m_nodes) {
        if (node.type == NodeType::ChosenMedian) {
            node.type = NodeType::PMedianCandidate;
            emit nodeUpdated(id);
        }
    }
    emit solutionEdgesRemoved();
}

const Node *Graph::findNode(NodeId id) const
{
    auto it = m_nodes.find(id);
    return (it != m_nodes.end()) ? &it->second : nullptr;
}

const Edge *Graph::findEdge(EdgeId id) const
{
    auto it = m_edges.find(id);
    return (it != m_edges.end()) ? &it->second : nullptr;
}

const Edge Graph::lastAddedSolutionEdge() const
{
    return m_solEdges.back();
}

bool Graph::isFullyConnected() const
{
    if (m_nodes.size() <= 1) {
        return true;
    }

    std::unordered_map<NodeId, std::vector<NodeId>, NodeIdHash> adjList;
    for (const auto &[edgeId, edge] : m_edges) {
        if (!edge.isEnabled) {
            continue;
        }
        adjList[edge.from].push_back(edge.to);
        adjList[edge.to].push_back(edge.from);
    }

    std::unordered_set<NodeId, NodeIdHash> visited;
    std::queue<NodeId> queue;

    const NodeId startNode = m_nodes.begin()->first;
    queue.push(startNode);
    visited.insert(startNode);

    while (!queue.empty()) {
        const NodeId curr = queue.front();
        queue.pop();

        if (auto it = adjList.find(curr); it != adjList.end()) {
            for (const NodeId neighbor : it->second) {
                if (!visited.contains(neighbor)) {
                    visited.insert(neighbor);
                    queue.push(neighbor);
                }
            }
        }
    }

    return visited.size() == m_nodes.size();
}

void Graph::clear()
{
    auto oldEdges{std::move(m_edges)};
    for (const auto &[id, edge] : oldEdges) {
        emit edgeRemoved(id);
    }

    auto oldNodes{std::move(m_nodes)};
    for (const auto &[id, node] : oldNodes) {
        emit nodeRemoved(id);
    }

    m_solEdges.clear();
    emit solutionEdgesRemoved();

    m_nextNodeId = 1;
    m_nextEdgeId = 1;
}

bool Graph::save(const QString &path) const
{
    QSaveFile file{path};
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray nodesArr;
    for (const auto &[id, node] : m_nodes) {
        QJsonObject nodeObj;
        nodeObj.insert(QStringLiteral("id"), static_cast<double>(id));
        nodeObj.insert(QStringLiteral("x"), node.pos.x());
        nodeObj.insert(QStringLiteral("y"), node.pos.y());
        nodeObj.insert(QStringLiteral("type"), nodeTypeToString(node.type));
        nodeObj.insert(QStringLiteral("visited"), node.visited);
        nodesArr.append(nodeObj);
    }

    QJsonArray edgesArr;
    for (const auto &[id, edge] : m_edges) {
        QJsonObject edgeObj;
        edgeObj.insert(QStringLiteral("id"), static_cast<double>(edge.id));
        edgeObj.insert(QStringLiteral("from"), static_cast<double>(edge.from));
        edgeObj.insert(QStringLiteral("to"), static_cast<double>(edge.to));
        edgeObj.insert(QStringLiteral("length"), static_cast<double>(edge.length));
        edgeObj.insert(QStringLiteral("isEnabled"), edge.isEnabled);
        edgeObj.insert(QStringLiteral("isLengthManual"), edge.isLengthManual);
        edgeObj.insert(QStringLiteral("isSolverOutput"), edge.isSolverOutput);
        edgesArr.append(edgeObj);
    }

    QJsonObject root;
    root.insert(QStringLiteral("nextNodeId"), static_cast<double>(m_nextNodeId));
    root.insert(QStringLiteral("nextEdgeId"), static_cast<double>(m_nextEdgeId));
    root.insert(QStringLiteral("nodes"), nodesArr);
    root.insert(QStringLiteral("edges"), edgesArr);

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));

    return file.commit();
}

bool Graph::load(const QString &path)
{
    QFile file{path};
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << path;
        return false;
    }

    QJsonParseError err;
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON Parse error:" << err.errorString();
        return false;
    }

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "JSON is not an object.";
        return false;
    }

    QJsonObject root = doc.object();

    std::unordered_map<NodeId, Node, NodeIdHash> tempNodes;
    std::unordered_map<EdgeId, Edge, EdgeIdHash> tempEdges;

    uint32_t tempNextNodeId = static_cast<uint32_t>(
        root[QStringLiteral("nextNodeId")].toVariant().toUInt());
    uint32_t tempNextEdgeId = static_cast<uint32_t>(
        root[QStringLiteral("nextEdgeId")].toVariant().toUInt());

    QJsonArray nodesArr = root[QStringLiteral("nodes")].toArray();
    for (const QJsonValue &val : std::as_const(nodesArr)) {
        if (!val.isObject()) {
            return false;
        }
        QJsonObject obj = val.toObject();

        NodeId id = static_cast<NodeId>(obj[QStringLiteral("id")].toVariant().toUInt());
        if (id == NodeId::Invalid)
            return false;

        Node node;
        node.id = id;
        node.pos = QPointF{obj[QStringLiteral("x")].toDouble(), obj[QStringLiteral("y")].toDouble()};
        node.type = stringToNodeType(obj[QStringLiteral("type")].toString());
        node.visited = obj[QStringLiteral("visited")].toBool();

        tempNodes.emplace(id, node);
    }

    QJsonArray edgesArr = root[QStringLiteral("edges")].toArray();
    for (const QJsonValue &val : std::as_const(edgesArr)) {
        if (!val.isObject())
            return false;
        QJsonObject obj = val.toObject();

        EdgeId id = static_cast<EdgeId>(obj[QStringLiteral("id")].toVariant().toUInt());
        if (id == EdgeId::Invalid)
            return false;

        Edge edge;
        edge.id = id;
        edge.from = static_cast<NodeId>(obj[QStringLiteral("from")].toVariant().toUInt());
        edge.to = static_cast<NodeId>(obj[QStringLiteral("to")].toVariant().toUInt());
        edge.length = static_cast<float>(obj[QStringLiteral("length")].toDouble());
        edge.isEnabled = obj[QStringLiteral("isEnabled")].toBool(true);
        edge.isLengthManual = obj[QStringLiteral("isLengthManual")].toBool(false);
        edge.isSolverOutput = obj[QStringLiteral("isSolverOutput")].toBool(false);

        tempEdges.emplace(id, edge);
    }

    clear();
    m_nextNodeId = tempNextNodeId;
    m_nextEdgeId = tempNextEdgeId;
    m_nodes = std::move(tempNodes);
    m_edges = std::move(tempEdges);

    for (const auto &[id, node] : m_nodes)
        emit nodeAdded(id);
    for (const auto &[id, edge] : m_edges)
        emit edgeAdded(id);

    return true;
}

float Graph::calculateDistance(QPointF a, QPointF b) const
{
    return static_cast<float>(std::hypot(a.x() - b.x(), a.y() - b.y()));
}