#ifndef STATION_H
#define STATION_H

#include <iostream>
#include <string>
#include <vector>

class Station {
private:
  std::string stationId;   // 站点ID
  std::string stationName; // 站点名称
  std::string cityName;    // 所属城市
  double longitude;        // 经度
  double latitude;         // 纬度
  std::string stationType; // 站点类型（起始站、中间站、终点站）
  int platformCount;       // 站台数量
  bool isTransferStation;  // 是否为换乘站

public:
  // 构造函数
  Station();
  Station(const std::string &id, const std::string &name,
          const std::string &city, double lng, double lat,
          const std::string &type = "中间站", int platforms = 2,
          bool transfer = false);

  // 析构函数
  ~Station();

  // Getter方法
  std::string getStationId() const { return stationId; }
  std::string getStationName() const { return stationName; }
  std::string getCityName() const { return cityName; }
  double getLongitude() const { return longitude; }
  double getLatitude() const { return latitude; }
  std::string getStationType() const { return stationType; }
  int getPlatformCount() const { return platformCount; }
  bool getIsTransferStation() const { return isTransferStation; }

  // Setter方法
  void setStationId(const std::string &id) { stationId = id; }
  void setStationName(const std::string &name) { stationName = name; }
  void setCityName(const std::string &city) { cityName = city; }
  void setLongitude(double lng) { longitude = lng; }
  void setLatitude(double lat) { latitude = lat; }
  void setStationType(const std::string &type) { stationType = type; }
  void setPlatformCount(int count) { platformCount = count; }
  void setIsTransferStation(bool transfer) { isTransferStation = transfer; }

  // 功能方法
  std::string toString() const;
  bool operator==(const Station &other) const;
};

#endif // STATION_H