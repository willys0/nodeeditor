#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include "GraphicsScene.hpp"

#include "Connection.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "ConnectionStyle.hpp"
#include "StyleCollection.hpp"
//#include "ConnectionBlurEffect.hpp"

#include "NodeGraphicsObject.hpp"

#include "NodeConnectionInteraction.hpp"

#include "Node.hpp"

using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionStyle;
using QtNodes::GraphicsScene;
using QtNodes::StyleCollection;
using ConnectionId = ConnectionGraphicsObject::ConnectionId;


ConnectionGraphicsObject::
ConnectionGraphicsObject(GraphicsScene & scene,
                         ConnectionId    connectionId)
  : _scene(scene)
  , _connectionState()
  , _connectionId(connectionId)
  , _state(State::Pending)
  , _in{0, 0}
  , _out{0, 0}
  , _hovered(false)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable,    true);
  setFlag(QGraphicsItem::ItemIsFocusable,  true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setAcceptHoverEvents(true);

  // addGraphicsEffect();

  setZValue(-1.0);
}


ConnectionGraphicsObject::
~ConnectionGraphicsObject()
{
  _scene.removeItem(this);
}


ConnectionId
ConnectionGraphicsObject::
connectionId()
{
  return _connectionId;
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  auto points = pointsC1C2();

  // `normalized()` fixes inverted rects.
  QRectF basicRect = QRectF(_out, _in).normalized();

  QRectF c1c2Rect = QRectF(points.first, points.second).normalized();

  QRectF commonRect = basicRect.united(c1c2Rect);

  auto const & connectionStyle = StyleCollection::connectionStyle();
  float const diam = connectionStyle.pointDiameter();
  QPointF const cornerOffset(diam, diam);

  // Expand rect by port circle diameter
  commonRect.setTopLeft(commonRect.topLeft() - cornerOffset);
  commonRect.setBottomRight(commonRect.bottomRight() + 2 * cornerOffset);

  return commonRect;
}


QPainterPath
ConnectionGraphicsObject::
shape() const
{
#ifdef DEBUG_DRAWING

  //QPainterPath path;

  //path.addRect(boundingRect());
  //return path;

#else
  return ConnectionPainter::getPainterStroke(*this);
#endif
}


QPointF const &
ConnectionGraphicsObject::
endPoint(PortType portType) const
{
  Q_ASSERT(portType != PortType::None);

  return (portType == PortType::Out ?
          _out :
          _in);
}


void
ConnectionGraphicsObject::
setEndPoint(PortType portType, QPointF const & point)
{
  QPointF & p = endPoint(portType);
  p = point;
}


void
ConnectionGraphicsObject::
moveEndPointBy(PortType portType, QPointF const & offset)
{
  QPointF & p = endPoint(portType);
  p += offset;
}


void
ConnectionGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
ConnectionGraphicsObject::
move()
{
  for (PortType portType: { PortType::In, PortType::Out })
  {
    NodeId const nodeId = (portType == PortType::Out) ?
                          _connectionId.first :
                          _connectionId.second;

    if (nodeId == InvalidNodeId)
      continue;

    GraphModel & graphModel = _scene.graphModel();

    if (auto node = _connectionId.getNode(portType))
    {
      auto const & nodeGraphics = node->nodeGraphicsObject();

      auto const & nodeGeom = node->nodeGeometry();

      QPointF scenePos =
        nodeGeom.portScenePosition(_connectionId.getPortIndex(portType),
                                   portType,
                                   nodeGraphics.sceneTransform());

      QTransform sceneTransform = this->sceneTransform();

      QPointF connectionPos = sceneTransform.inverted().map(scenePos);

      _connectionId.setEndPoint(portType, connectionPos);

      _connectionId.getConnectionGraphicsObject().setGeometryChanged();
      _connectionId.getConnectionGraphicsObject().update();
    }
  }

}


void
ConnectionGraphicsObject::
lock(bool locked)
{
  setFlag(QGraphicsItem::ItemIsMovable,    !locked);
  setFlag(QGraphicsItem::ItemIsFocusable,  !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}


void
ConnectionGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const * option,
      QWidget *)
{
  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter,
                           _connectionId);
}


void
ConnectionGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mousePressEvent(event);
  //event->ignore();
}


void
ConnectionGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  prepareGeometryChange();

  auto view = static_cast<QGraphicsView *>(event->widget());
  auto node = locateNodeAt(event->scenePos(),
                           _scene,
                           view->transform());

  auto & state = _connectionId.connectionState();

  state.interactWithNode(node);
  if (node)
  {
    node->reactToPossibleConnection(state.requiredPort(),
                                    _connectionId.dataType(oppositePort(state.requiredPort())),
                                    event->scenePos());
  }

  //-------------------

  QPointF offset = event->pos() - event->lastPos();

  auto requiredPort = _connectionId.requiredPort();

  if (requiredPort != PortType::None)
  {
    _connectionId.moveEndPointBy(requiredPort, offset);
  }

  //-------------------

  update();

  event->accept();
}


void
ConnectionGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  ungrabMouse();
  event->accept();

  auto node = locateNodeAt(event->scenePos(), _scene,
                           _scene.views()[0]->transform());

  NodeConnectionInteraction interaction(*node, _connectionId, _scene);

  if (node && interaction.tryConnect())
  {
    node->resetReactionToConnection();
  }

  if (_connectionId.connectionState().requiresPort())
  {
    _scene.deleteConnection(_connectionId);
  }
}


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  _connectionId.connectionGeometry().setHovered(true);

  update();
  _scene.connectionHovered(connectionId(), event->screenPos());
  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _connection.connectionGeometry().setHovered(false);

  update();
  _scene.connectionHoverLeft(connection());
  event->accept();
}


std::pair<QPointF, QPointF>
ConnectionGraphicsObject::
pointsC1C2() const
{
  const double defaultOffset = 200;

  double xDistance = _in.x() - _out.x();

  double horizontalOffset = qMin(defaultOffset, std::abs(xDistance));

  double verticalOffset = 0;

  double ratioX = 0.5;

  if (xDistance <= 0)
  {
    double yDistance = _in.y() - _out.y() + 20;

    double vector = yDistance < 0 ? -1.0 : 1.0;

    verticalOffset = qMin(defaultOffset, std::abs(yDistance)) * vector;

    ratioX = 1.0;
  }

  horizontalOffset *= ratioX;

  QPointF c1(_out.x() + horizontalOffset,
             _out.y() + verticalOffset);

  QPointF c2(_in.x() - horizontalOffset,
             _in.y() - verticalOffset);

  return std::make_pair(c1, c2);
}


void
ConnectionGraphicsObject::
addGraphicsEffect()
{
  auto effect = new QGraphicsBlurEffect;

  effect->setBlurRadius(5);
  setGraphicsEffect(effect);

  //auto effect = new QGraphicsDropShadowEffect;
  //auto effect = new ConnectionBlurEffect(this);
  //effect->setOffset(4, 4);
  //effect->setColor(QColor(Qt::gray).darker(800));
}

