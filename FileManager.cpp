#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

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
    lastError = "解析站点数据错误: " + std::string(e.what());
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

// 其他未实现的方法（简化版本）
std::vector<std::shared_ptr<Route>>
FileManager::loadRoutes(const std::vector<std::shared_ptr<Station>> &stations) {
  return std::vector<std::shared_ptr<Route>>();
}

bool FileManager::saveRoutes(
    const std::vector<std::shared_ptr<Route>> &routes) {
  return true;
}

bool FileManager::saveRoute(const Route &route) { return true; }

std::vector<std::shared_ptr<Train>>
FileManager::loadTrains(const std::vector<std::shared_ptr<Route>> &routes) {
  return std::vector<std::shared_ptr<Train>>();
}

bool FileManager::saveTrains(
    const std::vector<std::shared_ptr<Train>> &trains) {
  return true;
}

bool FileManager::saveTrain(const Train &train) { return true; }

bool FileManager::saveFlowRecords(const PassengerFlow &passengerFlow) {
  return true;
}

bool FileManager::loadFlowRecords(PassengerFlow &passengerFlow) { return true; }

bool FileManager::appendFlowRecord(const FlowRecord &record) { return true; }