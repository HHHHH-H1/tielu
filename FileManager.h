#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "PassengerFlow.h"
#include "Route.h"
#include "Station.h"
#include "Train.h"
#include <fstream>
#include <memory>
#include <string>
#include <vector>


class FileManager {
private:
  std::string dataDirectory;   // 数据目录路径
  std::string stationsFile;    // 站点数据文件
  std::string routesFile;      // 线路数据文件
  std::string trainsFile;      // 列车数据文件
  std::string flowRecordsFile; // 客流记录文件
  std::string configFile;      // 配置文件

public:
  // 构造函数
  FileManager();
  FileManager(const std::string &dataDir);

  // 析构函数
  ~FileManager();

  // 文件路径设置
  void setDataDirectory(const std::string &dir);
  void setStationsFile(const std::string &filename);
  void setRoutesFile(const std::string &filename);
  void setTrainsFile(const std::string &filename);
  void setFlowRecordsFile(const std::string &filename);
  void setConfigFile(const std::string &filename);

  // 站点数据操作
  bool saveStations(const std::vector<std::shared_ptr<Station>> &stations);
  std::vector<std::shared_ptr<Station>> loadStations();
  bool saveStation(const Station &station);

  // 线路数据操作
  bool saveRoutes(const std::vector<std::shared_ptr<Route>> &routes);
  std::vector<std::shared_ptr<Route>>
  loadRoutes(const std::vector<std::shared_ptr<Station>> &stations);
  bool saveRoute(const Route &route);

  // 列车数据操作
  bool saveTrains(const std::vector<std::shared_ptr<Train>> &trains);
  std::vector<std::shared_ptr<Train>>
  loadTrains(const std::vector<std::shared_ptr<Route>> &routes);
  bool saveTrain(const Train &train);

  // 客流记录操作
  bool saveFlowRecords(const PassengerFlow &passengerFlow);
  bool loadFlowRecords(PassengerFlow &passengerFlow);
  bool appendFlowRecord(const FlowRecord &record);

  // 批量数据操作
  bool exportAllData(const std::vector<std::shared_ptr<Station>> &stations,
                     const std::vector<std::shared_ptr<Route>> &routes,
                     const std::vector<std::shared_ptr<Train>> &trains,
                     const PassengerFlow &passengerFlow);

  bool importAllData(std::vector<std::shared_ptr<Station>> &stations,
                     std::vector<std::shared_ptr<Route>> &routes,
                     std::vector<std::shared_ptr<Train>> &trains,
                     PassengerFlow &passengerFlow);

  // 数据备份和恢复
  bool backupData(const std::string &backupDir);
  bool restoreData(const std::string &backupDir);

  // CSV格式操作
  bool
  exportStationsToCSV(const std::vector<std::shared_ptr<Station>> &stations,
                      const std::string &filename);
  bool exportRoutesToCSV(const std::vector<std::shared_ptr<Route>> &routes,
                         const std::string &filename);
  bool exportFlowRecordsToCSV(const PassengerFlow &passengerFlow,
                              const std::string &filename);

  bool importStationsFromCSV(const std::string &filename,
                             std::vector<std::shared_ptr<Station>> &stations);
  bool
  importRoutesFromCSV(const std::string &filename,
                      const std::vector<std::shared_ptr<Station>> &stations,
                      std::vector<std::shared_ptr<Route>> &routes);
  bool importFlowRecordsFromCSV(const std::string &filename,
                                PassengerFlow &passengerFlow);

  // 配置文件操作
  bool saveConfig(const std::map<std::string, std::string> &config);
  std::map<std::string, std::string> loadConfig();

  // 报告生成
  bool generateReport(const std::string &reportContent,
                      const std::string &filename);
  bool generateAnalysisReport(const std::string &analysisData,
                              const std::string &filename);

  // 文件工具方法
  bool fileExists(const std::string &filename) const;
  bool createDirectory(const std::string &dirPath) const;
  bool deleteFile(const std::string &filename) const;
  long getFileSize(const std::string &filename) const;
  std::string getLastModifiedTime(const std::string &filename) const;

  // 数据验证
  bool validateStationData(const std::string &filename) const;
  bool validateRouteData(const std::string &filename) const;
  bool validateFlowData(const std::string &filename) const;

  // 错误处理
  std::string getLastError() const { return lastError; }
  void clearError() { lastError.clear(); }

private:
  mutable std::string lastError; // 最后一次错误信息

  // 辅助方法
  std::string getFullPath(const std::string &filename) const;
  std::vector<std::string> splitCSVLine(const std::string &line) const;
  std::string escapeCSVValue(const std::string &value) const;
  bool parseDateFromString(const std::string &dateStr, Date &date) const;
  std::string dateToString(const Date &date) const;

  // 数据解析方法
  std::shared_ptr<Station>
  parseStationFromCSV(const std::vector<std::string> &fields) const;
  std::shared_ptr<Route> parseRouteFromCSV(
      const std::vector<std::string> &fields,
      const std::vector<std::shared_ptr<Station>> &stations) const;
  std::shared_ptr<Train>
  parseTrainFromCSV(const std::vector<std::string> &fields,
                    const std::vector<std::shared_ptr<Route>> &routes) const;
  FlowRecord
  parseFlowRecordFromCSV(const std::vector<std::string> &fields) const;

  // 数据格式化方法
  std::string formatStationToCSV(const Station &station) const;
  std::string formatRouteToCSV(const Route &route) const;
  std::string formatTrainToCSV(const Train &train) const;
  std::string formatFlowRecordToCSV(const FlowRecord &record) const;
};

#endif // FILEMANAGER_H