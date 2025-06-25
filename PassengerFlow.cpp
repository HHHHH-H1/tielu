#include "PassengerFlow.h"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

// Date类方法实现
std::string Date::toString() const {
  std::ostringstream oss;
  oss << year << "-" << std::setfill('0') << std::setw(2) << month << "-"
      << std::setw(2) << day;
  return oss.str();
}

bool Date::operator<(const Date &other) const {
  if (year != other.year)
    return year < other.year;
  if (month != other.month)
    return month < other.month;
  return day < other.day;
}

bool Date::operator==(const Date &other) const {
  return year == other.year && month == other.month && day == other.day;
}

// FlowRecord类实现
FlowRecord::FlowRecord()
    : recordId(""), stationId(""), stationName(""), date(Date()), hour(0),
      boardingCount(0), alightingCount(0), trainId(""), direction("") {}

FlowRecord::FlowRecord(const std::string &id, const std::string &stId,
                       const std::string &stName, const Date &d, int h,
                       int boarding, int alighting, const std::string &tId,
                       const std::string &dir)
    : recordId(id), stationId(stId), stationName(stName), date(d), hour(h),
      boardingCount(boarding), alightingCount(alighting), trainId(tId),
      direction(dir) {}

FlowRecord::~FlowRecord() {}

std::string FlowRecord::toString() const {
  std::ostringstream oss;
  oss << "记录[" << recordId << "]: " << stationName << " " << date.toString()
      << " " << hour << ":00"
      << " - 上车: " << boardingCount << ", 下车: " << alightingCount
      << ", 总流量: " << getTotalFlow();
  if (!trainId.empty()) {
    oss << ", 列车: " << trainId;
  }
  if (!direction.empty()) {
    oss << ", 方向: " << direction;
  }
  return oss.str();
}

// PassengerFlow类实现
PassengerFlow::PassengerFlow() {}

PassengerFlow::~PassengerFlow() { records.clear(); }

void PassengerFlow::addRecord(const FlowRecord &record) {
  records.push_back(record);
  updateStatistics();
}

void PassengerFlow::removeRecord(const std::string &recordId) {
  records.erase(std::remove_if(records.begin(), records.end(),
                               [&recordId](const FlowRecord &record) {
                                 return record.getRecordId() == recordId;
                               }),
                records.end());
  updateStatistics();
}

FlowRecord *PassengerFlow::findRecord(const std::string &recordId) {
  auto it = std::find_if(records.begin(), records.end(),
                         [&recordId](const FlowRecord &record) {
                           return record.getRecordId() == recordId;
                         });

  return (it != records.end()) ? &(*it) : nullptr;
}

std::vector<FlowRecord>
PassengerFlow::getRecordsByStation(const std::string &stationId) const {
  std::vector<FlowRecord> result;
  std::copy_if(records.begin(), records.end(), std::back_inserter(result),
               [&stationId](const FlowRecord &record) {
                 return record.getStationId() == stationId;
               });
  return result;
}

std::vector<FlowRecord>
PassengerFlow::getRecordsByDate(const Date &date) const {
  std::vector<FlowRecord> result;
  std::copy_if(
      records.begin(), records.end(), std::back_inserter(result),
      [&date](const FlowRecord &record) { return record.getDate() == date; });
  return result;
}

std::vector<FlowRecord>
PassengerFlow::getRecordsByDateRange(const Date &startDate,
                                     const Date &endDate) const {
  std::vector<FlowRecord> result;
  std::copy_if(records.begin(), records.end(), std::back_inserter(result),
               [&startDate, &endDate](const FlowRecord &record) {
                 const Date &recordDate = record.getDate();
                 return !(recordDate < startDate) && recordDate < endDate ||
                        recordDate == endDate;
               });
  return result;
}

int PassengerFlow::getStationTotalFlow(const std::string &stationId) const {
  int total = 0;
  for (const auto &record : records) {
    if (record.getStationId() == stationId) {
      total += record.getTotalFlow();
    }
  }
  return total;
}

int PassengerFlow::getStationDailyFlow(const std::string &stationId,
                                       const Date &date) const {
  int total = 0;
  for (const auto &record : records) {
    if (record.getStationId() == stationId && record.getDate() == date) {
      total += record.getTotalFlow();
    }
  }
  return total;
}

std::vector<int>
PassengerFlow::getStationHourlyFlow(const std::string &stationId,
                                    const Date &date) const {
  std::vector<int> hourlyData(24, 0);
  for (const auto &record : records) {
    if (record.getStationId() == stationId && record.getDate() == date) {
      hourlyData[record.getHour()] += record.getTotalFlow();
    }
  }
  return hourlyData;
}

std::map<std::string, int> PassengerFlow::getAllStationsFlow() const {
  std::map<std::string, int> stationFlow;
  for (const auto &record : records) {
    // 使用站点名称而不是站点ID作为键
    std::string stationKey = record.getStationName();
    if (stationKey.empty()) {
      stationKey = record.getStationId(); // 如果没有站点名称，使用ID作为备用
    }
    stationFlow[stationKey] += record.getTotalFlow();
  }
  return stationFlow;
}

int PassengerFlow::getChengduToChongqingFlow(const Date &date) const {
  int total = 0;
  for (const auto &record : records) {
    if (record.getDate() == date && record.getDirection() == "川->渝") {
      total += record.getTotalFlow();
    }
  }
  return total;
}

int PassengerFlow::getChongqingToChengduFlow(const Date &date) const {
  int total = 0;
  for (const auto &record : records) {
    if (record.getDate() == date && record.getDirection() == "渝->川") {
      total += record.getTotalFlow();
    }
  }
  return total;
}

double PassengerFlow::getFlowRatio() const {
  int chengduToChongqing = 0;
  int chongqingToChengdu = 0;

  for (const auto &record : records) {
    if (record.getDirection() == "川->渝") {
      chengduToChongqing += record.getTotalFlow();
    } else if (record.getDirection() == "渝->川") {
      chongqingToChengdu += record.getTotalFlow();
    }
  }

  if (chongqingToChengdu == 0)
    return 0.0;
  return static_cast<double>(chengduToChongqing) / chongqingToChengdu;
}

double PassengerFlow::calculateLoadFactor(const std::string &trainId,
                                          const Date &date) const {
  // 这里需要结合列车容量信息，暂时返回一个模拟值
  int totalPassengers = 0;
  int recordCount = 0;

  for (const auto &record : records) {
    if (record.getTrainId() == trainId && record.getDate() == date) {
      totalPassengers += record.getBoardingCount();
      recordCount++;
    }
  }

  // 假设列车容量为1200人
  int trainCapacity = 1200;
  if (recordCount > 0) {
    return (static_cast<double>(totalPassengers) / recordCount /
            trainCapacity) *
           100.0;
  }
  return 0.0;
}

std::map<std::string, double>
PassengerFlow::getAllTrainsLoadFactor(const Date &date) const {
  std::map<std::string, double> loadFactors;
  std::map<std::string, int> trainPassengers;
  std::map<std::string, int> trainRecords;

  for (const auto &record : records) {
    if (record.getDate() == date && !record.getTrainId().empty()) {
      trainPassengers[record.getTrainId()] += record.getBoardingCount();
      trainRecords[record.getTrainId()]++;
    }
  }

  for (const auto &pair : trainPassengers) {
    const std::string &trainId = pair.first;
    int totalPassengers = pair.second;
    int recordCount = trainRecords[trainId];

    int trainCapacity = 1200; // 假设容量
    if (recordCount > 0) {
      loadFactors[trainId] =
          (static_cast<double>(totalPassengers) / recordCount / trainCapacity) *
          100.0;
    }
  }

  return loadFactors;
}

// 简化的预测功能（基于历史平均）
std::vector<int> PassengerFlow::predictFlow(const std::string &stationId,
                                            int days) const {
  std::vector<int> prediction(days, 0);

  // 计算历史平均流量
  int totalFlow = getStationTotalFlow(stationId);
  int recordDays = 0;

  std::map<Date, bool> uniqueDates;
  for (const auto &record : records) {
    if (record.getStationId() == stationId) {
      uniqueDates[record.getDate()] = true;
    }
  }
  recordDays = static_cast<int>(uniqueDates.size());

  int avgDailyFlow = (recordDays > 0) ? totalFlow / recordDays : 0;

  // 简单预测：使用历史平均值
  for (int i = 0; i < days; ++i) {
    prediction[i] = avgDailyFlow;
  }

  return prediction;
}

std::vector<int>
PassengerFlow::predictDirectionalFlow(const std::string &direction,
                                      int days) const {
  std::vector<int> prediction(days, 0);

  int totalFlow = 0;
  int recordDays = 0;
  std::map<Date, bool> uniqueDates;

  for (const auto &record : records) {
    if (record.getDirection() == direction) {
      totalFlow += record.getTotalFlow();
      uniqueDates[record.getDate()] = true;
    }
  }
  recordDays = static_cast<int>(uniqueDates.size());

  int avgDailyFlow = (recordDays > 0) ? totalFlow / recordDays : 0;

  for (int i = 0; i < days; ++i) {
    prediction[i] = avgDailyFlow;
  }

  return prediction;
}

std::string PassengerFlow::generateFlowReport(const Date &date) const {
  std::ostringstream oss;
  oss << "=== " << date.toString() << " 客流报告 ===\n\n";

  auto dayRecords = getRecordsByDate(date);
  int totalFlow = 0;

  std::map<std::string, int> stationFlow;
  for (const auto &record : dayRecords) {
    totalFlow += record.getTotalFlow();
    stationFlow[record.getStationName()] += record.getTotalFlow();
  }

  oss << "总客流量: " << totalFlow << " 人次\n";
  oss << "川->渝方向: " << getChengduToChongqingFlow(date) << " 人次\n";
  oss << "渝->川方向: " << getChongqingToChengduFlow(date) << " 人次\n\n";

  oss << "各站点客流量:\n";
  for (const auto &pair : stationFlow) {
    oss << pair.first << ": " << pair.second << " 人次\n";
  }

  return oss.str();
}

std::string PassengerFlow::generateStationRanking() const {
  auto stationFlow = getAllStationsFlow();

  // 转换为vector以便排序
  std::vector<std::pair<std::string, int>> ranking;
  for (const auto &pair : stationFlow) {
    ranking.push_back(pair);
  }

  // 按流量降序排序
  std::sort(
      ranking.begin(), ranking.end(),
      [](const std::pair<std::string, int> &a,
         const std::pair<std::string, int> &b) { return a.second > b.second; });

  std::ostringstream oss;
  oss << "=== 站点客流量排行 ===\n\n";

  for (size_t i = 0; i < ranking.size(); ++i) {
    oss << (i + 1) << ". " << ranking[i].first << ": " << ranking[i].second
        << " 人次\n";
  }

  return oss.str();
}

void PassengerFlow::updateStatistics() {
  stationDailyFlow.clear();
  hourlyFlow.clear();

  for (const auto &record : records) {
    std::string key = record.getStationId() + "_" + record.getDate().toString();
    stationDailyFlow[key] += record.getTotalFlow();

    std::string hourKey =
        record.getStationId() + "_" + record.getDate().toString();
    if (hourlyFlow[hourKey].size() < 24) {
      hourlyFlow[hourKey].resize(24, 0);
    }
    hourlyFlow[hourKey][record.getHour()] += record.getTotalFlow();
  }
}