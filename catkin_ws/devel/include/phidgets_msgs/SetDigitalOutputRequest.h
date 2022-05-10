// Generated by gencpp from file phidgets_msgs/SetDigitalOutputRequest.msg
// DO NOT EDIT!


#ifndef PHIDGETS_MSGS_MESSAGE_SETDIGITALOUTPUTREQUEST_H
#define PHIDGETS_MSGS_MESSAGE_SETDIGITALOUTPUTREQUEST_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace phidgets_msgs
{
template <class ContainerAllocator>
struct SetDigitalOutputRequest_
{
  typedef SetDigitalOutputRequest_<ContainerAllocator> Type;

  SetDigitalOutputRequest_()
    : index(0)
    , state(false)  {
    }
  SetDigitalOutputRequest_(const ContainerAllocator& _alloc)
    : index(0)
    , state(false)  {
  (void)_alloc;
    }



   typedef uint16_t _index_type;
  _index_type index;

   typedef uint8_t _state_type;
  _state_type state;





  typedef boost::shared_ptr< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> const> ConstPtr;

}; // struct SetDigitalOutputRequest_

typedef ::phidgets_msgs::SetDigitalOutputRequest_<std::allocator<void> > SetDigitalOutputRequest;

typedef boost::shared_ptr< ::phidgets_msgs::SetDigitalOutputRequest > SetDigitalOutputRequestPtr;
typedef boost::shared_ptr< ::phidgets_msgs::SetDigitalOutputRequest const> SetDigitalOutputRequestConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >::stream(s, "", v);
return s;
}


template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator==(const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator1> & lhs, const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator2> & rhs)
{
  return lhs.index == rhs.index &&
    lhs.state == rhs.state;
}

template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator!=(const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator1> & lhs, const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator2> & rhs)
{
  return !(lhs == rhs);
}


} // namespace phidgets_msgs

namespace ros
{
namespace message_traits
{





template <class ContainerAllocator>
struct IsMessage< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "87b461c46634beb857c7651b37066682";
  }

  static const char* value(const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x87b461c46634beb8ULL;
  static const uint64_t static_value2 = 0x57c7651b37066682ULL;
};

template<class ContainerAllocator>
struct DataType< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "phidgets_msgs/SetDigitalOutputRequest";
  }

  static const char* value(const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "# Sets the state of a digital output.\n"
"\n"
"uint16 index   # index of the output to be set\n"
"bool state\n"
;
  }

  static const char* value(const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.index);
      stream.next(m.state);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct SetDigitalOutputRequest_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::phidgets_msgs::SetDigitalOutputRequest_<ContainerAllocator>& v)
  {
    s << indent << "index: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.index);
    s << indent << "state: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.state);
  }
};

} // namespace message_operations
} // namespace ros

#endif // PHIDGETS_MSGS_MESSAGE_SETDIGITALOUTPUTREQUEST_H