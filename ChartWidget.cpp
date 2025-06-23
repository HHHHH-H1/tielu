#include "ChartWidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>


ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent) { setupUI(); }

ChartWidget::~ChartWidget() {}

void ChartWidget::setupUI() {
  mainLayout = new QVBoxLayout(this);

  // 标题
  titleLabel = new QLabel("客流数据可视化");
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet(
      "font-size: 18px; font-weight: bold; margin: 10px;");
  mainLayout->addWidget(titleLabel);

  // 控制面板
  controlLayout = new QHBoxLayout;

  chartTypeCombo = new QComboBox;
  chartTypeCombo->addItems(
      {"客流趋势图", "站点排行图", "双向流量对比", "载客率分析", "小时分布图"});
  connect(chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &ChartWidget::onChartTypeChanged);

  refreshButton = new QPushButton("刷新图表");
  connect(refreshButton, &QPushButton::clicked, this,
          &ChartWidget::onChartTypeChanged);

  controlLayout->addWidget(new QLabel("图表类型:"));
  controlLayout->addWidget(chartTypeCombo);
  controlLayout->addWidget(refreshButton);
  controlLayout->addStretch();

  mainLayout->addLayout(controlLayout);

  // 图表视图
  chart = new QChart;
  chart->setTheme(QChart::ChartThemeLight);
  chart->setAnimationOptions(QChart::AllAnimations);

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumHeight(400);

  mainLayout->addWidget(chartView);

  // 设置样式
  setStyleSheet(R"(
        QChartView {
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: white;
        }
    )");
}

void ChartWidget::clearChart() {
  chart->removeAllSeries();
  for (auto axis : chart->axes()) {
    chart->removeAxis(axis);
  }
}

void ChartWidget::updateChart(const PassengerFlow &passengerFlow,
                              ChartType type) {
  clearChart();

  switch (type) {
  case FlowTrend:
    updateFlowTrendChart(passengerFlow);
    break;
  case StationRanking:
    updateStationRankingChart(passengerFlow);
    break;
  case DirectionalFlow:
    updateDirectionalFlowChart(passengerFlow);
    break;
  case LoadFactor:
    updateLoadFactorChart(passengerFlow);
    break;
  case HourlyFlow:
    updateHourlyFlowChart(passengerFlow);
    break;
  }
}

void ChartWidget::updateFlowTrendChart(const PassengerFlow &passengerFlow) {
  titleLabel->setText("客流趋势分析");
  chart->setTitle("日客流变化趋势");

  QLineSeries *series = new QLineSeries;
  series->setName("总客流量");

  // 模拟7天的客流数据
  std::vector<std::pair<QString, double>> trendData = {
      {"12-09", 2800}, {"12-10", 3200}, {"12-11", 2900}, {"12-12", 3500},
      {"12-13", 3800}, {"12-14", 4200}, {"12-15", 3900}};

  for (size_t i = 0; i < trendData.size(); ++i) {
    series->append(i, trendData[i].second);
  }

  chart->addSeries(series);

  // 设置坐标轴
  QValueAxis *axisY = new QValueAxis;
  axisY->setTitleText("客流量 (人次)");
  axisY->setRange(0, 5000);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  QBarCategoryAxis *axisX = new QBarCategoryAxis;
  QStringList categories;
  for (const auto &data : trendData) {
    categories << data.first;
  }
  axisX->append(categories);
  axisX->setTitleText("日期");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);
}

void ChartWidget::updateStationRankingChart(
    const PassengerFlow &passengerFlow) {
  titleLabel->setText("站点客流排行");
  chart->setTitle("各站点客流量排行");

  auto stationFlow = passengerFlow.getAllStationsFlow();

  QBarSeries *series = new QBarSeries;
  QBarSet *set = new QBarSet("客流量");

  QStringList categories;
  std::vector<std::pair<std::string, int>> sortedFlow(stationFlow.begin(),
                                                      stationFlow.end());
  std::sort(sortedFlow.begin(), sortedFlow.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });

  for (const auto &pair : sortedFlow) {
    categories << QString::fromStdString(pair.first);
    *set << pair.second;
  }

  series->append(set);
  chart->addSeries(series);

  QBarCategoryAxis *axisX = new QBarCategoryAxis;
  axisX->append(categories);
  axisX->setTitleText("站点");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setTitleText("客流量 (人次)");
  axisY->setRange(
      0, *std::max_element(set->values().begin(), set->values().end()) * 1.2);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);
}

void ChartWidget::updateDirectionalFlowChart(
    const PassengerFlow &passengerFlow) {
  titleLabel->setText("川渝双向流量对比");
  chart->setTitle("川渝双向客流对比分析");

  Date today(2024, 12, 15);
  int cd2cq = passengerFlow.getChengduToChongqingFlow(today);
  int cq2cd = passengerFlow.getChongqingToChengduFlow(today);

  QBarSeries *series = new QBarSeries;
  QBarSet *set = new QBarSet("客流量");

  *set << cd2cq << cq2cd;
  series->append(set);
  chart->addSeries(series);

  QBarCategoryAxis *axisX = new QBarCategoryAxis;
  axisX->append({"川→渝", "渝→川"});
  axisX->setTitleText("方向");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setTitleText("客流量 (人次)");
  axisY->setRange(0, std::max(cd2cq, cq2cd) * 1.2);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  // 添加比例信息
  double ratio = passengerFlow.getFlowRatio();
  QString subtitle = QString("流量比 (川/渝): %1").arg(ratio, 0, 'f', 2);
  chart->setTitle(chart->title() + "\n" + subtitle);
}

void ChartWidget::updateLoadFactorChart(const PassengerFlow &passengerFlow) {
  titleLabel->setText("列车载客率分析");
  chart->setTitle("各列车载客率对比");

  Date today(2024, 12, 15);
  auto loadFactors = passengerFlow.getAllTrainsLoadFactor(today);

  QBarSeries *series = new QBarSeries;
  QBarSet *set = new QBarSet("载客率 (%)");

  QStringList categories;
  for (const auto &pair : loadFactors) {
    categories << QString::fromStdString(pair.first);
    *set << pair.second;
  }

  series->append(set);
  chart->addSeries(series);

  QBarCategoryAxis *axisX = new QBarCategoryAxis;
  axisX->append(categories);
  axisX->setTitleText("列车");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setTitleText("载客率 (%)");
  axisY->setRange(0, 120);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  // 添加安全线
  QLineSeries *safeLine = new QLineSeries;
  safeLine->setName("安全线 (80%)");
  safeLine->setPen(QPen(Qt::red, 2, Qt::DashLine));
  for (int i = 0; i < categories.size(); ++i) {
    safeLine->append(i, 80);
  }
  chart->addSeries(safeLine);
  safeLine->attachAxis(axisX);
  safeLine->attachAxis(axisY);
}

void ChartWidget::updateHourlyFlowChart(const PassengerFlow &passengerFlow) {
  titleLabel->setText("小时客流分布");
  chart->setTitle("24小时客流分布图");

  QLineSeries *series = new QLineSeries;
  series->setName("小时客流");

  // 模拟24小时客流分布
  std::vector<int> hourlyFlow = {50,  30,  20,  15,  20,  80,  200, 450,
                                 380, 280, 320, 400, 450, 380, 350, 400,
                                 480, 520, 420, 350, 280, 200, 150, 80};

  for (size_t i = 0; i < hourlyFlow.size(); ++i) {
    series->append(i, hourlyFlow[i]);
  }

  chart->addSeries(series);

  QValueAxis *axisX = new QValueAxis;
  axisX->setTitleText("小时");
  axisX->setRange(0, 23);
  axisX->setTickCount(13);
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setTitleText("客流量 (人次)");
  axisY->setRange(0, 600);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);
}

void ChartWidget::onChartTypeChanged() {
  // 这里可以添加图表类型改变时的处理逻辑
  // 需要从主窗口获取最新数据并更新图表
}

QLineSeries *ChartWidget::createLineSeries(
    const QString &name, const std::vector<std::pair<QString, double>> &data) {
  QLineSeries *series = new QLineSeries;
  series->setName(name);

  for (size_t i = 0; i < data.size(); ++i) {
    series->append(i, data[i].second);
  }

  return series;
}

QBarSeries *ChartWidget::createBarSeries(
    const QString &name, const std::vector<std::pair<QString, double>> &data) {
  QBarSeries *series = new QBarSeries;
  QBarSet *set = new QBarSet(name);

  for (const auto &pair : data) {
    *set << pair.second;
  }

  series->append(set);
  return series;
}

QPieSeries *ChartWidget::createPieSeries(
    const std::vector<std::pair<QString, double>> &data) {
  QPieSeries *series = new QPieSeries;

  for (const auto &pair : data) {
    series->append(pair.first, pair.second);
  }

  return series;
}

#include "ChartWidget.moc"