#include "NodeGeometry.hpp"

#include <iostream>
#include <cmath>

#include "Definitions.hpp"
#include "GraphModel.hpp"

//#include "NodeDataModel.hpp"
//#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeGeometry;
using QtNodes::NodeDataModel;
using QtNodes::PortIndex;
using QtNodes::PortType;


NodeGeometry::
NodeGeometry(NodeId nodeId, GraphModel const & graphModel)
  : _nodeId(nodeId)
  , _graphModel(graphModel)
  , _defaultInPortWidth(70)
  , _defaultOutPortWidth(70)
  , _entryHeight(20)
  , _verticalSpacing(20)
  , _fontMetrics(QFont())
  , _boldFontMetrics(QFont())
{
  QFont f; f.setBold(true);

  _boldFontMetrics = QFontMetrics(f);

  _entryHeight = _fontMetrics.height();
}


QSize
NodeGeometry::
size() const
{
  //return _graphModel.nodeData(_nodeId, NodeRole::Size);

  return recalculateSize();
}


//void
//NodeGeometry::
//setSize(QSize s)
//{
  //return _graphModel.setNodeData(_nodeId, NodeRole::Size, s);
//}


unsigned int
NodeGeometry::
entryHeight() const
{
  return _entryHeight;
}


unsigned int
NodeGeometry::
verticalSpacing() const
{
  return _verticalSpacing;
}


bool
NodeGeometry::
hovered() const
{
  return _graphModel.nodeData(_nodeId, NodeRole::Hovered);
}


void
NodeGeometry::
setHovered(bool hovered)
{
  return _graphModel.setNodeData(_nodeId, NodeRole::Size, hovered);
}


unsigned int
NodeGeometry::
nInPorts() const
{
  return _graphModel.nodeData(_nodeId, NodeRole::NumberOfInPorts);

}


unsigned int
NodeGeometry::
nOutPorts() const
{
  return _graphModel.nodeData(_nodeId, NodeRole::NumberOfOutPorts);
}


//QRectF
//NodeGeometry::
//entryBoundingRect() const
//{
//double const addon = 0.0;

//return QRectF(0 - addon,
//0 - addon,
//_entryWidth + 2 * addon,
//_entryHeight + 2 * addon);
//}


QRectF
NodeGeometry::
boundingRect() const
{
  auto const & style = _graphModel.nodeData(_nodeId, NodeRole::Style);
  auto const & nodeStyle = StyleCollection::nodeStyle();

  double addon = 4 * nodeStyle.ConnectionPointDiameter;

  QSize size = recalculateSize();

  return QRectF(0 - addon,
                0 - addon,
                size.width() + 2 * addon,
                size.height() + 2 * addon);
}


QSize
NodeGeometry::
recalculateSize() const
{
  unsigned int height = 0;
  {
    unsigned int maxNumOfEntries = std::max(nInPorts(), nOutPorts());
    unsigned int step = _entryHeight + _verticalSpacing;
    height = step * maxNumOfEntries;
  }

  if (auto w = _graphModel.nodeData(_nodeId, NodeRole::Widget))
  {
    height = std::max(_height, static_cast<unsigned int>(w->height()));
  }

  height += captionHeight();

  inPortWidth  = portWidth(PortType::In);
  outPortWidth = portWidth(PortType::Out);

  unsigned int width = inPortWidth + outPortWidth + 2 * _spacing;

  if (auto w = _graphModel.nodeData(_nodeId, NodeRole::Widget);
  {
    width += w->width();
  }

  width = std::max(width, captionWidth());

  return QSize(width, height);
}


QSize
NodeGeometry::
recalculateSize(QFont const & font) const
{
  QFontMetrics fontMetrics(font);
  QFont boldFont = font;

  boldFont.setBold(true);

  QFontMetrics boldFontMetrics(boldFont);

  if (_boldFontMetrics != boldFontMetrics)
  {
    _fontMetrics     = fontMetrics;
    _boldFontMetrics = boldFontMetrics;

  }

  return recalculateSize();
}


QPointF
NodeGeometry::
portScenePosition(PortIndex index,
                  PortType  portType,
                  QTransform const & t) const
{
  auto const & nodeStyle = StyleCollection::nodeStyle();

  unsigned int step = _entryHeight + _spacing;

  QPointF result;

  double totalHeight = 0.0;

  totalHeight += captionHeight();

  totalHeight += step * index;

  // TODO: why?
  totalHeight += step / 2.0;

  switch (portType)
  {
    case PortType::Out:
    {
      double x = _width + nodeStyle.ConnectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case PortType::In:
    {
      double x = 0.0 - nodeStyle.ConnectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    default:
      break;
  }

  return t.map(result);
}


PortIndex
NodeGeometry::
checkHitScenePoint(PortType portType,
                   QPointF const scenePoint,
                   QTransform const & sceneTransform) const
{
  auto const & nodeStyle = StyleCollection::nodeStyle();

  PortIndex result = INVALID;

  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

  unsigned int const nItems = _dataModel->nPorts(portType);

  for (unsigned int i = 0; i < nItems; ++i)
  {
    auto pp = portScenePosition(i, portType, sceneTransform);

    QPointF p     = pp - scenePoint;
    auto distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
    {
      result = PortIndex(i);
      break;
    }
  }

  return result;
}


QRect
NodeGeometry::
resizeRect() const
{
  unsigned int rectSize = 7;

  return QRect(_width - rectSize,
               _height - rectSize,
               rectSize,
               rectSize);
}


QPointF
NodeGeometry::
widgetPosition() const
{
  if (auto w = _dataModel->embeddedWidget())
  {
    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, place it immediately after the caption.
      return QPointF(_spacing + portWidth(PortType::In), captionHeight());
    }
    else
    {
      if (_dataModel->validationState() != NodeValidationState::Valid)
      {
        return QPointF(_spacing + portWidth(PortType::In),
                       (captionHeight() + _height - validationHeight() - _spacing - w->height()) / 2.0);
      }

      return QPointF(_spacing + portWidth(PortType::In),
                     (captionHeight() + _height - w->height()) / 2.0);
    }
  }
  return QPointF();
}


int
NodeGeometry::
equivalentWidgetHeight() const
{
  if (_dataModel->validationState() != NodeValidationState::Valid)
  {
    return height() - captionHeight() + validationHeight();
  }

  return height() - captionHeight();
}


unsigned int
NodeGeometry::
captionHeight() const
{
  if (!_graphModel.nodeData(_nodeId, NodeRole::CaptionVisible))
    return 0;

  QString name = _graphModel.nodeData(_nodeId, NodeRole::Caption);

  return _boldFontMetrics.boundingRect(name).height();
}


unsigned int
NodeGeometry::
captionWidth() const
{
  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _boldFontMetrics.boundingRect(name).width();
}


unsigned int
NodeGeometry::
validationHeight() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).height();
}


unsigned int
NodeGeometry::
validationWidth() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).width();
}


//QPointF
//NodeGeometry::
//calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node * targetNode,
                                      //PortIndex sourcePortIndex, PortType sourcePort, Node * sourceNode,
                                      //Node & newNode)
//{
  ////Calculating the nodes position in the scene. It'll be positioned half way
  ////between the two ports that it "connects".  The first line calculates the
  ////halfway point between the ports (node position + port position on the node
  ////for both nodes averaged).  The second line offsets this coordinate with the
  ////size of the new node, so that the new nodes center falls on the originally
  ////calculated coordinate, instead of it's upper left corner.
  //auto converterNodePos = (sourceNode->nodeGraphicsObject().pos() + sourceNode->nodeGeometry().portScenePosition(sourcePortIndex, sourcePort) +
                           //targetNode->nodeGraphicsObject().pos() + targetNode->nodeGeometry().portScenePosition(targetPortIndex, targetPort)) / 2.0f;
  //converterNodePos.setX(converterNodePos.x() - newNode.nodeGeometry().width() / 2.0f);
  //converterNodePos.setY(converterNodePos.y() - newNode.nodeGeometry().height() / 2.0f);
  //return converterNodePos;
//}


unsigned int
NodeGeometry::
portWidth(PortType portType) const
{
  unsigned width = 0;

  for (auto i = 0ul; i < _dataModel->nPorts(portType); ++i)
  {
    QString name;

    if (_dataModel->portCaptionVisible(portType, i))
    {
      name = _dataModel->portCaption(portType, i);
    }
    else
    {
      name = _dataModel->dataType(portType, i).name;
    }

    width = std::max(unsigned(_fontMetrics.width(name)),
                     width);
  }

  return width;
}
