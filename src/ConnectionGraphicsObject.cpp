#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include "GraphicsScene.hpp"

#include "Connection.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "ConnectionBlurEffect.hpp"

#include "NodeGraphicsObject.hpp"

#include "NodeConnectionInteraction.hpp"

#include "Node.hpp"

using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionId;
using QtNodes::GraphicsScene;

ConnectionGraphicsObject::
ConnectionGraphicsObject(GraphicsScene & scene,
                         ConnectionId    connectionId)
  : _scene(scene)
  , _connectionId(connectionId)
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
  return _connection.connectionGeometry().boundingRect();
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
  auto const & geom =
    _connectionId.connectionGeometry();

  return ConnectionPainter::getPainterStroke(geom);

#endif
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

      _connectionId.connectionGeometry().setEndPoint(portType,
                                                     connectionPos);

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
    _connectionId.connectionGeometry().moveEndPoint(requiredPort, offset);
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

