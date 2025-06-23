#ifndef ROUTE_H
#define ROUTE_H

#include "Station.h"
#include <memory>
#include <string>
#include <vector>


class Route {
private:
  std::string routeId;   // 线路ID
  std::string routeName; // 线路名称
  std::string routeType; // 线路类型（高铁、动车、普通列车）
  std::vector<std::shared_ptr<Station>> stations; // 线路上的站点列表
  double totalDistance;                           // 线路总长度（公里）
  int maxSpeed;                                   // 最高运行速度（km/h）
  std::string startCity;                          // 起始城市
  std::string endCity;                            // 终点城市
  bool isOperational;                             // 是否正在运营

public:
  // 构造函数
  Route();
  Route(const std::string &id, const std::string &name, const std::string &type,
        double distance = 0.0, int speed = 250, bool operational = true);

  // 析构函数
  ~Route();

  // Getter方法
  std::string getRouteId() const { return routeId; }
  std::string getRouteName() const { return routeName; }
  std::string getRouteType() const { return routeType; }
  std::vector<std::shared_ptr<Station>> getStations() const { return stations; }
  double getTotalDistance() const { return totalDistance; }
  int getMaxSpeed() const { return maxSpeed; }
  std::string getStartCity() const { return startCity; }
  std::string getEndCity() const { return endCity; }
  bool getIsOperational() const { return isOperational; }

  // Setter方法
  void setRouteId(const std::string &id) { routeId = id; }
  void setRouteName(const std::string &name) { routeName = name; }
  void setRouteType(const std::string &type) { routeType = type; }
  void setTotalDistance(double distance) { totalDistance = distance; }
  void setMaxSpeed(int speed) { maxSpeed = speed; }
  void setStartCity(const std::string &city) { startCity = city; }
  void setEndCity(const std::string &city) { endCity = city; }
  void setIsOperational(bool operational) { isOperational = operational; }

  // 功能方法
  void addStation(std::shared_ptr<Station> station);
  void removeStation(const std::string &stationId);
  std::shared_ptr<Station> findStation(const std::string &stationId) const;
  int getStationCount() const;
  std::string getStationsInOrder() const;
  double calculateDistance(const std::string &fromStationId,
                           const std::string &toStationId) const;
  std::string toString() const;
  bool operator==(const Route &other) const;
};

#endif // ROUTE_H