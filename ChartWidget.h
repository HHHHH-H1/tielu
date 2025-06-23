#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include "PassengerFlow.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <memory>
#include <vector>


QT_CHARTS_USE_NAMESPACE

class ChartWidget : public QWidget {
  Q_OBJECT

public:
  explicit ChartWidget(QWidget *parent = nullptr);
  ~ChartWidget();

  enum ChartType {
    FlowTrend,       // 客流趋势图
    StationRanking,  // 站点排行图
    DirectionalFlow, // 双向流量对比图
    LoadFactor,      // 载客率图
    HourlyFlow       // 小时客流分布图
  };

public slots:
  void updateChart(const PassengerFlow &passengerFlow, ChartType type);
  void updateFlowTrendChart(const PassengerFlow &passengerFlow);
  void updateStationRankingChart(const PassengerFlow &passengerFlow);
  void updateDirectionalFlowChart(const PassengerFlow &passengerFlow);
  void updateLoadFactorChart(const PassengerFlow &passengerFlow);
  void updateHourlyFlowChart(const PassengerFlow &passengerFlow);

private slots:
  void onChartTypeChanged();

private:
  void setupUI();
  void clearChart();
  QLineSeries *
  createLineSeries(const QString &name,
                   const std::vector<std::pair<QString, double>> &data);
  QBarSeries *
  createBarSeries(const QString &name,
                  const std::vector<std::pair<QString, double>> &data);
  QPieSeries *
  createPieSeries(const std::vector<std::pair<QString, double>> &data);

  QVBoxLayout *mainLayout;
  QHBoxLayout *controlLayout;
  QComboBox *chartTypeCombo;
  QPushButton *refreshButton;
  QChartView *chartView;
  QChart *chart;
  QLabel *titleLabel;
};

#endif // CHARTWIDGET_H