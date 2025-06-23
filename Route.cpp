#include "Route.h"
#include <algorithm>
#include <cmath>
#include <sstream>


// 默认构造函数
Route::Route()
    : routeId(""), routeName(""), routeType("高铁"), totalDistance(0.0),
      maxSpeed(250), startCity(""), endCity(""), isOperational(true) {}

// 带参数的构造函数
Route::Route(const std::string &id, const std::string &name,
             const std::string &type, double distance, int speed,
             bool operational)
    : routeId(id), routeName(name), routeType(type), totalDistance(distance),
      maxSpeed(speed), startCity(""), endCity(""), isOperational(operational) {}

// 析构函数
Route::~Route() { stations.clear(); }

// 添加站点
void Route::addStation(std::shared_ptr<Station> station) {
  if (station) {
    stations.push_back(station);

    // 更新起始和终点城市
    if (stations.size() == 1) {
      startCity = station->getCityName();
    }
    endCity = station->getCityName();
  }
}

// 移除站点
void Route::removeStation(const std::string &stationId) {
  stations.erase(
      std::remove_if(stations.begin(), stations.end(),
                     [&stationId](const std::shared_ptr<Station> &station) {
                       return station && station->getStationId() == stationId;
                     }),
      stations.end());
}

// 查找站点
std::shared_ptr<Station>
Route::findStation(const std::string &stationId) const {
  auto it =
      std::find_if(stations.begin(), stations.end(),
                   [&stationId](const std::shared_ptr<Station> &station) {
                     return station && station->getStationId() == stationId;
                   });

  return (it != stations.end()) ? *it : nullptr;
}

// 获取站点数量
int Route::getStationCount() const { return static_cast<int>(stations.size()); }

// 按顺序获取站点信息
std::string Route::getStationsInOrder() const {
  std::ostringstream oss;
  oss << "线路 " << routeName << " 站点顺序:\n";

  for (size_t i = 0; i < stations.size(); ++i) {
    if (stations[i]) {
      oss << (i + 1) << ". " << stations[i]->getStationName() << " ("
          << stations[i]->getCityName() << ")\n";
    }
  }

  return oss.str();
}

// 计算两站间距离（简化版，基于坐标计算）
double Route::calculateDistance(const std::string &fromStationId,
                                const std::string &toStationId) const {
  auto fromStation = findStation(fromStationId);
  auto toStation = findStation(toStationId);

  if (!fromStation || !toStation) {
    return 0.0;
  }

  // 使用简化的距离计算公式
  double lat1 = fromStation->getLatitude();
  double lon1 = fromStation->getLongitude();
  double lat2 = toStation->getLatitude();
  double lon2 = toStation->getLongitude();

  // 简化的欧几里得距离
  double dx = (lon2 - lon1) * 111.0; // 经度转公里的近似值
  double dy = (lat2 - lat1) * 111.0; // 纬度转公里的近似值

  return std::sqrt(dx * dx + dy * dy);
}

// 转换为字符串
std::string Route::toString() const {
  std::ostringstream oss;
  oss << "线路[" << routeId << "]: " << routeName << " (" << routeType << ") - "
      << startCity << " → " << endCity << " - 距离: " << totalDistance << "km"
      << " - 最高速度: " << maxSpeed << "km/h"
      << " - 站点数: " << stations.size();
  return oss.str();
}

// 比较操作符
bool Route::operator==(const Route &other) const {
  return routeId == other.routeId;
}