#pragma once

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

namespace QtNodes
{

class ConnectionGeometry;
class ConnectionGraphicsObject;

class ConnectionPainter
{
public:

  static
  void paint(QPainter * painter,
             ConnectionGraphicsObject const & connection);

  static
  QPainterPath getPainterStroke(ConnectionGraphicsObject const & connection);
};

}
