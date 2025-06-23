#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include "PassengerFlow.h"
#include "Route.h"
#include "Station.h"
#include "Train.h"
#include <map>
#include <memory>
#include <string>
#include <vector>


// 分析结果结构
struct AnalysisResult {
  std::string title;
  std::map<std::string, double> data;
  std::string description;

  AnalysisResult(const std::string &t = "", const std::string &desc = "")
      : title(t), description(desc) {}
};

// 图表数据结构
struct ChartData {
  std::string chartType;           // 图表类型（柱状图、折线图、饼图等）
  std::string title;               // 图表标题
  std::vector<std::string> labels; // X轴标签
  std::vector<double> values;      // Y轴数值
  std::string unit;                // 数值单位

  ChartData(const std::string &type = "bar", const std::string &t = "")
      : chartType(type), title(t), unit("人次") {}
};

class DataAnalyzer {
private:
  std::vector<std::shared_ptr<Station>> stations;
  std::vector<std::shared_ptr<Route>> routes;
  std::vector<std::shared_ptr<Train>> trains;
  std::shared_ptr<PassengerFlow> passengerFlow;

public:
  // 构造函数
  DataAnalyzer();
  DataAnalyzer(std::shared_ptr<PassengerFlow> flow);

  // 析构函数
  ~DataAnalyzer();

  // 数据管理
  void addStation(std::shared_ptr<Station> station);
  void addRoute(std::shared_ptr<Route> route);
  void addTrain(std::shared_ptr<Train> train);
  void setPassengerFlow(std::shared_ptr<PassengerFlow> flow);

  // 站点分析
  AnalysisResult analyzeStationFlow(const std::string &stationId) const;
  AnalysisResult analyzeAllStationsFlow() const;
  ChartData generateStationFlowChart() const;
  ChartData generateStationRankingChart() const;

  // 线路分析
  AnalysisResult analyzeRouteEfficiency(const std::string &routeId) const;
  AnalysisResult analyzeAllRoutesFlow() const;
  ChartData generateRouteComparisonChart() const;

  // 列车分析
  AnalysisResult analyzeTrainLoadFactor() const;
  AnalysisResult analyzeTrainPerformance(const std::string &trainId) const;
  ChartData generateLoadFactorChart(const Date &date) const;

  // 时间段分析
  AnalysisResult analyzeHourlyFlow(const std::string &stationId,
                                   const Date &date) const;
  AnalysisResult analyzeDailyFlow(const Date &startDate,
                                  const Date &endDate) const;
  ChartData generateHourlyFlowChart(const std::string &stationId,
                                    const Date &date) const;
  ChartData generateDailyTrendChart(const Date &startDate,
                                    const Date &endDate) const;

  // 方向性分析
  AnalysisResult analyzeDirectionalFlow() const;
  AnalysisResult analyzeChengduChongqingFlow() const;
  ChartData generateDirectionalFlowChart(const Date &date) const;

  // 预测分析
  AnalysisResult predictStationFlow(const std::string &stationId,
                                    int days = 3) const;
  AnalysisResult predictDirectionalFlow(const std::string &direction,
                                        int days = 3) const;
  ChartData generatePredictionChart(const std::string &stationId,
                                    int days = 3) const;

  // 综合分析
  AnalysisResult generateDailyReport(const Date &date) const;
  AnalysisResult generateWeeklyReport(const Date &startDate) const;
  AnalysisResult generateMonthlyReport(int year, int month) const;

  // 比较分析
  AnalysisResult
  compareStationsFlow(const std::vector<std::string> &stationIds) const;
  AnalysisResult comparePeriodsFlow(const Date &period1Start,
                                    const Date &period1End,
                                    const Date &period2Start,
                                    const Date &period2End) const;

  // 热点分析
  std::vector<std::string> findPeakHours(const std::string &stationId,
                                         const Date &date) const;
  std::vector<std::string> findBusiestStations(int topN = 10) const;
  std::vector<std::string> findBusiestRoutes(int topN = 5) const;

  // 数据导出
  std::string exportAnalysisToText(const AnalysisResult &result) const;
  std::string exportChartToText(const ChartData &chart) const;
  std::string generateFullReport() const;

  // 统计信息
  int getTotalStations() const { return static_cast<int>(stations.size()); }
  int getTotalRoutes() const { return static_cast<int>(routes.size()); }
  int getTotalTrains() const { return static_cast<int>(trains.size()); }

  // 辅助方法
  bool isValidStationId(const std::string &stationId) const;
  bool isValidRouteId(const std::string &routeId) const;
  bool isValidTrainId(const std::string &trainId) const;
  std::shared_ptr<Station> findStation(const std::string &stationId) const;
  std::shared_ptr<Route> findRoute(const std::string &routeId) const;
  std::shared_ptr<Train> findTrain(const std::string &trainId) const;
};

#endif // DATAANALYZER_H