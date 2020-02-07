#pragma once

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QTransform>
#include <QtGui/QFontMetrics>

#include "PortType.hpp"
#include "Export.hpp"
#include "memory.hpp"

namespace QtNodes
{

class NodeState;
class NodeDataModel;
class Node;

class NODE_EDITOR_PUBLIC NodeGeometry
{
public:
  NodeGeometry(NodeId nodeId, GraphModel const & graphModel);

public:
  QSize size();
  //void setSize(QSize s);

  unsigned int entryHeight() const;

  unsigned int verticalSpacing() const;

  bool hovered() const;
  void setHovered(bool hovered);

  unsigned int nOutPorts() const;
  unsigned int nInPorts() const;

public:
  QRectF boundingRect() const;

  /// Updates size unconditionally
  QSize recalculateSize() const;

  /// Updates size if the QFontMetrics is changed
  QSize recalculateSize(QFont const & font) const;

  // TODO removed default QTransform()
  QPointF portScenePosition(PortIndex index,
                            PortType portType,
                            QTransform const & t = QTransform()) const;

  PortIndex checkHitScenePoint(PortType portType,
                               QPointF point,
                               QTransform const & t = QTransform()) const;

  QRect resizeRect() const;

  /// Returns the position of a widget on the Node surface
  QPointF widgetPosition() const;

  /// Returns the maximum height a widget can be without causing the node to grow.
  int equivalentWidgetHeight() const;

  unsigned int validationHeight() const;

  unsigned int validationWidth() const;

  static
  QPointF calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node * targetNode,
                                                PortIndex sourcePortIndex, PortType sourcePort, Node * sourceNode,
                                                Node & newNode);
private:

  unsigned int captionHeight() const;

  unsigned int captionWidth() const;

  unsigned int portWidth(PortType portType) const;

private:

  NodeId const _nodeId;
  GraphModel const & graphModel;

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable QFontMetrics _fontMetrics;
  mutable QFontMetrics _boldFontMetrics;

  unsigned int _entryWidth;
  mutable unsigned int _defaultInPortWidth;
  mutable unsigned int _defaultOutPortWidth;
  mutable unsigned int _entryHeight;
  unsigned int _verticalSpacing;

  bool _hovered;

  unsigned int _nSources;
  unsigned int _nSinks;

  //QPointF _draggingPos;

  std::unique_ptr<NodeDataModel> const & _dataModel;

};
}
