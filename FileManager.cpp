#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// 构造函数
FileManager::FileManager() : dataDirectory("data") {
  stationsFile = "stations.csv";
  routesFile = "routes.csv";
  trainsFile = "trains.csv";
  flowRecordsFile = "flow_records.csv";
  configFile = "config.txt";
}

FileManager::FileManager(const std::string &dataDir) : dataDirectory(dataDir) {
  stationsFile = "stations.csv";
  routesFile = "routes.csv";
  trainsFile = "trains.csv";
  flowRecordsFile = "flow_records.csv";
  configFile = "config.txt";
}

// 析构函数
FileManager::~FileManager() {}

// 文件路径设置
void FileManager::setDataDirectory(const std::string &dir) {
  dataDirectory = dir;
}

void FileManager::setStationsFile(const std::string &filename) {
  stationsFile = filename;
}

void FileManager::setRoutesFile(const std::string &filename) {
  routesFile = filename;
}

void FileManager::setTrainsFile(const std::string &filename) {
  trainsFile = filename;
}

void FileManager::setFlowRecordsFile(const std::string &filename) {
  flowRecordsFile = filename;
}

void FileManager::setConfigFile(const std::string &filename) {
  configFile = filename;
}

// 站点数据操作
bool FileManager::saveStations(
    const std::vector<std::shared_ptr<Station>> &stations) {
  std::string fullPath = getFullPath(stationsFile);
  std::ofstream file(fullPath);

  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }

  // 写入CSV头部
  file << "StationID,StationName,CityName,Longitude,Latitude,StationType,"
          "PlatformCount,IsTransferStation\n";

  // 写入数据
  for (const auto &station : stations) {
    if (station) {
      file << formatStationToCSV(*station) << "\n";
    }
  }

  file.close();
  return true;
}

std::vector<std::shared_ptr<Station>> FileManager::loadStations() {
  std::vector<std::shared_ptr<Station>> stations;
  std::string fullPath = getFullPath(stationsFile);
  std::ifstream file(fullPath);

  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return stations;
  }

  std::string line;
  bool firstLine = true;

  while (std::getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue; // 跳过CSV头部
    }

    auto fields = splitCSVLine(line);
    if (fields.size() >= 8) {
      auto station = parseStationFromCSV(fields);
      if (station) {
        stations.push_back(station);
      }
    }
  }

  file.close();
  return stations;
}

bool FileManager::saveStation(const Station &station) {
  // 简化实现：追加到文件末尾
  std::string fullPath = getFullPath(stationsFile);
  std::ofstream file(fullPath, std::ios::app);

  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }

  file << formatStationToCSV(station) << "\n";
  file.close();
  return true;
}

// 文件工具方法
bool FileManager::fileExists(const std::string &filename) const {
  std::ifstream file(filename);
  return file.good();
}

bool FileManager::createDirectory(const std::string &dirPath) const {
  // 简化实现，实际应该使用filesystem库
  return true;
}

bool FileManager::deleteFile(const std::string &filename) const {
  return std::remove(filename.c_str()) == 0;
}

long FileManager::getFileSize(const std::string &filename) const {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  return file.tellg();
}

std::string
FileManager::getLastModifiedTime(const std::string &filename) const {
  return "未实现";
}

// 辅助方法
std::string FileManager::getFullPath(const std::string &filename) const {
  if (dataDirectory.empty()) {
    return filename;
  }
  return dataDirectory + "/" + filename;
}

std::vector<std::string>
FileManager::splitCSVLine(const std::string &line) const {
  std::vector<std::string> fields;
  std::stringstream ss(line);
  std::string field;

  while (std::getline(ss, field, ',')) {
    fields.push_back(field);
  }

  return fields;
}

std::string FileManager::escapeCSVValue(const std::string &value) const {
  // 简化实现
  return value;
}

bool FileManager::parseDateFromString(const std::string &dateStr,
                                      Date &date) const {
  // 简化实现：假设格式为YYYY-MM-DD
  std::stringstream ss(dateStr);
  std::string token;

  if (std::getline(ss, token, '-')) {
    date.year = std::stoi(token);
  }
  if (std::getline(ss, token, '-')) {
    date.month = std::stoi(token);
  }
  if (std::getline(ss, token, '-')) {
    date.day = std::stoi(token);
  }

  return true;
}

std::string FileManager::dateToString(const Date &date) const {
  return std::to_string(date.year) + "-" + std::to_string(date.month) + "-" +
         std::to_string(date.day);
}

// 数据解析方法
std::shared_ptr<Station>
FileManager::parseStationFromCSV(const std::vector<std::string> &fields) const {
  if (fields.size() < 8) {
    return nullptr;
  }

  try {
    std::string id = fields[0];
    std::string name = fields[1];
    std::string city = fields[2];
    double longitude = std::stod(fields[3]);
    double latitude = std::stod(fields[4]);
    std::string type = fields[5];
    int platformCount = std::stoi(fields[6]);
    bool isTransfer = (fields[7] == "1" || fields[7] == "true");

    return std::make_shared<Station>(id, name, city, longitude, latitude, type,
                                     platformCount, isTransfer);
  } catch (const std::exception &e) {
    lastError = std::string("解析站点数据错误: ") + e.what();
    return nullptr;
  }
}

// 数据格式化方法
std::string FileManager::formatStationToCSV(const Station &station) const {
  std::ostringstream oss;
  oss << station.getStationId() << "," << station.getStationName() << ","
      << station.getCityName() << "," << station.getLongitude() << ","
      << station.getLatitude() << "," << station.getStationType() << ","
      << station.getPlatformCount() << ","
      << (station.getIsTransferStation() ? "1" : "0");
  return oss.str();
}

// 解析线路CSV字段
std::shared_ptr<Route>
FileManager::parseRouteFromCSV(const std::vector<std::string> &fields,
                               const std::vector<std::shared_ptr<Station>> &stations) const {
  if (fields.size() < 6) {
    return nullptr;
  }

  try {
    std::string id = fields[0];
    std::string name = fields[1];
    std::string type = fields[2];
    double distance = std::stod(fields[3]);
    int speed = std::stoi(fields[4]);

    auto route = std::make_shared<Route>(id, name, type, distance, speed);

    std::stringstream ss(fields[5]);
    std::string stId;
    while (std::getline(ss, stId, ';')) {
      auto it = std::find_if(stations.begin(), stations.end(),
                             [&](const std::shared_ptr<Station> &st) {
                               return st && st->getStationId() == stId;
                             });
      if (it != stations.end()) {
        route->addStation(*it);
      }
    }

    return route;
  } catch (const std::exception &e) {
    lastError = std::string("解析线路数据错误: ") + e.what();
    return nullptr;
  }
}

std::string FileManager::formatRouteToCSV(const Route &route) const {
  std::ostringstream oss;
  oss << route.getRouteId() << ',' << route.getRouteName() << ','
      << route.getRouteType() << ',' << route.getTotalDistance() << ','
      << route.getMaxSpeed() << ',';

  const auto &sts = route.getStations();
  for (size_t i = 0; i < sts.size(); ++i) {
    if (sts[i]) {
      if (i > 0)
        oss << ';';
      oss << sts[i]->getStationId();
    }
  }
  return oss.str();
}

// 解析列车CSV字段
std::shared_ptr<Train>
FileManager::parseTrainFromCSV(const std::vector<std::string> &fields,
                               const std::vector<std::shared_ptr<Route>> &routes) const {
  if (fields.size() < 4) {
    return nullptr;
  }

  try {
    std::string id = fields[0];
    std::string type = fields[1];
    std::string routeId = fields[2];
    int capacity = std::stoi(fields[3]);

    auto it = std::find_if(routes.begin(), routes.end(),
                           [&](const std::shared_ptr<Route> &r) {
                             return r && r->getRouteId() == routeId;
                           });
    std::shared_ptr<Route> r = (it != routes.end()) ? *it : nullptr;
    return std::make_shared<Train>(id, type, r, capacity);
  } catch (const std::exception &e) {
    lastError = std::string("解析列车数据错误: ") + e.what();
    return nullptr;
  }
}

std::string FileManager::formatTrainToCSV(const Train &train) const {
  std::ostringstream oss;
  oss << train.getTrainId() << ',' << train.getTrainType() << ',';
  if (train.getRoute()) {
    oss << train.getRoute()->getRouteId();
  }
  oss << ',' << train.getTotalCapacity();
  return oss.str();
}

FlowRecord
FileManager::parseFlowRecordFromCSV(const std::vector<std::string> &fields) const {
  if (fields.size() < 9) {
    return FlowRecord();
  }

  Date date;
  parseDateFromString(fields[3], date);
  int hour = std::stoi(fields[4]);
  int boarding = std::stoi(fields[5]);
  int alighting = std::stoi(fields[6]);

  return FlowRecord(fields[0], fields[1], fields[2], date, hour, boarding,
                    alighting, fields[7], fields[8]);
}

std::string FileManager::formatFlowRecordToCSV(const FlowRecord &record) const {
  std::ostringstream oss;
  oss << record.getRecordId() << ',' << record.getStationId() << ','
      << record.getStationName() << ',' << dateToString(record.getDate()) << ','
      << record.getHour() << ',' << record.getBoardingCount() << ','
      << record.getAlightingCount() << ',' << record.getTrainId() << ','
      << record.getDirection();
  return oss.str();
}

// 其他未实现的方法（简化版本）
std::vector<std::shared_ptr<Route>>
FileManager::loadRoutes(const std::vector<std::shared_ptr<Station>> &stations) {
  std::vector<std::shared_ptr<Route>> routes;
  std::string fullPath = getFullPath(routesFile);
  std::ifstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return routes;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto route = parseRouteFromCSV(fields, stations);
    if (route)
      routes.push_back(route);
  }
  return routes;
}

bool FileManager::saveRoutes(
    const std::vector<std::shared_ptr<Route>> &routes) {
  std::string fullPath = getFullPath(routesFile);
  std::ofstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << "RouteID,RouteName,RouteType,TotalDistance,MaxSpeed,StationIDs\n";
  for (const auto &r : routes) {
    if (r)
      file << formatRouteToCSV(*r) << '\n';
  }
  return true;
}

bool FileManager::saveRoute(const Route &route) {
  std::string fullPath = getFullPath(routesFile);
  std::ofstream file(fullPath, std::ios::app);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << formatRouteToCSV(route) << '\n';
  return true;
}

std::vector<std::shared_ptr<Train>>
FileManager::loadTrains(const std::vector<std::shared_ptr<Route>> &routes) {
  std::vector<std::shared_ptr<Train>> trains;
  std::string fullPath = getFullPath(trainsFile);
  std::ifstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return trains;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto train = parseTrainFromCSV(fields, routes);
    if (train)
      trains.push_back(train);
  }
  return trains;
}

bool FileManager::saveTrains(
    const std::vector<std::shared_ptr<Train>> &trains) {
  std::string fullPath = getFullPath(trainsFile);
  std::ofstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << "TrainID,TrainType,RouteID,TotalCapacity\n";
  for (const auto &t : trains) {
    if (t)
      file << formatTrainToCSV(*t) << '\n';
  }
  return true;
}

bool FileManager::saveTrain(const Train &train) {
  std::string fullPath = getFullPath(trainsFile);
  std::ofstream file(fullPath, std::ios::app);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << formatTrainToCSV(train) << '\n';
  return true;
}

bool FileManager::saveFlowRecords(const PassengerFlow &passengerFlow) {
  std::string fullPath = getFullPath(flowRecordsFile);
  std::ofstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << "RecordID,StationID,StationName,Date,Hour,BoardingCount,AlightingCount,TrainID,Direction\n";
  auto records = passengerFlow.getRecordsByDateRange(Date(0, 1, 1),
                                                     Date(9999, 12, 31));
  for (const auto &rec : records) {
    file << formatFlowRecordToCSV(rec) << '\n';
  }
  return true;
}

bool FileManager::loadFlowRecords(PassengerFlow &passengerFlow) {
  std::string fullPath = getFullPath(flowRecordsFile);
  std::ifstream file(fullPath);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto record = parseFlowRecordFromCSV(fields);
    passengerFlow.addRecord(record);
  }
  return true;
}

bool FileManager::appendFlowRecord(const FlowRecord &record) {
  std::string fullPath = getFullPath(flowRecordsFile);
  std::ofstream file(fullPath, std::ios::app);
  if (!file.is_open()) {
    lastError = "无法打开文件: " + fullPath;
    return false;
  }
  file << formatFlowRecordToCSV(record) << '\n';
  return true;
}

bool FileManager::exportStationsToCSV(
    const std::vector<std::shared_ptr<Station>> &stations,
    const std::string &filename) {
  std::ofstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  file << "StationID,StationName,CityName,Longitude,Latitude,StationType,PlatformCount,IsTransferStation\n";
  for (const auto &station : stations) {
    if (station)
      file << formatStationToCSV(*station) << '\n';
  }
  return true;
}

bool FileManager::exportRoutesToCSV(
    const std::vector<std::shared_ptr<Route>> &routes,
    const std::string &filename) {
  std::ofstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  file << "RouteID,RouteName,RouteType,TotalDistance,MaxSpeed,StationIDs\n";
  for (const auto &route : routes) {
    if (route)
      file << formatRouteToCSV(*route) << '\n';
  }
  return true;
}

bool FileManager::exportFlowRecordsToCSV(const PassengerFlow &passengerFlow,
                                         const std::string &filename) {
  std::ofstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  file << "RecordID,StationID,StationName,Date,Hour,BoardingCount,AlightingCount,TrainID,Direction\n";
  auto records =
      passengerFlow.getRecordsByDateRange(Date(0, 1, 1), Date(9999, 12, 31));
  for (const auto &rec : records) {
    file << formatFlowRecordToCSV(rec) << '\n';
  }
  return true;
}

bool FileManager::importStationsFromCSV(
    const std::string &filename,
    std::vector<std::shared_ptr<Station>> &stations) {
  std::ifstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto st = parseStationFromCSV(fields);
    if (st)
      stations.push_back(st);
  }
  return true;
}

bool FileManager::importRoutesFromCSV(
    const std::string &filename,
    const std::vector<std::shared_ptr<Station>> &stations,
    std::vector<std::shared_ptr<Route>> &routes) {
  std::ifstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto rt = parseRouteFromCSV(fields, stations);
    if (rt)
      routes.push_back(rt);
  }
  return true;
}

bool FileManager::importFlowRecordsFromCSV(const std::string &filename,
                                           PassengerFlow &passengerFlow) {
  std::ifstream file(getFullPath(filename));
  if (!file.is_open()) {
    lastError = "无法打开文件: " + getFullPath(filename);
    return false;
  }
  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      continue;
    }
    auto fields = splitCSVLine(line);
    auto rec = parseFlowRecordFromCSV(fields);
    passengerFlow.addRecord(rec);
  }
  return true;
}

bool FileManager::exportAllData(
    const std::vector<std::shared_ptr<Station>> &stations,
    const std::vector<std::shared_ptr<Route>> &routes,
    const std::vector<std::shared_ptr<Train>> &trains,
    const PassengerFlow &passengerFlow) {
  return saveStations(stations) && saveRoutes(routes) && saveTrains(trains) &&
         saveFlowRecords(passengerFlow);
}

bool FileManager::importAllData(std::vector<std::shared_ptr<Station>> &stations,
                                std::vector<std::shared_ptr<Route>> &routes,
                                std::vector<std::shared_ptr<Train>> &trains,
                                PassengerFlow &passengerFlow) {
  stations = loadStations();
  if (!lastError.empty() && stations.empty())
    return false;
  routes = loadRoutes(stations);
  if (!lastError.empty())
    return false;
  trains = loadTrains(routes);
  if (!lastError.empty())
    return false;
  if (!loadFlowRecords(passengerFlow))
    return false;
  return true;
}
