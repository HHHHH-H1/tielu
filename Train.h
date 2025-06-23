#ifndef TRAIN_H
#define TRAIN_H

#include "Route.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <vector>


// 时间点结构
struct TimePoint {
  int hour;
  int minute;

  TimePoint(int h = 0, int m = 0) : hour(h), minute(m) {}
  std::string toString() const;
  int toMinutes() const { return hour * 60 + minute; }
};

// 时刻表条目
struct ScheduleEntry {
  std::string stationId;
  std::string stationName;
  TimePoint arrivalTime;
  TimePoint departureTime;
  int stopDuration; // 停车时长（分钟）

  ScheduleEntry(const std::string &id, const std::string &name,
                TimePoint arrival, TimePoint departure, int duration = 2)
      : stationId(id), stationName(name), arrivalTime(arrival),
        departureTime(departure), stopDuration(duration) {}
};

class Train {
private:
  std::string trainId;                 // 列车号
  std::string trainType;               // 列车类型（G、D、C、K等）
  std::shared_ptr<Route> route;        // 运行线路
  std::vector<ScheduleEntry> schedule; // 时刻表
  int totalCapacity;                   // 总载客量
  int currentPassengers;               // 当前载客数
  double currentSpeed;                 // 当前速度
  std::string currentStatus;           // 当前状态（运行中、停靠、检修等）
  bool isInService;                    // 是否在服务中

public:
  // 构造函数
  Train();
  Train(const std::string &id, const std::string &type,
        std::shared_ptr<Route> trainRoute, int capacity = 1200);

  // 析构函数
  ~Train();

  // Getter方法
  std::string getTrainId() const { return trainId; }
  std::string getTrainType() const { return trainType; }
  std::shared_ptr<Route> getRoute() const { return route; }
  std::vector<ScheduleEntry> getSchedule() const { return schedule; }
  int getTotalCapacity() const { return totalCapacity; }
  int getCurrentPassengers() const { return currentPassengers; }
  double getCurrentSpeed() const { return currentSpeed; }
  std::string getCurrentStatus() const { return currentStatus; }
  bool getIsInService() const { return isInService; }

  // Setter方法
  void setTrainId(const std::string &id) { trainId = id; }
  void setTrainType(const std::string &type) { trainType = type; }
  void setRoute(std::shared_ptr<Route> trainRoute) { route = trainRoute; }
  void setTotalCapacity(int capacity) { totalCapacity = capacity; }
  void setCurrentPassengers(int passengers) { currentPassengers = passengers; }
  void setCurrentSpeed(double speed) { currentSpeed = speed; }
  void setCurrentStatus(const std::string &status) { currentStatus = status; }
  void setIsInService(bool inService) { isInService = inService; }

  // 功能方法
  void addScheduleEntry(const ScheduleEntry &entry);
  void removeScheduleEntry(const std::string &stationId);
  ScheduleEntry *findScheduleEntry(const std::string &stationId);
  double getLoadFactor() const; // 载客率
  std::string getScheduleString() const;
  TimePoint getArrivalTime(const std::string &stationId) const;
  TimePoint getDepartureTime(const std::string &stationId) const;
  int calculateTravelTime(const std::string &fromStationId,
                          const std::string &toStationId) const;
  std::string toString() const;
  bool operator==(const Train &other) const;
};

#endif // TRAIN_H