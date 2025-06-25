#ifndef TIMESERIESANALYZER_H
#define TIMESERIESANALYZER_H

#include "PassengerFlow.h"
#include "Station.h"
#include <map>
#include <memory>
#include <string>
#include <vector>


// 预测结果结构
struct PredictionResult {
  std::vector<double> values; // 预测值
  std::vector<double> upper;  // 置信区间上界
  std::vector<double> lower;  // 置信区间下界
  double accuracy;            // 准确度指标
  std::string algorithmName;  // 算法名称

  PredictionResult() : accuracy(0.0) {}
};

// 聚类分析结果
struct ClusterAnalysis {
  std::vector<std::vector<std::string>> groups; // 聚类分组
  std::vector<std::string> characteristics;     // 每组特征
  double quality;                               // 聚类质量

  ClusterAnalysis() : quality(0.0) {}
};

// 高级时间序列分析器
class TimeSeriesAnalyzer {
private:
  std::shared_ptr<PassengerFlow> passengerFlow;
  std::vector<std::shared_ptr<Station>> stations;

public:
  // 构造函数
  TimeSeriesAnalyzer();
  explicit TimeSeriesAnalyzer(std::shared_ptr<PassengerFlow> flow);

  // 设置数据
  void setPassengerFlow(std::shared_ptr<PassengerFlow> flow);
  void addStation(std::shared_ptr<Station> station);

  // ========== 高级时间序列预测 ==========

  // ARIMA模型预测
  PredictionResult predictWithARIMA(const std::string &stationId, int days = 7);

  // 指数平滑预测
  PredictionResult predictWithExponentialSmoothing(const std::string &stationId,
                                                   int days = 7);

  // 季节性分解预测
  PredictionResult
  predictWithSeasonalDecomposition(const std::string &stationId, int days = 7);

  // 线性趋势预测
  PredictionResult predictWithLinearTrend(const std::string &stationId,
                                          int days = 7);

  // 神经网络预测（简化版）
  PredictionResult predictWithNeuralNetwork(const std::string &stationId,
                                            int days = 7);

  // 组合预测方法
  PredictionResult predictWithEnsemble(const std::string &stationId,
                                       int days = 7);

  // ========== 聚类分析 ==========

  // 基于客流模式的K-means聚类
  ClusterAnalysis clusterStationsByFlowPattern(int k = 3);

  // 基于时间模式的聚类
  ClusterAnalysis clusterStationsByTimePattern(int k = 4);

  // 基于地理位置的聚类
  ClusterAnalysis clusterStationsByLocation(int k = 3);

  // 层次聚类分析
  ClusterAnalysis performHierarchicalClustering(int k = 3);

  // ========== 模式挖掘 ==========

  // 挖掘时间模式
  std::map<std::string, double> mineTemporalPatterns();

  // 挖掘空间模式
  std::map<std::string, double> mineSpatialPatterns();

  // 发现异常模式
  std::vector<std::string> detectAnomalousPatterns();

  // 周期性检测
  std::map<std::string, int> detectPeriodicity();

  // ========== 关联性分析 ==========

  // 站点间相关性分析
  std::map<std::pair<std::string, std::string>, double>
  analyzeStationCorrelations();

  // 换乘站点效率分析
  std::map<std::string, double> analyzeTransferEfficiency();

  // 网络中心性分析
  std::map<std::string, double> analyzeNetworkCentrality();

  // ========== 验证与评估 ==========

  // 模型验证
  std::map<std::string, double> validatePredictionModels();

  // 交叉验证
  std::map<std::string, double> performCrossValidation();

  // 准确性评估
  std::map<std::string, double> assessPredictionAccuracy();

  // ========== 决策支持 ==========

  // 运力优化建议
  std::vector<std::string> generateCapacityRecommendations();

  // 调度优化建议
  std::vector<std::string> generateScheduleRecommendations();

  // 投资优先级建议
  std::vector<std::string> generateInvestmentPriorities();

  // 应急预案建议
  std::vector<std::string> generateEmergencyPlans();

private:
  // 辅助方法
  std::vector<double> getTimeSeriesData(const std::string &stationId, int days);
  std::vector<double> normalizeData(const std::vector<double> &data);
  double calculateAccuracy(const std::vector<double> &actual,
                           const std::vector<double> &predicted);
  std::vector<std::vector<double>> extractFeatures();
  std::vector<int>
  performKMeansCluster(const std::vector<std::vector<double>> &data, int k);
};

#endif // TIMESERIESANALYZER_H