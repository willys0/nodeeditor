#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class FlowItemEntry;
class GraphicsScene;
class GraphModel;
class Node;
class NodeGeometry;
class NodeGraphicsObject;
class NodeState;

/// @ Lightweight class incapsulating paint code.
class NodePainter
{
public:

  NodePainter();

public:

  static
  void paint(QPainter * painter,
             NodeId node,
             GraphicsScene const & scene);

  static
  void drawNodeRect(QPainter * painter,
                    NodeGeometry const & geom,
                    GraphModel const &   graphModel,
                    NodeGraphicsObject const & graphicsObject);

  static
  void drawModelName(QPainter * painter,
                     NodeGeometry const &  geom,
                     NodeState const &     state,
                     NodeDataModel const * model);

  static
  void drawEntryLabels(QiiiiiPainter * painter,
                       NodeGeometry const &  geom,
                       NodeState const &     state,
                       NodeDataModel const * model);

  static
  void drawConnectionPoints(QPainter * painter,
                            NodeGeometry const &  geom,
                            NodeState const &     state,
                            NodeDataModel const * model,
                            GraphicsScene const &     scene);

  static
  void drawFilledConnectionPoints(QPainter * painter,
                                  NodeGeometry const &  geom,
                                  NodeState const &     state,
                                  NodeDataModel const * model);

  static
  void drawResizeRect(QPainter * painter,
                      NodeGeometry const & geom,
                      NodeDataModel const * model);

  static
  void drawValidationRect(QPainter * painter,
                          NodeGeometry const &  geom,
                          NodeDataModel const * model,
                          NodeGraphicsObject const & graphicsObject);
};
}
