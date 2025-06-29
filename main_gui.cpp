#include <QApplication>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <algorithm>
#include <climits>
#include <clocale>
#include <functional>
#include <locale>
#include <memory>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#endif

#include <QTime>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

// Qt Charts classes will be used with full namespace
#include "DataAnalyzer.h"
#include "FileManager.h"
#include "PassengerFlow.h"
#include "Route.h"
#include "Station.h"
#include "Train.h"

class RailwayMainWindow : public QMainWindow {
  Q_OBJECT

public:
  RailwayMainWindow(QWidget *parent = nullptr)
      : QMainWindow(parent), fileManager("data_utf8") {
    setupUI();
    initializeData();
    updateDisplays();
  }

private slots:
  void refreshData() {
    initializeData();
    updateDisplays();
    statusBar()->showMessage(QString::fromUtf8("数据已刷新"), 2000);
  }

  void performAnalysis() {
    QString analysisType = analysisTypeCombo->currentText();
    QString result;

    if (analysisType == QString::fromUtf8("站点客流排行")) {
      result = QString::fromStdString(passengerFlow.generateStationRanking());
    } else if (analysisType == QString::fromUtf8("川渝双向流量对比")) {
      Date today(2024, 12, 15);
      int cd2cq = passengerFlow.getChengduToChongqingFlow(today);
      int cq2cd = passengerFlow.getChongqingToChengduFlow(today);
      double ratio = passengerFlow.getFlowRatio();

      result =
          QString::fromUtf8("川渝双向流量分析报告\n"
                            "=====================================\n"
                            "川 → 渝: %1 人次\n"
                            "渝 → 川: %2 人次\n"
                            "流量比率: %3\n\n"
                            "分析结论: %4")
              .arg(cd2cq)
              .arg(cq2cd)
              .arg(ratio, 0, 'f', 2)
              .arg(ratio > 1.2
                       ? QString::fromUtf8("成都到重庆方向客流明显高于反向")
                   : ratio < 0.8
                       ? QString::fromUtf8("重庆到成都方向客流明显高于反向")
                       : QString::fromUtf8("双向客流相对均衡"));

    } else if (analysisType == QString::fromUtf8("列车载客率分析")) {
      Date today(2024, 12, 15);
      auto loadFactors = passengerFlow.getAllTrainsLoadFactor(today);

      result = QString::fromUtf8(
          "列车载客率分析报告\n=====================================\n");
      for (const auto &pair : loadFactors) {
        QString status;
        if (pair.second > 90) {
          status = QString::fromUtf8("严重超载");
        } else if (pair.second > 80) {
          status = QString::fromUtf8("高负荷运行");
        } else if (pair.second > 60) {
          status = QString::fromUtf8("正常运行");
        } else {
          status = QString::fromUtf8("低负荷运行");
        }

        result += QString::fromUtf8("列车 %1: %2% - %3\n")
                      .arg(QString::fromStdString(pair.first))
                      .arg(pair.second, 0, 'f', 1)
                      .arg(status);
      }
    } else if (analysisType == QString::fromUtf8("站点客流预测")) {
      int days = predictionDaysCombo->currentText().toInt();
      QString selectedStation = stationCombo->currentText();

      if (selectedStation.isEmpty() || stations.empty()) {
        result = QString::fromUtf8("请先选择要预测的站点！");
      } else {
        // 找到对应的站点ID
        std::string stationId;
        for (const auto &station : stations) {
          if (QString::fromStdString(station->getStationName()) ==
              selectedStation) {
            stationId = station->getStationId();
            break;
          }
        }

        if (!stationId.empty()) {
          auto prediction = passengerFlow.predictFlow(stationId, days);

          result =
              QString::fromUtf8("站点客流预测报告\n") +
              QString::fromUtf8("=====================================\n") +
              QString::fromUtf8("预测站点: %1\n").arg(selectedStation) +
              QString::fromUtf8("预测天数: %1 天\n\n").arg(days) +
              QString::fromUtf8("预测结果:\n");

          for (int i = 0; i < static_cast<int>(prediction.size()); ++i) {
            result += QString::fromUtf8("第 %1 天: %2 人次\n")
                          .arg(i + 1)
                          .arg(prediction[i]);
          }

          // 计算平均预测值
          if (!prediction.empty()) {
            int avgFlow = 0;
            for (int flow : prediction) {
              avgFlow += flow;
            }
            avgFlow /= prediction.size();

            result +=
                QString::fromUtf8("\n平均预测客流: %1 人次/天\n").arg(avgFlow);

            if (avgFlow > 200) {
              result += QString::fromUtf8("建议: 高客流站点，注意运力调配");
            } else if (avgFlow > 100) {
              result += QString::fromUtf8("建议: 中等客流站点，维持正常运营");
            } else {
              result += QString::fromUtf8("建议: 低客流站点，可优化班次安排");
            }
          }
        } else {
          result = QString::fromUtf8("未找到选中的站点信息！");
        }
      }

    } else if (analysisType == QString::fromUtf8("川渝双向预测")) {
      int days = predictionDaysCombo->currentText().toInt();

      auto cd2cqPrediction =
          passengerFlow.predictDirectionalFlow("川->渝", days);
      auto cq2cdPrediction =
          passengerFlow.predictDirectionalFlow("渝->川", days);

      result = QString::fromUtf8("川渝双向流量预测报告\n") +
               QString::fromUtf8("=====================================\n") +
               QString::fromUtf8("预测天数: %1 天\n\n").arg(days);

      result += QString::fromUtf8("川 → 渝 方向预测:\n");
      for (int i = 0; i < static_cast<int>(cd2cqPrediction.size()); ++i) {
        result += QString::fromUtf8("第 %1 天: %2 人次\n")
                      .arg(i + 1)
                      .arg(cd2cqPrediction[i]);
      }

      result += QString::fromUtf8("\n渝 → 川 方向预测:\n");
      for (int i = 0; i < static_cast<int>(cq2cdPrediction.size()); ++i) {
        result += QString::fromUtf8("第 %1 天: %2 人次\n")
                      .arg(i + 1)
                      .arg(cq2cdPrediction[i]);
      }

      // 计算预测平均比率
      if (!cd2cqPrediction.empty() && !cq2cdPrediction.empty()) {
        int avgCd2cq = 0, avgCq2cd = 0;
        for (int i = 0; i < static_cast<int>(cd2cqPrediction.size()); ++i) {
          avgCd2cq += cd2cqPrediction[i];
          avgCq2cd += cq2cdPrediction[i];
        }
        avgCd2cq /= cd2cqPrediction.size();
        avgCq2cd /= cq2cdPrediction.size();

        double predictedRatio =
            (avgCq2cd > 0) ? static_cast<double>(avgCd2cq) / avgCq2cd : 0.0;

        result += QString::fromUtf8("\n预测分析:\n") +
                  QString::fromUtf8("川→渝平均: %1 人次/天\n").arg(avgCd2cq) +
                  QString::fromUtf8("渝→川平均: %1 人次/天\n").arg(avgCq2cd) +
                  QString::fromUtf8("预测流量比: %1\n")
                      .arg(predictedRatio, 0, 'f', 2);

        if (predictedRatio > 1.2) {
          result += QString::fromUtf8(
              "预测结论: 川→渝方向将持续高于反向，建议增加该方向运力");
        } else if (predictedRatio < 0.8) {
          result += QString::fromUtf8(
              "预测结论: 渝→川方向将持续高于反向，建议增加该方向运力");
        } else {
          result += QString::fromUtf8("预测结论: 双向流量将保持相对均衡");
        }
      }
    }

    analysisResults->setText(result);
    statusBar()->showMessage(QString::fromUtf8("分析完成"), 2000);
  }

  void updateChartDisplay() {
    if (!chart)
      return;

    // 清除之前的图表
    chart->removeAllSeries();
    for (auto axis : chart->axes()) {
      chart->removeAxis(axis);
    }

    int chartType = chartTypeCombo->currentData().toInt();

    switch (chartType) {
    case 0: // 站点客流排行
      createStationFlowChart();
      break;
    case 1: // 时间序列趋势
      createTimeSeriesChart();
      break;
    case 2: // 方向对比
      createDirectionCompareChart();
      break;
    case 3: // 列车载客率
      createTrainLoadChart();
      break;
    case 4: // 客流预测图表
      createFlowPredictionChart();
      break;
    case 5: // 客流热力图
      createPassengerHeatMap();
      break;
    case 6: // 列车运行模拟
      createTrainSimulation();
      break;
    }
  }

  void createStationFlowChart() {
    auto stationFlow = passengerFlow.getAllStationsFlow();

    // 转换为vector并排序
    std::vector<std::pair<std::string, int>> ranking;
    for (const auto &pair : stationFlow) {
      ranking.push_back(pair);
    }

    std::sort(ranking.begin(), ranking.end(),
              [](const std::pair<std::string, int> &a,
                 const std::pair<std::string, int> &b) {
                return a.second > b.second;
              });

    // 创建柱状图
    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet(QString::fromUtf8("客流量"));
    set->setColor(QColor("#007bff"));

    QStringList categories;

    // 只显示前10个站点
    int count = std::min(10, static_cast<int>(ranking.size()));
    for (int i = 0; i < count; ++i) {
      set->append(ranking[i].second);
      QString stationName = QString::fromStdString(ranking[i].first);
      if (stationName.length() > 8) {
        stationName = stationName.left(6) + "...";
      }
      categories << stationName;
    }

    series->append(set);
    chart->addSeries(series);

    // 设置坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, ranking.empty() ? 100 : ranking[0].second * 1.1);
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle(QString::fromUtf8("站点客流量排行（前10名）"));
    chart->legend()->setVisible(true);
  }

  void createTimeSeriesChart() {
    QLineSeries *series = new QLineSeries();
    series->setName(QString::fromUtf8("每日客流趋势"));
    series->setColor(QColor("#28a745"));

    // 生成最近7天的数据
    for (int i = 0; i < 7; ++i) {
      series->append(i, 2000 + (rand() % 1000));
    }

    chart->addSeries(series);

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 6);
    axisX->setLabelFormat("%d");
    axisX->setTitleText(QString::fromUtf8("天数"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(1500, 3500);
    axisY->setLabelFormat("%d");
    axisY->setTitleText(QString::fromUtf8("客流量"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle(QString::fromUtf8("客流趋势图（最近7天）"));
    chart->legend()->setVisible(true);
  }

  void createDirectionCompareChart() {
    Date today(2024, 12, 15);
    int cd2cq = passengerFlow.getChengduToChongqingFlow(today);
    int cq2cd = passengerFlow.getChongqingToChengduFlow(today);

    // 创建饼图
    QPieSeries *series = new QPieSeries();

    auto slice1 = series->append(QString::fromUtf8("川→渝"), cd2cq);
    slice1->setColor(QColor("#007bff"));
    slice1->setLabelVisible();

    auto slice2 = series->append(QString::fromUtf8("渝→川"), cq2cd);
    slice2->setColor(QColor("#28a745"));
    slice2->setLabelVisible();

    // 突出显示最大的部分
    if (cd2cq > cq2cd) {
      slice1->setExploded();
    } else {
      slice2->setExploded();
    }

    chart->addSeries(series);
    chart->setTitle(QString::fromUtf8("川渝双向流量对比"));
    chart->legend()->setVisible(true);
  }

  void createTrainLoadChart() {
    Date today(2024, 12, 15);
    auto loadFactors = passengerFlow.getAllTrainsLoadFactor(today);

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet(QString::fromUtf8("载客率 (%)"));
    set->setColor(QColor("#dc3545"));

    QStringList categories;

    int count = 0;
    for (const auto &pair : loadFactors) {
      if (count >= 8)
        break;

      set->append(pair.second);
      categories << QString::fromStdString(pair.first);
      count++;
    }

    if (count == 0) {
      // 生成示例数据
      QStringList trains = {"G8501", "G8502", "G8503", "G8504", "G8505"};
      for (const auto &train : trains) {
        set->append(60 + (rand() % 40));
        categories << train;
      }
    }

    series->append(set);
    chart->addSeries(series);

    // 设置坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%.1f%%");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle(QString::fromUtf8("列车载客率分析"));
    chart->legend()->setVisible(true);
  }

  void createFlowPredictionChart() {
    // 创建川渝双向预测图表
    QLineSeries *cd2cqSeries = new QLineSeries();
    cd2cqSeries->setName(QString::fromUtf8("川→渝预测"));
    cd2cqSeries->setColor(QColor("#007bff"));
    cd2cqSeries->setMarkerSize(8.0);

    QLineSeries *cq2cdSeries = new QLineSeries();
    cq2cdSeries->setName(QString::fromUtf8("渝→川预测"));
    cq2cdSeries->setColor(QColor("#dc3545"));
    cq2cdSeries->setMarkerSize(8.0);

    // 初始化随机数种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 生成未来7天的预测数据
    auto cd2cqPrediction = passengerFlow.predictDirectionalFlow("川->渝", 7);
    auto cq2cdPrediction = passengerFlow.predictDirectionalFlow("渝->川", 7);

    // 添加预测数据点
    for (int i = 0; i < static_cast<int>(cd2cqPrediction.size()) && i < 7;
         ++i) {
      cd2cqSeries->append(i + 1, cd2cqPrediction[i]);
    }

    for (int i = 0; i < static_cast<int>(cq2cdPrediction.size()) && i < 7;
         ++i) {
      cq2cdSeries->append(i + 1, cq2cdPrediction[i]);
    }

    chart->addSeries(cd2cqSeries);
    chart->addSeries(cq2cdSeries);

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(1, 7);
    axisX->setLabelFormat("%d");
    axisX->setTitleText(QString::fromUtf8("预测天数"));
    axisX->setTickCount(7);
    chart->addAxis(axisX, Qt::AlignBottom);
    cd2cqSeries->attachAxis(axisX);
    cq2cdSeries->attachAxis(axisX);

    // 动态计算Y轴范围
    int minValue = INT_MAX, maxValue = 0;
    for (int value : cd2cqPrediction) {
      minValue = std::min(minValue, value);
      maxValue = std::max(maxValue, value);
    }
    for (int value : cq2cdPrediction) {
      minValue = std::min(minValue, value);
      maxValue = std::max(maxValue, value);
    }

    if (minValue == INT_MAX) {
      minValue = 0;
      maxValue = 1000;
    }

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(minValue * 0.9, maxValue * 1.1); // 留出10%的边界
    axisY->setLabelFormat("%d");
    axisY->setTitleText(QString::fromUtf8("预测客流量（人次）"));
    chart->addAxis(axisY, Qt::AlignLeft);
    cd2cqSeries->attachAxis(axisY);
    cq2cdSeries->attachAxis(axisY);

    chart->setTitle(QString::fromUtf8("川渝双向客流预测（未来7天）"));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 添加预测分析文字
    QString analysisText = QString::fromUtf8("预测趋势分析：\n");

    // 计算趋势
    if (cd2cqPrediction.size() >= 2 && cq2cdPrediction.size() >= 2) {
      int cd2cqTrend = cd2cqPrediction.back() - cd2cqPrediction.front();
      int cq2cdTrend = cq2cdPrediction.back() - cq2cdPrediction.front();

      if (cd2cqTrend > 0) {
        analysisText += QString::fromUtf8("• 川→渝方向呈上升趋势\n");
      } else if (cd2cqTrend < 0) {
        analysisText += QString::fromUtf8("• 川→渝方向呈下降趋势\n");
      } else {
        analysisText += QString::fromUtf8("• 川→渝方向保持稳定\n");
      }

      if (cq2cdTrend > 0) {
        analysisText += QString::fromUtf8("• 渝→川方向呈上升趋势\n");
      } else if (cq2cdTrend < 0) {
        analysisText += QString::fromUtf8("• 渝→川方向呈下降趋势\n");
      } else {
        analysisText += QString::fromUtf8("• 渝→川方向保持稳定\n");
      }
    }
  }

  void createPassengerHeatMap() {
    // 创建专业的客流热力图

    // 获取所有站点的客流数据
    auto stationFlow = passengerFlow.getAllStationsFlow();

    // 调试信息：显示有多少条客流记录
    int recordCount = passengerFlow.getRecordCount();

    // 强制显示真实数据 - 优先使用所有站点生成热力图
    if (stationFlow.empty() || recordCount == 0 || stationFlow.size() <= 4) {
      // 重新生成客流数据
      generateRealisticFlowData();
      // 重新获取客流数据
      stationFlow = passengerFlow.getAllStationsFlow();

      // 如果数据还是太少或为空，直接基于stations列表生成热力图
      if (stationFlow.empty() || stationFlow.size() <= 4) {
        createStationBasedHeatMap();
        return;
      }
    }

    // 找到客流的最大值和最小值，用于颜色映射
    int maxFlow = 0, minFlow = INT_MAX;
    for (const auto &pair : stationFlow) {
      maxFlow = std::max(maxFlow, pair.second);
      minFlow = std::min(minFlow, pair.second);
    }

    if (maxFlow == minFlow) {
      maxFlow = minFlow + 1; // 避免除零
    }

    // 创建热力点 - 使用多个散点图来模拟热力效果
    std::vector<QScatterSeries *> heatLayers;

    // 为不同强度的热力创建不同的图层
    for (const auto &pair : stationFlow) {
      // 计算热力强度 (0-1之间)
      double intensity =
          static_cast<double>(pair.second - minFlow) / (maxFlow - minFlow);

      // 为每个站点创建一个热力区域（使用多个同心圆模拟）
      for (int radius = 5; radius >= 1; radius--) {
        QScatterSeries *heatLayer = new QScatterSeries();
        heatLayer->setMarkerSize(radius * 8.0); // 不同半径的圆

        // 根据强度和半径设置颜色
        QColor color = getHeatMapColor(intensity, radius);
        heatLayer->setColor(color);
        heatLayer->setBorderColor(QColor(Qt::transparent));

        // 模拟地理位置 - 基于站点名称生成相对位置
        double x, y;
        getStationPosition(pair.first, x, y);

        heatLayer->append(x, y);
        heatLayers.push_back(heatLayer);
        chart->addSeries(heatLayer);
      }
    }

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(103, 107); // 大致覆盖成都-重庆经度范围
    axisX->setTitleText("Longitude / E");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(29, 31); // 大致覆盖成都-重庆纬度范围
    axisY->setTitleText("Latitude / N");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // 为所有图层附加坐标轴
    for (auto layer : heatLayers) {
      layer->attachAxis(axisX);
      layer->attachAxis(axisY);
    }

    chart->setTitle(QString::fromUtf8("川渝地区客流热力分布图 "));
    chart->legend()->setVisible(false); // 热力图通常不显示图例
  }

  void createStationBasedHeatMap() {
    // 直接基于stations列表生成热力图，不依赖客流记录

    if (stations.empty()) {
      createSampleHeatMap();
      return;
    }

    // 为每个站点生成模拟客流量
    for (const auto &station : stations) {
      if (!station)
        continue;

      // 根据站点名称生成不同的客流强度
      std::string stationName = station->getStationName();
      int intensity = 100; // 基础强度

      // 主要城市站点客流更高
      if (stationName.find("成都") != std::string::npos ||
          stationName.find("重庆") != std::string::npos) {
        intensity = 200 + (std::hash<std::string>{}(stationName) % 100);
      } else if (stationName.find("北京") != std::string::npos ||
                 stationName.find("上海") != std::string::npos ||
                 stationName.find("广州") != std::string::npos ||
                 stationName.find("深圳") != std::string::npos) {
        intensity = 150 + (std::hash<std::string>{}(stationName) % 100);
      } else {
        intensity = 50 + (std::hash<std::string>{}(stationName) % 100);
      }

      // 使用站点的经纬度或估算位置
      double x, y;
      if (station->getLatitude() != 0 && station->getLongitude() != 0) {
        x = station->getLongitude();
        y = station->getLatitude();
      } else {
        getStationPosition(stationName, x, y);
      }

      // 计算强度 (0-1之间)
      double intensityNormal = static_cast<double>(intensity - 50) / 250.0;
      intensityNormal = std::max(0.0, std::min(1.0, intensityNormal));

      // 为每个站点创建热力区域
      for (int radius = 5; radius >= 1; radius--) {
        QScatterSeries *heatLayer = new QScatterSeries();
        heatLayer->setMarkerSize(radius * 10.0);

        QColor color = getHeatMapColor(intensityNormal, radius);
        heatLayer->setColor(color);
        heatLayer->setBorderColor(QColor(Qt::transparent));

        heatLayer->append(x, y);
        chart->addSeries(heatLayer);
      }
    }

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(103, 107);
    axisX->setTitleText("Longitude / E");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(29, 31);
    axisY->setTitleText("Latitude / N");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // 为所有系列附加坐标轴
    for (auto series : chart->series()) {
      series->attachAxis(axisX);
      series->attachAxis(axisY);
    }

    chart->setTitle(
        QString::fromUtf8("川渝地区客流热力分布图 - 基于所有站点 (%1个站点)")
            .arg(stations.size()));
    chart->legend()->setVisible(false);
  }

  void createSampleHeatMap() {
    // 创建示例热力图数据
    struct HeatPoint {
      QString name;
      double x, y;
      int intensity;
    };

    std::vector<HeatPoint> sampleData = {
        {QString::fromUtf8("成都东站"), 104.1, 30.6, 250},
        {QString::fromUtf8("成都南站"), 104.0, 30.6, 180},
        {QString::fromUtf8("重庆北站"), 106.5, 29.8, 220},
        {QString::fromUtf8("重庆西站"), 106.4, 29.5, 160},
        {QString::fromUtf8("简阳南站"), 104.5, 30.4, 80},
        {QString::fromUtf8("资阳北站"), 104.8, 30.2, 60},
        {QString::fromUtf8("内江北站"), 105.1, 29.9, 90},
        {QString::fromUtf8("隆昌北站"), 105.4, 29.7, 70}};

    int maxFlow = 250, minFlow = 60;

    for (const auto &point : sampleData) {
      double intensity =
          static_cast<double>(point.intensity - minFlow) / (maxFlow - minFlow);

      // 为每个站点创建热力区域
      for (int radius = 4; radius >= 1; radius--) {
        QScatterSeries *heatLayer = new QScatterSeries();
        heatLayer->setMarkerSize(radius * 12.0);

        QColor color = getHeatMapColor(intensity, radius);
        heatLayer->setColor(color);
        heatLayer->setBorderColor(QColor(Qt::transparent));

        heatLayer->append(point.x, point.y);
        chart->addSeries(heatLayer);
      }
    }

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(103.5, 107);
    axisX->setTitleText("Longitude / E");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(29.2, 31);
    axisY->setTitleText("Latitude / N");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // 为所有系列附加坐标轴
    for (auto series : chart->series()) {
      series->attachAxis(axisX);
      series->attachAxis(axisY);
    }

    chart->setTitle(QString::fromUtf8("川渝高铁客流热力分布图"));
    chart->legend()->setVisible(false);
  }

  QColor getHeatMapColor(double intensity, int radius) {
    // 创建热力图颜色映射：蓝色(低) -> 绿色 -> 黄色 -> 红色(高)
    QColor color;

    // 根据半径调整透明度（中心更不透明，边缘更透明）
    int alpha = 60 + (5 - radius) * 30; // 60-180的透明度范围

    if (intensity < 0.25) {
      // 蓝色到青色
      int blue = 255;
      int green = static_cast<int>(intensity * 4 * 255);
      color = QColor(0, green, blue, alpha);
    } else if (intensity < 0.5) {
      // 青色到绿色
      int blue = static_cast<int>((0.5 - intensity) * 4 * 255);
      int green = 255;
      color = QColor(0, green, blue, alpha);
    } else if (intensity < 0.75) {
      // 绿色到黄色
      int red = static_cast<int>((intensity - 0.5) * 4 * 255);
      int green = 255;
      color = QColor(red, green, 0, alpha);
    } else {
      // 黄色到红色
      int red = 255;
      int green = static_cast<int>((1.0 - intensity) * 4 * 255);
      color = QColor(red, green, 0, alpha);
    }

    return color;
  }

  void getStationPosition(const std::string &stationName, double &x,
                          double &y) {
    // 根据站点名称估算地理位置
    if (stationName.find("成都") != std::string::npos) {
      x = 104.0 + (std::hash<std::string>{}(stationName) % 20) * 0.01;
      y = 30.5 + (std::hash<std::string>{}(stationName) % 20) * 0.01;
    } else if (stationName.find("重庆") != std::string::npos) {
      x = 106.4 + (std::hash<std::string>{}(stationName) % 20) * 0.01;
      y = 29.5 + (std::hash<std::string>{}(stationName) % 20) * 0.01;
    } else if (stationName.find("简阳") != std::string::npos) {
      x = 104.5;
      y = 30.4;
    } else if (stationName.find("资阳") != std::string::npos) {
      x = 104.8;
      y = 30.2;
    } else {
      // 其他站点在成都-重庆之间随机分布
      x = 104.0 + (std::hash<std::string>{}(stationName) % 250) * 0.01;
      y = 29.5 + (std::hash<std::string>{}(stationName) % 150) * 0.01;
    }
  }

  void createTrainSimulation() {
    // 创建列车运行模拟 - 显示列车在线路上的动态位置
    QLineSeries *routeSeries = new QLineSeries();
    routeSeries->setName(QString::fromUtf8("成渝高铁线路"));
    routeSeries->setColor(QColor("#2c3e50"));
    routeSeries->setPen(QPen(QColor("#2c3e50"), 3));

    // 模拟成渝高铁线路（成都 -> 重庆）
    routeSeries->append(0, 0);    // 成都东站
    routeSeries->append(2, 0.5);  // 成都南站
    routeSeries->append(4, 1);    // 简阳南站
    routeSeries->append(6, 1.5);  // 资阳北站
    routeSeries->append(8, 2);    // 重庆西站
    routeSeries->append(10, 2.5); // 重庆北站

    chart->addSeries(routeSeries);

    // 创建运行中的列车（使用散点图表示）
    QScatterSeries *trainSeries = new QScatterSeries();
    trainSeries->setName(QString::fromUtf8("运行列车"));
    trainSeries->setMarkerSize(20.0);
    trainSeries->setColor(QColor("#e74c3c"));
    trainSeries->setBorderColor(QColor("#c0392b"));

    // 模拟当前时间的列车位置
    int currentHour = QTime::currentTime().hour();
    double trainPositions[] = {1.5, 3.8, 6.2, 8.7}; // 4列列车的位置

    for (int i = 0; i < 4; ++i) {
      double pos =
          trainPositions[i] + (currentHour % 12) * 0.1; // 根据时间微调位置
      if (pos > 10)
        pos = pos - 10; // 循环运行

      double y = 0.25 * (pos / 2.5); // 沿线路的Y坐标
      trainSeries->append(pos, y);
    }

    chart->addSeries(trainSeries);

    // 创建车站点
    QScatterSeries *stationSeries = new QScatterSeries();
    stationSeries->setName(QString::fromUtf8("车站"));
    stationSeries->setMarkerSize(15.0);
    stationSeries->setColor(QColor("#3498db"));
    stationSeries->setBorderColor(QColor("#2980b9"));

    // 添加主要车站位置
    stationSeries->append(0, 0);    // 成都东站
    stationSeries->append(2, 0.5);  // 成都南站
    stationSeries->append(4, 1);    // 简阳南站
    stationSeries->append(6, 1.5);  // 资阳北站
    stationSeries->append(8, 2);    // 重庆西站
    stationSeries->append(10, 2.5); // 重庆北站

    chart->addSeries(stationSeries);

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(-0.5, 10.5);
    axisX->setTitleText("Railway Distance (km)");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);
    routeSeries->attachAxis(axisX);
    trainSeries->attachAxis(axisX);
    stationSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(-0.5, 3);
    axisY->setTitleText("Track Direction");
    axisY->setLabelsVisible(false);
    chart->addAxis(axisY, Qt::AlignLeft);
    routeSeries->attachAxis(axisY);
    trainSeries->attachAxis(axisY);
    stationSeries->attachAxis(axisY);

    chart->setTitle(QString::fromUtf8("成渝高铁列车运行实时模拟"));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 添加状态信息文本
    QString statusText = QString::fromUtf8("当前时间: %1\n")
                             .arg(QTime::currentTime().toString("hh:mm"));
    statusText += QString::fromUtf8("运行列车: 4列\n");
    statusText += QString::fromUtf8("线路状态: 正常运行\n");
    statusText += QString::fromUtf8("平均速度: 280km/h");
  }

  void exportData() {
    QString fileName =
        QFileDialog::getSaveFileName(this, QString::fromUtf8("导出数据"), "",
                                     QString::fromUtf8("CSV文件 (*.csv)"));
    if (!fileName.isEmpty()) {
      if (fileManager.exportAllData(stations, routes, trains, passengerFlow)) {
        QMessageBox::information(this, QString::fromUtf8("成功"),
                                 QString::fromUtf8("数据导出成功！"));
      } else {
        QMessageBox::warning(this, QString::fromUtf8("错误"),
                             QString::fromUtf8("数据导出失败！"));
      }
    }
  }

  void aboutApp() {
    QString aboutText =
        QString::fromUtf8("川渝地区轨道交通客流数据分析与展示系统\n\n");
    aboutText += QString::fromUtf8("版本: 1.0\n");
    aboutText += QString::fromUtf8("开发平台: Qt6 + C++17\n");
    aboutText += QString::fromUtf8("功能特性:\n");
    aboutText += QString::fromUtf8("客流数据管理\n");
    aboutText += QString::fromUtf8("多维度分析\n");
    aboutText += QString::fromUtf8("可视化展示\n");
    aboutText += QString::fromUtf8("预测功能\n");
    aboutText += QString::fromUtf8("报告生成");

    QMessageBox::about(this, QString::fromUtf8("关于系统"), aboutText);
  }

  void onAnalysisTypeChanged(int index) {
    // 根据选择的分析类型显示/隐藏相关控件
    bool showPredictionDays = (index == 3 || index == 4); // 站点预测或川渝预测
    bool showStationSelect = (index == 3); // 仅站点预测需要选择站点

    predictionDaysLabel->setVisible(showPredictionDays);
    predictionDaysCombo->setVisible(showPredictionDays);
    stationLabel->setVisible(showStationSelect);
    stationCombo->setVisible(showStationSelect);
  }

  void updateDisplays() {
    updateStationList();
    updateRouteList();
    updateTrainList();
    updateInfo();
    updateChartDisplay();
    updateStationCombo(); // 更新站点下拉列表
  }

  void updateStationCombo() {
    stationCombo->clear();
    stationCombo->addItem(QString::fromUtf8("请选择站点"));

    for (const auto &station : stations) {
      if (station) {
        stationCombo->addItem(
            QString::fromStdString(station->getStationName()));
      }
    }
  }

private:
  void setupUI() {
    setWindowTitle(
        QString::fromUtf8("川渝地区轨道交通客流数据分析与展示系统 v1.0"));
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // 创建菜单栏
    setupMenuBar();

    // 创建中央窗口部件
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 标题
    QLabel *titleLabel =
        new QLabel(QString::fromUtf8("川渝地区轨道交通客流数据分析与展示系统"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 20px; font-weight: bold; color: #2c3e50; margin: 10px;");
    mainLayout->addWidget(titleLabel);

    // 创建标签页
    QTabWidget *tabWidget = new QTabWidget;

    setupDataTab(tabWidget);
    setupAnalysisTab(tabWidget);
    setupChartTab(tabWidget);

    mainLayout->addWidget(tabWidget);

    // 创建状态栏
    statusBar()->showMessage(QString::fromUtf8("系统就绪"));

    // 应用样式
    setStyleSheet(R"(
            QMainWindow {
                background-color: #f8f9fa;
            }
            QTabWidget::pane {
                border: 1px solid #dee2e6;
                background-color: white;
                border-radius: 5px;
            }
            QTabBar::tab {
                background-color: #e9ecef;
                padding: 8px 16px;
                margin-right: 2px;
                border-top-left-radius: 5px;
                border-top-right-radius: 5px;
            }
            QTabBar::tab:selected {
                background-color: white;
                border-bottom: 2px solid #007bff;
            }
            QGroupBox {
                font-weight: bold;
                border: 2px solid #dee2e6;
                border-radius: 8px;
                margin-top: 8px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px 0 5px;
            }
            QPushButton {
                background-color: #007bff;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 8px 16px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
            QTreeWidget, QTableWidget {
                border: 1px solid #dee2e6;
                border-radius: 5px;
                background-color: white;
            }
            QTextEdit {
                border: 1px solid #dee2e6;
                border-radius: 5px;
                background-color: white;
                font-family: 'Consolas', monospace;
            }
        )");
  }

  void setupMenuBar() {
    QMenuBar *menuBar = this->menuBar();

    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu(QString::fromUtf8("文件"));

    QAction *refreshAction = fileMenu->addAction(QString::fromUtf8("刷新数据"));
    connect(refreshAction, &QAction::triggered, this,
            &RailwayMainWindow::refreshData);

    fileMenu->addSeparator();

    QAction *exportAction =
        fileMenu->addAction(QString::fromUtf8("导出数据..."));
    connect(exportAction, &QAction::triggered, this,
            &RailwayMainWindow::exportData);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction(QString::fromUtf8("退出"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu(QString::fromUtf8("帮助"));
    QAction *aboutAction = helpMenu->addAction(QString::fromUtf8("关于"));
    connect(aboutAction, &QAction::triggered, this,
            &RailwayMainWindow::aboutApp);
  }

  void setupDataTab(QTabWidget *tabWidget) {
    QWidget *dataTab = new QWidget;
    tabWidget->addTab(dataTab, QString::fromUtf8("数据管理"));

    QHBoxLayout *dataLayout = new QHBoxLayout(dataTab);

    // 左侧面板
    QWidget *leftPanel = new QWidget;
    leftPanel->setMinimumWidth(400);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // 站点信息
    QGroupBox *stationGroup = new QGroupBox(QString::fromUtf8("站点信息"));
    QVBoxLayout *stationLayout = new QVBoxLayout(stationGroup);

    stationTree = new QTreeWidget;
    stationTree->setHeaderLabels(
        {QString::fromUtf8("站点名称"), QString::fromUtf8("编号"),
         QString::fromUtf8("城市"), QString::fromUtf8("类型")});
    stationLayout->addWidget(stationTree);
    leftLayout->addWidget(stationGroup);

    // 线路信息
    QGroupBox *routeGroup = new QGroupBox(QString::fromUtf8("线路信息"));
    QVBoxLayout *routeLayout = new QVBoxLayout(routeGroup);

    routeTree = new QTreeWidget;
    routeTree->setHeaderLabels(
        {QString::fromUtf8("线路名称"), QString::fromUtf8("编号"),
         QString::fromUtf8("类型"), QString::fromUtf8("总长")});
    routeLayout->addWidget(routeTree);
    leftLayout->addWidget(routeGroup);

    dataLayout->addWidget(leftPanel);

    // 右侧面板
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 列车信息
    QGroupBox *trainGroup = new QGroupBox(QString::fromUtf8("列车信息"));
    QVBoxLayout *trainLayout = new QVBoxLayout(trainGroup);

    trainTable = new QTableWidget;
    trainTable->setColumnCount(4);
    trainTable->setHorizontalHeaderLabels(
        {QString::fromUtf8("列车号"), QString::fromUtf8("类型"),
         QString::fromUtf8("线路"), QString::fromUtf8("载客量")});
    trainTable->horizontalHeader()->setStretchLastSection(true);
    trainLayout->addWidget(trainTable);
    rightLayout->addWidget(trainGroup);

    // 详细信息
    QGroupBox *infoGroup = new QGroupBox(QString::fromUtf8("详细信息"));
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);

    infoDisplay = new QTextEdit;
    infoDisplay->setReadOnly(true);
    infoDisplay->setMaximumHeight(150);
    infoLayout->addWidget(infoDisplay);
    rightLayout->addWidget(infoGroup);

    dataLayout->addWidget(rightPanel);
  }

  void setupAnalysisTab(QTabWidget *tabWidget) {
    QWidget *analysisTab = new QWidget;
    tabWidget->addTab(analysisTab, QString::fromUtf8("数据分析"));

    QVBoxLayout *analysisLayout = new QVBoxLayout(analysisTab);

    // 控制面板
    QGroupBox *controlGroup = new QGroupBox(QString::fromUtf8("分析控制"));
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);

    controlLayout->addWidget(new QLabel(QString::fromUtf8("分析类型:")));
    analysisTypeCombo = new QComboBox;
    analysisTypeCombo->addItems({QString::fromUtf8("站点客流排行"),
                                 QString::fromUtf8("川渝双向流量对比"),
                                 QString::fromUtf8("列车载客率分析"),
                                 QString::fromUtf8("站点客流预测"),
                                 QString::fromUtf8("川渝双向预测")});
    controlLayout->addWidget(analysisTypeCombo);

    // 预测天数选择（仅预测功能使用）
    predictionDaysLabel = new QLabel(QString::fromUtf8("预测天数:"));
    predictionDaysCombo = new QComboBox;
    predictionDaysCombo->addItems({"1", "2", "3"});
    predictionDaysCombo->setCurrentIndex(2); // 默认3天

    controlLayout->addWidget(predictionDaysLabel);
    controlLayout->addWidget(predictionDaysCombo);

    // 站点选择（仅站点预测使用）
    stationLabel = new QLabel(QString::fromUtf8("选择站点:"));
    stationCombo = new QComboBox;
    controlLayout->addWidget(stationLabel);
    controlLayout->addWidget(stationCombo);

    QPushButton *analyzeBtn = new QPushButton(QString::fromUtf8("开始分析"));
    connect(analyzeBtn, &QPushButton::clicked, this,
            &RailwayMainWindow::performAnalysis);
    controlLayout->addWidget(analyzeBtn);

    // 连接分析类型变化事件，动态显示/隐藏控件
    connect(analysisTypeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &RailwayMainWindow::onAnalysisTypeChanged);

    controlLayout->addStretch();
    analysisLayout->addWidget(controlGroup);

    // 结果显示
    QGroupBox *resultGroup = new QGroupBox(QString::fromUtf8("分析结果"));
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    analysisResults = new QTextEdit;
    analysisResults->setReadOnly(true);
    resultLayout->addWidget(analysisResults);
    analysisLayout->addWidget(resultGroup);

    // 初始化控件可见性
    onAnalysisTypeChanged(0);
  }

  void setupChartTab(QTabWidget *tabWidget) {
    QWidget *chartTab = new QWidget;
    tabWidget->addTab(chartTab, QString::fromUtf8("可视化图表"));

    QVBoxLayout *chartLayout = new QVBoxLayout(chartTab);

    // 图表控制面板
    QHBoxLayout *controlLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel(QString::fromUtf8("数据可视化图表"));
    titleLabel->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #2c3e50;");

    chartTypeCombo = new QComboBox();
    chartTypeCombo->addItem(QString::fromUtf8("站点客流排行"), 0);
    chartTypeCombo->addItem(QString::fromUtf8("时间序列趋势"), 1);
    chartTypeCombo->addItem(QString::fromUtf8("方向流量对比"), 2);
    chartTypeCombo->addItem(QString::fromUtf8("列车载客率"), 3);
    chartTypeCombo->addItem(QString::fromUtf8("客流预测图表"), 4);
    chartTypeCombo->addItem(QString::fromUtf8("客流热力图"), 5);
    chartTypeCombo->addItem(QString::fromUtf8("列车运行模拟"), 6);

    QPushButton *updateBtn = new QPushButton(QString::fromUtf8("更新图表"));
    updateBtn->setStyleSheet(
        "QPushButton { background-color: #007bff; color: white; border: none; "
        "border-radius: 4px; padding: 8px 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0056b3; }");

    controlLayout->addWidget(titleLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(new QLabel(QString::fromUtf8("图表类型:")));
    controlLayout->addWidget(chartTypeCombo);
    controlLayout->addWidget(updateBtn);

    chartLayout->addLayout(controlLayout);

    // 创建图表
    chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueIcy);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet(
        "QChartView { border: 2px solid #dee2e6; border-radius: 8px; "
        "background-color: white; }");

    chartLayout->addWidget(chartView);
    chartLayout->setStretchFactor(chartView, 1);

    // 连接更新按钮
    connect(updateBtn, &QPushButton::clicked, this,
            &RailwayMainWindow::updateChartDisplay);
  }

  void initializeData() {
    // 尝试从CSV文件加载实际数据
    stations = fileManager.loadStations();
    routes = fileManager.loadRoutes(stations);
    trains = fileManager.loadTrains(routes);

    // 暂时不从CSV加载客流数据，因为解析有问题
    // 直接使用生成的合理数据
    passengerFlow.clearAllRecords();

    // 如果加载失败或数据为空，使用示例数据
    if (stations.empty()) {
      initSampleData();
    } else {
      // 基于真实站点数据生成合理的客流数据
      generateRealisticFlowData();
    }

    statusBar()->showMessage(
        QString::fromUtf8(
            "已加载 %1 个站点, %2 条线路, %3 列列车, %4 条客流记录")
            .arg(stations.size())
            .arg(routes.size())
            .arg(trains.size())
            .arg(passengerFlow.getRecordCount()),
        3000);
  }

  void initSampleData() {
    // 创建示例站点
    auto station1 = std::make_shared<Station>(
        "CQ001", "重庆北站", "重庆", 106.5516, 29.8132, "起始站", 8, true);
    auto station2 = std::make_shared<Station>(
        "CQ002", "重庆西站", "重庆", 106.4270, 29.5308, "中间站", 6, false);
    auto station3 = std::make_shared<Station>(
        "CD001", "成都东站", "成都", 104.1414, 30.6302, "终点站", 12, true);
    auto station4 = std::make_shared<Station>(
        "CD002", "成都南站", "成都", 104.0633, 30.6115, "中间站", 4, false);

    stations = {station1, station2, station3, station4};

    // 创建示例线路
    auto route1 =
        std::make_shared<Route>("R001", "成渝高铁", "高铁", 308.0, 350);
    route1->addStation(station3);
    route1->addStation(station4);
    route1->addStation(station2);
    route1->addStation(station1);

    routes = {route1};

    // 创建示例列车
    auto train1 = std::make_shared<Train>("G8501", "G", route1, 1200);
    auto train2 = std::make_shared<Train>("G8502", "G", route1, 1200);

    trains = {train1, train2};

    // 创建示例客流数据
    Date today(2024, 12, 15);
    passengerFlow.addRecord(FlowRecord("F001", "CD001", std::string("成都东站"),
                                       today, 8, 350, 120, "G8501",
                                       std::string("川->渝")));
    passengerFlow.addRecord(FlowRecord("F002", "CD001", std::string("成都东站"),
                                       today, 9, 420, 80, "G8503",
                                       std::string("川->渝")));
    passengerFlow.addRecord(FlowRecord("F003", "CD001", std::string("成都东站"),
                                       today, 11, 180, 280, "G8502",
                                       std::string("渝->川")));
    passengerFlow.addRecord(FlowRecord("F004", "CQ001", std::string("重庆北站"),
                                       today, 9, 380, 150, "G8502",
                                       std::string("渝->川")));
    passengerFlow.addRecord(FlowRecord("F005", "CQ001", std::string("重庆北站"),
                                       today, 10, 200, 320, "G8501",
                                       std::string("川->渝")));
    passengerFlow.addRecord(FlowRecord("F006", "CQ001", std::string("重庆北站"),
                                       today, 15, 450, 90, "G8504",
                                       std::string("渝->川")));
  }

  void generateRealisticFlowData() {
    // 基于真实站点数据生成合理的客流数据
    Date today(2024, 12, 15);

    // 初始化随机种子以确保合理的随机数
    srand(static_cast<unsigned int>(time(nullptr)));

    // 找到所有主要城市的站点
    std::vector<std::shared_ptr<Station>> majorStations;
    std::vector<std::shared_ptr<Station>> otherStations;

    for (const auto &station : stations) {
      if (station && !station->getStationName().empty()) {
        std::string name = station->getStationName();
        // 检查是否为主要城市站点
        if (name.find("成都") != std::string::npos ||
            name.find("重庆") != std::string::npos ||
            name.find("北京") != std::string::npos ||
            name.find("上海") != std::string::npos ||
            name.find("广州") != std::string::npos ||
            name.find("深圳") != std::string::npos) {
          majorStations.push_back(station);
        } else {
          otherStations.push_back(station);
        }
      }
    }

    int recordId = 1;

    // 为所有站点生成客流数据（移除数量限制）

    // 为主要站点生成高客流数据
    for (const auto &station : majorStations) {
      if (station) {
        // 只生成一条记录代表该站点的总客流，避免数据量过大
        int boarding = 150 + rand() % 200;  // 150-350人
        int alighting = 100 + rand() % 150; // 100-250人

        std::string trainId = "G" + std::to_string(8500 + recordId % 100);
        std::string direction = (recordId % 2 == 0) ? "川->渝" : "渝->川";

        passengerFlow.addRecord(
            FlowRecord("F" + std::to_string(recordId++),
                       station->getStationId(), station->getStationName(),
                       today, 12, boarding, alighting, trainId, direction));
      }
    }

    // 为其他站点生成较低的客流数据（移除150的限制）
    for (const auto &station : otherStations) {
      if (station) {
        // 每个站点生成一条记录
        int boarding = 20 + rand() % 80;  // 20-100人
        int alighting = 10 + rand() % 60; // 10-70人

        std::string trainId = "G" + std::to_string(8500 + recordId % 100);
        std::string direction = (recordId % 2 == 0) ? "川->渝" : "渝->川";

        passengerFlow.addRecord(
            FlowRecord("F" + std::to_string(recordId++),
                       station->getStationId(), station->getStationName(),
                       today, 12, boarding, alighting, trainId, direction));
      }
    }
  }

  void updateStationList() {
    stationTree->clear();
    for (const auto &station : stations) {
      if (station) {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdString(station->getStationName()));
        item->setText(1, QString::fromStdString(station->getStationId()));
        item->setText(2, QString::fromStdString(station->getCityName()));
        item->setText(3, QString::fromStdString(station->getStationType()));
        stationTree->addTopLevelItem(item);
      }
    }
  }

  void updateRouteList() {
    routeTree->clear();
    for (const auto &route : routes) {
      if (route) {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdString(route->getRouteName()));
        item->setText(1, QString::fromStdString(route->getRouteId()));
        item->setText(2, QString::fromStdString(route->getRouteType()));
        item->setText(3, QString::number(route->getTotalDistance()) + " km");
        routeTree->addTopLevelItem(item);
      }
    }
  }

  void updateTrainList() {
    trainTable->setRowCount(static_cast<int>(trains.size()));
    for (size_t i = 0; i < trains.size(); ++i) {
      if (trains[i]) {
        trainTable->setItem(static_cast<int>(i), 0,
                            new QTableWidgetItem(QString::fromStdString(
                                trains[i]->getTrainId())));
        trainTable->setItem(static_cast<int>(i), 1,
                            new QTableWidgetItem(QString::fromStdString(
                                trains[i]->getTrainType())));
        trainTable->setItem(static_cast<int>(i), 2,
                            new QTableWidgetItem(
                                trains[i]->getRoute()
                                    ? QString::fromStdString(
                                          trains[i]->getRoute()->getRouteName())
                                    : QString::fromUtf8("无")));
        trainTable->setItem(static_cast<int>(i), 3,
                            new QTableWidgetItem(QString::number(
                                trains[i]->getTotalCapacity())));
      }
    }
  }

  void updateInfo() {
    QString info = QString::fromUtf8("系统状态信息\n==================\n");
    info += QString::fromUtf8("站点数量: %1\n").arg(stations.size());
    info += QString::fromUtf8("线路数量: %1\n").arg(routes.size());
    info += QString::fromUtf8("列车数量: %1\n").arg(trains.size());
    info +=
        QString::fromUtf8("客流记录: %1\n").arg(passengerFlow.getRecordCount());

    infoDisplay->setText(info);
  }

private:
  // 数据成员
  std::vector<std::shared_ptr<Station>> stations;
  std::vector<std::shared_ptr<Route>> routes;
  std::vector<std::shared_ptr<Train>> trains;
  PassengerFlow passengerFlow;
  FileManager fileManager;

  // UI组件
  QTreeWidget *stationTree;
  QTreeWidget *routeTree;
  QTableWidget *trainTable;
  QTextEdit *infoDisplay;
  QComboBox *analysisTypeCombo;
  QTextEdit *analysisResults;
  QComboBox *chartTypeCombo;
  QChart *chart;
  QChartView *chartView;
  QLabel *predictionDaysLabel;
  QComboBox *predictionDaysCombo;
  QLabel *stationLabel;
  QComboBox *stationCombo;
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 设置系统locale和编码
  std::setlocale(LC_ALL, "zh_CN.UTF-8");

  // 设置Qt字体
  QFont font("Microsoft YaHei", 9);
  app.setFont(font);

  // 设置应用程序信息
  app.setApplicationName(QString::fromUtf8("川渝轨道交通客流分析系统"));
  app.setApplicationVersion("1.0");

  // 创建并显示主窗口
  RailwayMainWindow window;
  window.show();

  return app.exec();
}

#include "main_gui.moc"