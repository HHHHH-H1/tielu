#include "Train.h"
#include <algorithm>
#include <iomanip>
#include <sstream>


// TimePoint类方法实现
std::string TimePoint::toString() const {
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << hour << ":" << std::setw(2)
      << minute;
  return oss.str();
}

// 默认构造函数
Train::Train()
    : trainId(""), trainType("G"), route(nullptr), totalCapacity(1200),
      currentPassengers(0), currentSpeed(0.0), currentStatus("停靠"),
      isInService(true) {}

// 带参数的构造函数
Train::Train(const std::string &id, const std::string &type,
             std::shared_ptr<Route> trainRoute, int capacity)
    : trainId(id), trainType(type), route(trainRoute), totalCapacity(capacity),
      currentPassengers(0), currentSpeed(0.0), currentStatus("停靠"),
      isInService(true) {}

// 析构函数
Train::~Train() { schedule.clear(); }

// 添加时刻表条目
void Train::addScheduleEntry(const ScheduleEntry &entry) {
  schedule.push_back(entry);
}

// 移除时刻表条目
void Train::removeScheduleEntry(const std::string &stationId) {
  schedule.erase(std::remove_if(schedule.begin(), schedule.end(),
                                [&stationId](const ScheduleEntry &entry) {
                                  return entry.stationId == stationId;
                                }),
                 schedule.end());
}

// 查找时刻表条目
ScheduleEntry *Train::findScheduleEntry(const std::string &stationId) {
  auto it = std::find_if(schedule.begin(), schedule.end(),
                         [&stationId](const ScheduleEntry &entry) {
                           return entry.stationId == stationId;
                         });

  return (it != schedule.end()) ? &(*it) : nullptr;
}

// 获取载客率
double Train::getLoadFactor() const {
  if (totalCapacity == 0)
    return 0.0;
  return (static_cast<double>(currentPassengers) / totalCapacity) * 100.0;
}

// 获取时刻表字符串
std::string Train::getScheduleString() const {
  std::ostringstream oss;
  oss << "列车 " << trainId << " 时刻表:\n";
  oss << "站点名称\t\t到达时间\t发车时间\t停车时长\n";
  oss << "------------------------------------------------\n";

  for (const auto &entry : schedule) {
    oss << entry.stationName << "\t\t" << entry.arrivalTime.toString() << "\t\t"
        << entry.departureTime.toString() << "\t\t" << entry.stopDuration
        << "分钟\n";
  }

  return oss.str();
}

// 获取到达时间
TimePoint Train::getArrivalTime(const std::string &stationId) const {
  auto it = std::find_if(schedule.begin(), schedule.end(),
                         [&stationId](const ScheduleEntry &entry) {
                           return entry.stationId == stationId;
                         });

  return (it != schedule.end()) ? it->arrivalTime : TimePoint();
}

// 获取发车时间
TimePoint Train::getDepartureTime(const std::string &stationId) const {
  auto it = std::find_if(schedule.begin(), schedule.end(),
                         [&stationId](const ScheduleEntry &entry) {
                           return entry.stationId == stationId;
                         });

  return (it != schedule.end()) ? it->departureTime : TimePoint();
}

// 计算行程时间
int Train::calculateTravelTime(const std::string &fromStationId,
                               const std::string &toStationId) const {
  TimePoint fromTime = getDepartureTime(fromStationId);
  TimePoint toTime = getArrivalTime(toStationId);

  return toTime.toMinutes() - fromTime.toMinutes();
}

// 转换为字符串
std::string Train::toString() const {
  std::ostringstream oss;
  oss << "列车[" << trainId << "]: " << trainType << "次"
      << " - 载客率: " << std::fixed << std::setprecision(1) << getLoadFactor()
      << "%"
      << " (" << currentPassengers << "/" << totalCapacity << ")"
      << " - 状态: " << currentStatus;

  if (route) {
    oss << " - 线路: " << route->getRouteName();
  }

  return oss.str();
}

// 比较操作符
bool Train::operator==(const Train &other) const {
  return trainId == other.trainId;
}