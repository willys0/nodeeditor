#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <functional>
#include <tuple>
#include <unordered_map>

#include "DataModelRegistry.hpp"
#include "Definitions.hpp"
#include "Export.hpp"
#include "GraphModel.hpp"
#include "TypeConverter.hpp"

#include "memory.hpp"
#include "QUuidStdHash.hpp"

namespace QtNodes
{

class ConnectionGraphicsObject;
class GraphModel;
class NodeGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC NodeGraphicsScene : public QGraphicsScene
{
  Q_OBJECT
public:

  //NodeGraphicsScene(std::shared_ptr<DataModelRegistry> registry,
  //QObject * parent = Q_NULLPTR);

  NodeGraphicsScene(GraphiModel & graphModel);

  NodeGraphicsScene(QObject * parent = Q_NULLPTR);

  ~NodeGraphicsScene();

public:

  std::shared_ptr<Connection>createConnection(PortType  connectedPort,
                                              Node &    node,
                                              PortIndex portIndex);

  std::shared_ptr<Connection>createConnection(Node &                nodeIn,
                                              PortIndex             portIndexIn,
                                              Node &                nodeOut,
                                              PortIndex             portIndexOut,
                                              TypeConverter const & converter = TypeConverter{});

  std::shared_ptr<Connection> restoreConnection(QJsonObject const & connectionJson);

  void deleteConnection(Connection & connection);

  Node & createNode(std::unique_ptr<NodeDataModel> && dataModel);

  Node & restoreNode(QJsonObject const & nodeJson);

  void removeNode(Node & node);

  DataModelRegistry & registry() const;

  void setRegistry(std::shared_ptr<DataModelRegistry> registry);

  void iterateOverNodes(std::function<void(Node *)> const & visitor);

  void iterateOverNodeData(std::function<void(NodeDataModel *)> const & visitor);

  void iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel *)> const & visitor);

  QPointF getNodePosition(Node const & node) const;

  void setNodePosition(Node & node, QPointF const & pos) const;

  QSizeF getNodeSize(Node const & node) const;

public:

  //std::unordered_map<QUuid, std::unique_ptr<Node> > const & nodes() const;

  //std::unordered_map<QUuid, std::shared_ptr<Connection> > const & connections() const;

  //std::vector<Node*> allNodes() const;

  //std::vector<Node*> selectedNodes() const;

public:

  void clearScene();

  void save() const;

  void load();

  QByteArray saveToMemory() const;

  void loadFromMemory(const QByteArray & data);

Q_SIGNALS:

  /**
   * @brief Node has been created but not on the scene yet.
   * @see nodePlaced()
   */
  //void nodeCreated(Node &n);

  /**
   * @brief Node has been added to the scene.
   * @details Connect to this signal if need a correct position of node.
   * @see nodeCreated()
   */
  //void nodePlaced(Node &n);

  //void nodeDeleted(Node &n);

  //void connectionCreated(Connection const &c);
  //void connectionDeleted(Connection const &c);

  //void nodeMoved(Node& n, const QPointF& newLocation);

  //void nodeDoubleClicked(Node& n);

  //void connectionHovered(Connection& c, QPoint screenPos);

  //void nodeHovered(Node& n, QPoint screenPos);

  //void connectionHoverLeft(Connection& c);

  //void nodeHoverLeft(Node& n);

  //void nodeContextMenu(Node& n, const QPointF& pos);

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  // TODO shared pointer?
  GraphModel & _graphModel;


  std::unordered_map<NodeId,
                     std::unique_ptr<NodeGraphicsObject>>
  _nodeGraphicObjects;

  std::unordered_map<std::pair<NodeId, NodeId>,
                     std::unique_ptr<ConnectionGraphicsObject>>
  _connectionGraphicObjects;



  //std::unordered_map<QUuid, SharedConnection> _connections;
  //std::unordered_map<QUuid, UniqueNode>       _nodes;
  //std::shared_ptr<DataModelRegistry>          _registry;

private Q_SLOTS:

  //void setupConnectionSignals(Connection const& c);

  //void sendConnectionCreatedToNodes(Connection const& c);
  //void sendConnectionDeletedToNodes(Connection const& c);
};


}
