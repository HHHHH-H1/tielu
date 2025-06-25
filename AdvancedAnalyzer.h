#ifndef ADVANCEDANALYZER_H
#define ADVANCEDANALYZER_H

#include "DataAnalyzer.h"
#include "PassengerFlow.h"
#include "Station.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

// 注意：TimeSeriesForecast、ClusterResult、StationCorrelation
// 这些结构体已在 DataAnalyzer.h 中定义

// 模式挖掘结果
struct PatternResult {
  std::string patternType;
  std::map<std::string, double> patterns;
  std::string insights;
  std::vector<std::string> recommendations;

  PatternResult(const std::string &type = "") : patternType(type) {}
};

// 高级分析器类
class AdvancedAnalyzer {
private:
  std::vector<std::shared_ptr<Station>> stations;
  std::shared_ptr<PassengerFlow> passengerFlow;

public:
  // 构造函数
  AdvancedAnalyzer();
  AdvancedAnalyzer(std::shared_ptr<PassengerFlow> flow);

  // 析构函数
  ~AdvancedAnalyzer();

  // 数据管理
  void addStation(std::shared_ptr<Station> station);
  void setPassengerFlow(std::shared_ptr<PassengerFlow> flow);

  // ========== 高级时间序列预测 ==========

  // ARIMA模型预测
  TimeSeriesForecast forecastARIMA(const std::string &stationId, int days = 7,
                                   int p = 1, int d = 1, int q = 1) const;

  // 指数平滑预测
  TimeSeriesForecast forecastExponentialSmoothing(const std::string &stationId,
                                                  int days = 7,
                                                  double alpha = 0.3) const;

  // 季节性分解预测
  TimeSeriesForecast forecastSeasonalDecomposition(const std::string &stationId,
                                                   int days = 7,
                                                   int seasonPeriod = 7) const;

  // 线性回归预测
  TimeSeriesForecast forecastLinearRegression(const std::string &stationId,
                                              int days = 7) const;

  // 多项式回归预测
  TimeSeriesForecast forecastPolynomialRegression(const std::string &stationId,
                                                  int days = 7,
                                                  int degree = 2) const;

  // 比较预测方法
  AnalysisResult compareTimeSeriesMethods(const std::string &stationId,
                                          int days = 7) const;

  // ========== 聚类分析 ==========

  // K-means聚类 - 按客流量聚类站点
  ClusterResult clusterStationsByFlow(int k = 3) const;

  // 按出行模式聚类
  ClusterResult clusterByTravelPatterns(int k = 4) const;

  // 按时间模式聚类
  ClusterResult clusterByTimePatterns(int k = 5) const;

  // 层次聚类
  ClusterResult hierarchicalClustering(int k = 3) const;

  // 聚类特征分析
  AnalysisResult
  analyzeClusterCharacteristics(const ClusterResult &result) const;

  // 最优聚类数确定
  AnalysisResult findOptimalClusterNumber(int maxK = 8) const;

  // ========== 客流时空分布规律挖掘 ==========

  // 时间模式挖掘
  PatternResult mineTemporalPatterns() const;

  // 空间模式挖掘
  PatternResult mineSpatialPatterns() const;

  // 时空模式挖掘
  PatternResult mineSpatio_TemporalPatterns() const;

  // 周期性模式检测
  PatternResult detectPeriodicPatterns() const;

  // 异常检测
  AnalysisResult identifyFlowAnomalies() const;

  // 趋势分析
  PatternResult analyzeTrends() const;

  // ========== 站点关联性分析 ==========

  // 站点相关性分析
  StationCorrelation analyzeStationCorrelations() const;

  // 换乘效率分析
  AnalysisResult analyzeTransferEfficiency() const;

  // 换乘引导策略优化
  AnalysisResult optimizeTransferGuidance() const;

  // 网络中心性分析
  AnalysisResult analyzeNetworkCentrality() const;

  // 网络韧性分析
  AnalysisResult analyzeNetworkResilience() const;

  // 关键节点识别
  AnalysisResult identifyCriticalNodes() const;

  // ========== 模型验证与评估 ==========

  // 预测模型验证
  AnalysisResult validatePredictionModel(const std::string &method,
                                         int testDays = 3) const;

  // 交叉验证
  AnalysisResult crossValidateModels() const;

  // 模型准确性计算
  AnalysisResult calculateModelAccuracy() const;

  // 模型性能报告
  AnalysisResult generateModelPerformanceReport() const;

  // 预测置信度评估
  AnalysisResult assessPredictionConfidence() const;

  // ========== 数据驱动决策支持 ==========

  // 运力配置优化
  AnalysisResult generateCapacityOptimization() const;

  // 时刻表优化
  AnalysisResult generateScheduleOptimization() const;

  // 服务改进建议
  AnalysisResult generateServiceRecommendations() const;

  // 投资优先级分析
  AnalysisResult generateInvestmentPriorities() const;

  // 应急响应策略
  AnalysisResult generateEmergencyResponseStrategy() const;

  // 客流疏导建议
  AnalysisResult generateFlowGuidanceStrategy() const;

private:
  // ========== 私有辅助方法 ==========

  // 数据获取
  std::vector<double> getStationTimeSeriesData(const std::string &stationId,
                                               int days = 30) const;
  std::vector<std::vector<double>> extractStationFeatures() const;

  // 统计计算
  double calculateMean(const std::vector<double> &data) const;
  double calculateStandardDeviation(const std::vector<double> &data) const;
  double calculateCorrelation(const std::vector<double> &x,
                              const std::vector<double> &y) const;
  double calculateMAPE(const std::vector<double> &actual,
                       const std::vector<double> &predicted) const;

  // 时间序列处理
  std::vector<double> calculateMovingAverage(const std::vector<double> &data,
                                             int window) const;
  std::vector<double>
  calculateExponentialSmoothing(const std::vector<double> &data,
                                double alpha) const;
  std::vector<double> differenceData(const std::vector<double> &data,
                                     int d) const;
  std::vector<double> calculateAutocorrelation(const std::vector<double> &data,
                                               int maxLag) const;

  // 回归分析
  std::pair<double, double>
  linearRegression(const std::vector<double> &x,
                   const std::vector<double> &y) const;
  std::vector<double> polynomialRegression(const std::vector<double> &x,
                                           const std::vector<double> &y,
                                           int degree) const;

  // 聚类算法
  ClusterResult performKMeans(const std::vector<std::vector<double>> &data,
                              int k, int maxIterations = 100) const;
  ClusterResult
  performHierarchicalClustering(const std::vector<std::vector<double>> &data,
                                int k) const;
  double calculateSilhouetteScore(const std::vector<std::vector<double>> &data,
                                  const std::vector<int> &labels) const;
  double calculateDistance(const std::vector<double> &a,
                           const std::vector<double> &b) const;

  // ARIMA模型
  TimeSeriesForecast fitARIMA(const std::vector<double> &data, int p, int d,
                              int q, int forecastSteps) const;

  // 模式识别
  std::vector<int> findPeaks(const std::vector<double> &data) const;
  double calculatePeriodicity(const std::vector<double> &data) const;

  // 异常检测
  std::vector<int> detectAnomalies(const std::vector<double> &data,
                                   double threshold = 2.0) const;

  // 验证指标
  double calculateR2(const std::vector<double> &actual,
                     const std::vector<double> &predicted) const;
  double calculateMAE(const std::vector<double> &actual,
                      const std::vector<double> &predicted) const;
  double calculateRMSE(const std::vector<double> &actual,
                       const std::vector<double> &predicted) const;
};

#endif // ADVANCEDANALYZER_H