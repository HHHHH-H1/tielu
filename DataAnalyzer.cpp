#include "DataAnalyzer.h"
#include <algorithm>
#include <iostream>
#include <sstream>

// 构造函数
DataAnalyzer::DataAnalyzer() : passengerFlow(nullptr) {}

DataAnalyzer::DataAnalyzer(std::shared_ptr<PassengerFlow> flow)
    : passengerFlow(flow) {}

// 析构函数
DataAnalyzer::~DataAnalyzer() {
  stations.clear();
  routes.clear();
  trains.clear();
}

// 数据管理
void DataAnalyzer::addStation(std::shared_ptr<Station> station) {
  if (station) {
    stations.push_back(station);
  }
}

void DataAnalyzer::addRoute(std::shared_ptr<Route> route) {
  if (route) {
    routes.push_back(route);
  }
}

void DataAnalyzer::addTrain(std::shared_ptr<Train> train) {
  if (train) {
    trains.push_back(train);
  }
}

void DataAnalyzer::setPassengerFlow(std::shared_ptr<PassengerFlow> flow) {
  passengerFlow = flow;
}

// 站点分析
AnalysisResult
DataAnalyzer::analyzeStationFlow(const std::string &stationId) const {
  AnalysisResult result("站点客流分析", "分析指定站点的客流情况");

  if (!passengerFlow) {
    return result;
  }

  int totalFlow = passengerFlow->getStationTotalFlow(stationId);
  result.data["总客流量"] = totalFlow;

  return result;
}

AnalysisResult DataAnalyzer::analyzeAllStationsFlow() const {
  AnalysisResult result("全站点客流分析", "分析所有站点的客流情况");

  if (!passengerFlow) {
    return result;
  }

  auto stationFlow = passengerFlow->getAllStationsFlow();
  for (const auto &pair : stationFlow) {
    result.data[pair.first] = pair.second;
  }

  return result;
}

ChartData DataAnalyzer::generateStationFlowChart() const {
  ChartData chart("bar", "站点客流图");

  if (!passengerFlow) {
    return chart;
  }

  auto stationFlow = passengerFlow->getAllStationsFlow();
  for (const auto &pair : stationFlow) {
    chart.labels.push_back(pair.first);
    chart.values.push_back(pair.second);
  }

  return chart;
}

ChartData DataAnalyzer::generateStationRankingChart() const {
  ChartData chart("bar", "站点客流排行");

  if (!passengerFlow) {
    return chart;
  }

  auto stationFlow = passengerFlow->getAllStationsFlow();

  // 转换为vector并排序
  std::vector<std::pair<std::string, int>> ranking;
  for (const auto &pair : stationFlow) {
    ranking.push_back(pair);
  }

  std::sort(
      ranking.begin(), ranking.end(),
      [](const std::pair<std::string, int> &a,
         const std::pair<std::string, int> &b) { return a.second > b.second; });

  for (const auto &pair : ranking) {
    chart.labels.push_back(pair.first);
    chart.values.push_back(pair.second);
  }

  return chart;
}

// 方向性分析
AnalysisResult DataAnalyzer::analyzeDirectionalFlow() const {
  AnalysisResult result("方向性流量分析", "分析川渝双向客流");

  if (!passengerFlow) {
    return result;
  }

  double ratio = passengerFlow->getFlowRatio();
  result.data["川渝流量比"] = ratio;

  return result;
}

AnalysisResult DataAnalyzer::analyzeChengduChongqingFlow() const {
  AnalysisResult result("川渝流量分析", "分析成都重庆间客流");

  if (!passengerFlow) {
    return result;
  }

  Date today(2024, 12, 15);
  int chengduToChongqing = passengerFlow->getChengduToChongqingFlow(today);
  int chongqingToChengdu = passengerFlow->getChongqingToChengduFlow(today);

  result.data["成都->重庆"] = chengduToChongqing;
  result.data["重庆->成都"] = chongqingToChengdu;

  return result;
}

ChartData DataAnalyzer::generateDirectionalFlowChart(const Date &date) const {
  ChartData chart("pie", "方向性流量图");

  if (!passengerFlow) {
    return chart;
  }

  int chengduToChongqing = passengerFlow->getChengduToChongqingFlow(date);
  int chongqingToChengdu = passengerFlow->getChongqingToChengduFlow(date);

  chart.labels.push_back("川->渝");
  chart.labels.push_back("渝->川");
  chart.values.push_back(chengduToChongqing);
  chart.values.push_back(chongqingToChengdu);

  return chart;
}

// 预测分析
AnalysisResult DataAnalyzer::predictStationFlow(const std::string &stationId,
                                                int days) const {
  AnalysisResult result("站点流量预测",
                        "预测未来" + std::to_string(days) + "天的客流");

  if (!passengerFlow) {
    return result;
  }

  auto prediction = passengerFlow->predictFlow(stationId, days);
  for (size_t i = 0; i < prediction.size(); ++i) {
    std::string dayKey = "第" + std::to_string(i + 1) + "天";
    result.data[dayKey] = prediction[i];
  }

  return result;
}

AnalysisResult
DataAnalyzer::predictDirectionalFlow(const std::string &direction,
                                     int days) const {
  AnalysisResult result("方向流量预测", "预测" + direction + "方向未来" +
                                            std::to_string(days) + "天的客流");

  if (!passengerFlow) {
    return result;
  }

  auto prediction = passengerFlow->predictDirectionalFlow(direction, days);
  for (size_t i = 0; i < prediction.size(); ++i) {
    std::string dayKey = "第" + std::to_string(i + 1) + "天";
    result.data[dayKey] = prediction[i];
  }

  return result;
}

ChartData DataAnalyzer::generatePredictionChart(const std::string &stationId,
                                                int days) const {
  ChartData chart("line", "客流预测图");

  if (!passengerFlow) {
    return chart;
  }

  auto prediction = passengerFlow->predictFlow(stationId, days);
  for (size_t i = 0; i < prediction.size(); ++i) {
    chart.labels.push_back("第" + std::to_string(i + 1) + "天");
    chart.values.push_back(prediction[i]);
  }

  return chart;
}

// 综合分析
AnalysisResult DataAnalyzer::generateDailyReport(const Date &date) const {
  AnalysisResult result("日报告", "生成" + date.toString() + "的综合报告");

  if (!passengerFlow) {
    return result;
  }

  int chengduToChongqing = passengerFlow->getChengduToChongqingFlow(date);
  int chongqingToChengdu = passengerFlow->getChongqingToChengduFlow(date);

  result.data["川->渝客流"] = chengduToChongqing;
  result.data["渝->川客流"] = chongqingToChengdu;
  result.data["总客流"] = chengduToChongqing + chongqingToChengdu;

  return result;
}

// 数据导出
std::string
DataAnalyzer::exportAnalysisToText(const AnalysisResult &result) const {
  std::ostringstream oss;
  oss << "=== " << result.title << " ===" << std::endl;
  oss << result.description << std::endl << std::endl;

  for (const auto &pair : result.data) {
    oss << pair.first << ": " << pair.second << std::endl;
  }

  return oss.str();
}

std::string DataAnalyzer::exportChartToText(const ChartData &chart) const {
  std::ostringstream oss;
  oss << "=== " << chart.title << " ===" << std::endl;
  oss << "图表类型: " << chart.chartType << std::endl;
  oss << "数值单位: " << chart.unit << std::endl << std::endl;

  for (size_t i = 0; i < chart.labels.size() && i < chart.values.size(); ++i) {
    oss << chart.labels[i] << ": " << chart.values[i] << " " << chart.unit
        << std::endl;
  }

  return oss.str();
}

std::string DataAnalyzer::generateFullReport() const {
  std::ostringstream oss;
  oss << "=== 川渝轨道交通客流分析综合报告 ===" << std::endl << std::endl;

  oss << "系统概况:" << std::endl;
  oss << "站点数量: " << getTotalStations() << std::endl;
  oss << "线路数量: " << getTotalRoutes() << std::endl;
  oss << "列车数量: " << getTotalTrains() << std::endl;

  if (passengerFlow) {
    oss << "客流记录数: " << passengerFlow->getRecordCount() << std::endl;
  }

  return oss.str();
}

// 辅助方法
bool DataAnalyzer::isValidStationId(const std::string &stationId) const {
  return findStation(stationId) != nullptr;
}

bool DataAnalyzer::isValidRouteId(const std::string &routeId) const {
  return findRoute(routeId) != nullptr;
}

bool DataAnalyzer::isValidTrainId(const std::string &trainId) const {
  return findTrain(trainId) != nullptr;
}

std::shared_ptr<Station>
DataAnalyzer::findStation(const std::string &stationId) const {
  auto it =
      std::find_if(stations.begin(), stations.end(),
                   [&stationId](const std::shared_ptr<Station> &station) {
                     return station && station->getStationId() == stationId;
                   });

  return (it != stations.end()) ? *it : nullptr;
}

std::shared_ptr<Route>
DataAnalyzer::findRoute(const std::string &routeId) const {
  auto it = std::find_if(routes.begin(), routes.end(),
                         [&routeId](const std::shared_ptr<Route> &route) {
                           return route && route->getRouteId() == routeId;
                         });

  return (it != routes.end()) ? *it : nullptr;
}

std::shared_ptr<Train>
DataAnalyzer::findTrain(const std::string &trainId) const {
  auto it = std::find_if(trains.begin(), trains.end(),
                         [&trainId](const std::shared_ptr<Train> &train) {
                           return train && train->getTrainId() == trainId;
                         });

  return (it != trains.end()) ? *it : nullptr;
}