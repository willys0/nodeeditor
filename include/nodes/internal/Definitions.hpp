#pragma once

#include <limits>

#include <QtCore/QMetaObject>

namespace QtNodes
{
Q_NAMESPACE


enum class NodeRole
{
  Position         = 0,
  CaptionVisible   = 1,
  Caption          = 2,
  Style            = 3,
  Hovered          = 4,
  NumberOfInPorts  = 5,
  NumberOfOutPorts = 6,
};
Q_ENUM_NS(NodeRole)


enum class NodeFlags
{
  Resizable = 0x0,
};
//Q_DECLARE_FLAGS(NodeFlags, NodeFlags)
Q_FLAG_NS(NodeFlags)

//Q_DECLARE_OPERATORS_FOR_FLAGS(NodeFlags)


enum class PortRole
{
  Data             = 0,
  DataType         = 1,
  ConnectionPolicy = 2,
  Caption          = 3,
};
Q_ENUM_NS(PortRole)


enum class ConnectionPolicy
{
  One,
  Many,
};
Q_ENUM_NS(ConnectionPolicy)


enum class PortType
{
  None,
  In,
  Out
};
Q_ENUM_NS(PortType)



inline
PortType
oppositePort(PortType port)
{
  PortType result = PortType::None;

  switch (port)
  {
    case PortType::In:
      result = PortType::Out;
      break;

    case PortType::Out:
      result = PortType::In;
      break;

    case PortType::None:
      result = PortType::None;
      break;

    default:
      break;
  }
  return result;
}


using NodeId = std::size_t;

static constexpr NodeId InvalidNodeId =
  std::numeric_limits<NodeId>::max();


using PortIndex = std::size_t;

static constexpr PortIndex InvalidPortIndex =
  std::numeric_limits<PortIndex>::max();


inline
bool
isPortValid(PortIndex index)
{ return index != InvalidPortIndex; }

inline
bool
isPortTypeValid(PortType portType)
{ return portType != PortType::None; }

}
