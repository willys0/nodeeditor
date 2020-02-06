#pragma once

#include <numeric_limits>


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


enum class PortRole
{
  Data             = 0,
  DataType         = 1,
  ConnectionPolicy = 2,
  Caption          = 3,
} :
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


using NodeId = std::size_t;

static constexpr InvalidNodeId =
  std::numeric_limits<NodeId>::max();


using PortIndex = std::size_t;

static constexpr InvalidPortIndex =
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
