#pragma once

#include <utility>

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Definitions.hpp"

class QGraphicsSceneMouseEvent;

namespace QtNodes
{

class GraphicsScene;
class Connection;
class ConnectionGeometry;

/// Graphic Object for connection. Adds itself to scene
class ConnectionGraphicsObject
  : public QGraphicsObject
{
  Q_OBJECT

public:

  using ConnectionId = std::pair<NodeId, NodeId>;

  ConnectionGraphicsObject(GraphicsScene & scene,
                           ConnectionId    connectionId);

  virtual ~ConnectionGraphicsObject();

  enum { Type = UserType + 2 };
  int type() const override { return Type; }

public:

  ConnectionId connectionId();

  QRectF boundingRect() const override;

  QPainterPath shape() const override;

  void setGeometryChanged();

  /// Updates the position of both ends
  void move();

  void lock(bool locked);

protected:

  void paint(QPainter * painter,
             QStyleOptionGraphicsItem const * option,
             QWidget *  widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent * event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;

private:

  void addGraphicsEffect();

private:

  GraphicsScene & _scene;

  ConnectionId _connectionId;
};
}
