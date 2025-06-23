#include "Station.h"
#include <sstream>

// 默认构造函数
Station::Station()
    : stationId(""), stationName(""), cityName(""), longitude(0.0),
      latitude(0.0), stationType("中间站"), platformCount(2),
      isTransferStation(false) {}

// 带参数的构造函数
Station::Station(const std::string &id, const std::string &name,
                 const std::string &city, double lng, double lat,
                 const std::string &type, int platforms, bool transfer)
    : stationId(id), stationName(name), cityName(city), longitude(lng),
      latitude(lat), stationType(type), platformCount(platforms),
      isTransferStation(transfer) {}

// 析构函数
Station::~Station() {}

// 转换为字符串
std::string Station::toString() const {
  std::ostringstream oss;
  oss << "站点[" << stationId << "]: " << stationName << " (" << cityName
      << ") - 坐标: (" << longitude << ", " << latitude
      << ") - 类型: " << stationType;
  return oss.str();
}

// 比较操作符
bool Station::operator==(const Station &other) const {
  return stationId == other.stationId;
}