#pragma once

#include <QtCore/QPointF>
#include <QtGui/QTransform>

namespace QtNodes
{

class NodeGraphicsScene;


NodeId
locateNodeAt(QPointF scenePoint,
             NodeGraphicsScene &scene,
             QTransform const & viewTransform);


}
