#include "FileManager.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

// 构造函数
FileManager::FileManager() : dataDirectory("data") {
  stationsFile = "客运站点（站点名称、站点编号、备注）.csv";
  routesFile = "运营线路客运站（运营线路编码、站点id、线路站点id、上一站id、运"
               "营线路站间距离 、下一站id、运输距离、线路代码）.csv";
  trainsFile = "列车表（列车编码、列车代码、列车运量）.csv";
  flowRecordsFile =
      "高铁客运量（成都--"
      "重庆）（运营线路编码、列车编码、站点id、日期、到达时间、出发时间、上客量"
      "、下客量等，起点站、终点站、票价、收入等）.csv";
  configFile = "config.txt";
}

FileManager::FileManager(const std::string &dataDir) : dataDirectory(dataDir) {
  stationsFile = "客运站点（站点名称、站点编号、备注）.csv";
  routesFile = "运营线路客运站（运营线路编码、站点id、线路站点id、上一站id、运"
               "营线路站间距离 、下一站id、运输距离、线路代码）.csv";
  trainsFile = "列车表（列车编码、列车代码、列车运量）.csv";
  flowRecordsFile =
      "高铁客运量（成都--"
      "重庆）（运营线路编码、列车编码、站点id、日期、到达时间、出发时间、上客量"
      "、下客量等，起点站、终点站、票价、收入等）.csv";
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
  // 处理空或NULL字符串
  if (dateStr.empty() || dateStr == "NULL") {
    date = Date(2024, 12, 15); // 默认日期
    return true;
  }

  try {
    // 支持多种日期格式：YYYY-MM-DD, YYYYMMDD
    std::stringstream ss(dateStr);
    std::string token;

    if (dateStr.find('-') != std::string::npos) {
      // YYYY-MM-DD 格式
      if (std::getline(ss, token, '-')) {
        date.year = std::stoi(token);
      }
      if (std::getline(ss, token, '-')) {
        date.month = std::stoi(token);
      }
      if (std::getline(ss, token, '-')) {
        date.day = std::stoi(token);
      }
    } else if (dateStr.length() == 8) {
      // YYYYMMDD 格式
      date.year = std::stoi(dateStr.substr(0, 4));
      date.month = std::stoi(dateStr.substr(4, 2));
      date.day = std::stoi(dateStr.substr(6, 2));
    } else {
      // 默认日期
      date = Date(2024, 12, 15);
    }

    return true;
  } catch (const std::exception &) {
    date = Date(2024, 12, 15); // 出错时使用默认日期
    return false;
  }
}

std::string FileManager::dateToString(const Date &date) const {
  return std::to_string(date.year) + "-" + std::to_string(date.month) + "-" +
         std::to_string(date.day);
}

// 数据解析方法 - 适应实际CSV文件格式
std::shared_ptr<Station>
FileManager::parseStationFromCSV(const std::vector<std::string> &fields) const {
  // 实际CSV格式：zdid（站点编号）,,,lxid,,,ysfsbm,zdmc（站点名称）,,,,sfty（是否停用）,,station_code(站点代码）,station_telecode（站点电报码）,station_shortname(站点简称）,
  if (fields.size() < 16) {
    return nullptr;
  }

  try {
    // 从实际CSV文件中提取数据
    std::string id = fields[14];        // station_telecode作为ID
    std::string name = fields[7];       // zdmc（站点名称）
    std::string shortName = fields[15]; // station_shortname(站点简称）
    std::string code = fields[13];      // station_code(站点代码）

    // 去除空格
    name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);
    shortName.erase(shortName.find_last_not_of(" \t\n\r\f\v") + 1);

    // 如果站点名称为空，跳过
    if (name.empty() || name == "NULL") {
      return nullptr;
    }

    // 设置默认值
    double longitude = 106.5 + (rand() % 200 - 100) * 0.01; // 模拟经度
    double latitude = 29.5 + (rand() % 200 - 100) * 0.01;   // 模拟纬度
    std::string type = "客运站";
    int platformCount = 2 + rand() % 6; // 2-8个站台
    bool isTransfer = false;

    // 判断是否为重要站点（包含"东"、"西"、"南"、"北"等的大站）
    if (name.find("东") != std::string::npos ||
        name.find("西") != std::string::npos ||
        name.find("南") != std::string::npos ||
        name.find("北") != std::string::npos ||
        name.find("中心") != std::string::npos) {
      platformCount = 6 + rand() % 6; // 6-12个站台
      isTransfer = true;
    }

    // 确定城市名称
    std::string city = "其他";
    if (name.find("北京") != std::string::npos)
      city = "北京";
    else if (name.find("天津") != std::string::npos)
      city = "天津";
    else if (name.find("成都") != std::string::npos)
      city = "成都";
    else if (name.find("重庆") != std::string::npos)
      city = "重庆";
    else if (name.find("唐山") != std::string::npos)
      city = "唐山";
    else if (name.find("石家庄") != std::string::npos)
      city = "石家庄";

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
std::shared_ptr<Route> FileManager::parseRouteFromCSV(
    const std::vector<std::string> &fields,
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

// 解析列车CSV字段 - 适应实际CSV文件格式
std::shared_ptr<Train> FileManager::parseTrainFromCSV(
    const std::vector<std::string> &fields,
    const std::vector<std::shared_ptr<Route>> &routes) const {
  // 实际CSV格式：lcbm（列车编码）,sxxbm,ysfsbm,lcdm（列车代码）,cc（车次）,sfzt,lcyn（列车运能）
  if (fields.size() < 7) {
    return nullptr;
  }

  try {
    std::string trainCode = fields[4];   // cc（车次）
    std::string capacityStr = fields[6]; // lcyn（列车运能）

    // 如果车次为空，跳过
    if (trainCode.empty() || trainCode == "NULL") {
      return nullptr;
    }

    // 解析运能
    int capacity = 1000; // 默认值
    if (capacityStr != "#N/A" && capacityStr != "NULL" &&
        !capacityStr.empty()) {
      try {
        capacity = std::stoi(capacityStr);
      } catch (...) {
        capacity = 1000;
      }
    }

    // 确定列车类型
    std::string type = "普通";
    if (trainCode.find("G") == 0)
      type = "高铁";
    else if (trainCode.find("D") == 0)
      type = "动车";
    else if (trainCode.find("C") == 0)
      type = "城际";
    else if (trainCode.find("K") == 0)
      type = "快速";
    else if (trainCode.find("T") == 0)
      type = "特快";

    // 暂时不关联具体线路，设为空
    std::shared_ptr<Route> route = nullptr;
    if (!routes.empty()) {
      route = routes[0]; // 使用第一条线路作为默认
    }

    return std::make_shared<Train>(trainCode, type, route, capacity);
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

FlowRecord FileManager::parseFlowRecordFromCSV(
    const std::vector<std::string> &fields) const {
  if (fields.size() < 9) {
    return FlowRecord();
  }

  try {
    Date date;
    if (!parseDateFromString(fields[3], date)) {
      return FlowRecord();
    }

    // 安全的字符串到整数转换
    int hour = 0;
    int boarding = 0;
    int alighting = 0;

    try {
      if (!fields[4].empty() && fields[4] != "NULL") {
        hour = std::stoi(fields[4]);
      }
    } catch (const std::exception &) {
      hour = 0;
    }

    try {
      if (!fields[5].empty() && fields[5] != "NULL") {
        boarding = std::stoi(fields[5]);
      }
    } catch (const std::exception &) {
      boarding = 0;
    }

    try {
      if (!fields[6].empty() && fields[6] != "NULL") {
        alighting = std::stoi(fields[6]);
      }
    } catch (const std::exception &) {
      alighting = 0;
    }

    return FlowRecord(fields[0], fields[1], fields[2], date, hour, boarding,
                      alighting, fields[7], fields[8]);
  } catch (const std::exception &) {
    return FlowRecord();
  }
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
  file << "RecordID,StationID,StationName,Date,Hour,BoardingCount,"
          "AlightingCount,TrainID,Direction\n";
  auto records =
      passengerFlow.getRecordsByDateRange(Date(0, 1, 1), Date(9999, 12, 31));
  for (const auto &rec : records) {
    file << formatFlowRecordToCSV(rec) << '\n';
  }
  return true;
}

bool FileManager::loadFlowRecords(PassengerFlow &passengerFlow) {
  std::string fullPath = getFullPath(flowRecordsFile);
  std::ifstream file(fullPath);
  if (!file.is_open()) {
    lastError = "客流数据文件不存在或无法打开: " + fullPath;
    return false;
  }

  std::string line;
  bool first = true;
  int validRecords = 0;
  int totalLines = 0;

  while (std::getline(file, line)) {
    totalLines++;
    if (first) {
      first = false;
      continue; // 跳过头行
    }

    // 跳过空行或无效行
    if (line.empty() ||
        line.find_first_not_of(" \t\r\n") == std::string::npos) {
      continue;
    }

    try {
      auto fields = splitCSVLine(line);
      if (fields.size() >= 9) { // 确保有足够的字段
        auto record = parseFlowRecordFromCSV(fields);
        if (!record.getRecordId().empty()) { // 确保记录有效
          passengerFlow.addRecord(record);
          validRecords++;
        }
      }
    } catch (const std::exception &) {
      // 跳过无法解析的行
      continue;
    }

    // 限制加载数量避免内存问题
    if (validRecords >= 10000) {
      break;
    }
  }

  if (validRecords == 0) {
    lastError = "未找到有效的客流数据记录";
    return false;
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
  file << "StationID,StationName,CityName,Longitude,Latitude,StationType,"
          "PlatformCount,IsTransferStation\n";
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
  file << "RecordID,StationID,StationName,Date,Hour,BoardingCount,"
          "AlightingCount,TrainID,Direction\n";
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
