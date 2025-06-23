#ifndef PASSENGERFLOW_H
#define PASSENGERFLOW_H

#include <ctime>
#include <map>
#include <string>
#include <vector>


// 日期结构
struct Date {
  int year;
  int month;
  int day;

  Date(int y = 2024, int m = 1, int d = 1) : year(y), month(m), day(d) {}
  std::string toString() const;
  bool operator<(const Date &other) const;
  bool operator==(const Date &other) const;
};

// 单个客流记录
class FlowRecord {
private:
  std::string recordId;    // 记录ID
  std::string stationId;   // 站点ID
  std::string stationName; // 站点名称
  Date date;               // 日期
  int hour;                // 小时（0-23）
  int boardingCount;       // 上车人数
  int alightingCount;      // 下车人数
  std::string trainId;     // 列车号
  std::string direction;   // 方向（川->渝, 渝->川）

public:
  // 构造函数
  FlowRecord();
  FlowRecord(const std::string &id, const std::string &stId,
             const std::string &stName, const Date &d, int h, int boarding,
             int alighting, const std::string &tId = "",
             const std::string &dir = "");

  // 析构函数
  ~FlowRecord();

  // Getter方法
  std::string getRecordId() const { return recordId; }
  std::string getStationId() const { return stationId; }
  std::string getStationName() const { return stationName; }
  Date getDate() const { return date; }
  int getHour() const { return hour; }
  int getBoardingCount() const { return boardingCount; }
  int getAlightingCount() const { return alightingCount; }
  std::string getTrainId() const { return trainId; }
  std::string getDirection() const { return direction; }

  // Setter方法
  void setRecordId(const std::string &id) { recordId = id; }
  void setStationId(const std::string &id) { stationId = id; }
  void setStationName(const std::string &name) { stationName = name; }
  void setDate(const Date &d) { date = d; }
  void setHour(int h) { hour = h; }
  void setBoardingCount(int count) { boardingCount = count; }
  void setAlightingCount(int count) { alightingCount = count; }
  void setTrainId(const std::string &id) { trainId = id; }
  void setDirection(const std::string &dir) { direction = dir; }

  // 功能方法
  int getTotalFlow() const { return boardingCount + alightingCount; }
  int getNetFlow() const { return boardingCount - alightingCount; }
  std::string toString() const;
};

// 客流数据管理类
class PassengerFlow {
private:
  std::vector<FlowRecord> records;                    // 所有客流记录
  std::map<std::string, int> stationDailyFlow;        // 站点日客流统计
  std::map<std::string, std::vector<int>> hourlyFlow; // 小时客流统计

public:
  // 构造函数
  PassengerFlow();

  // 析构函数
  ~PassengerFlow();

  // 数据管理
  void addRecord(const FlowRecord &record);
  void removeRecord(const std::string &recordId);
  FlowRecord *findRecord(const std::string &recordId);
  std::vector<FlowRecord>
  getRecordsByStation(const std::string &stationId) const;
  std::vector<FlowRecord> getRecordsByDate(const Date &date) const;
  std::vector<FlowRecord> getRecordsByDateRange(const Date &startDate,
                                                const Date &endDate) const;

  // 统计分析
  int getStationTotalFlow(const std::string &stationId) const;
  int getStationDailyFlow(const std::string &stationId, const Date &date) const;
  std::vector<int> getStationHourlyFlow(const std::string &stationId,
                                        const Date &date) const;
  std::map<std::string, int> getAllStationsFlow() const;

  // 川渝流量分析
  int getChengduToChongqingFlow(const Date &date) const;
  int getChongqingToChengduFlow(const Date &date) const;
  double getFlowRatio() const; // 川渝流量比

  // 载客率分析
  double calculateLoadFactor(const std::string &trainId,
                             const Date &date) const;
  std::map<std::string, double> getAllTrainsLoadFactor(const Date &date) const;

  // 预测功能
  std::vector<int> predictFlow(const std::string &stationId,
                               int days = 3) const;
  std::vector<int> predictDirectionalFlow(const std::string &direction,
                                          int days = 3) const;

  // 数据导出
  std::string generateFlowReport(const Date &date) const;
  std::string generateStationRanking() const;
  void updateStatistics();

  // 辅助方法
  int getRecordCount() const { return static_cast<int>(records.size()); }
  void clearAllRecords() { records.clear(); }
};

#endif // PASSENGERFLOW_H