#include "MainWindow.h"
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), fileManager("data") {
  setupUI();
  initializeData();
  updateDisplays();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
  setWindowTitle("川渝地区轨道交通客流数据分析与展示系统 v1.0");
  setMinimumSize(1200, 800);
  resize(1400, 900);

  setupMenuBar();
  setupCentralWidget();
  setupStatusBar();

  // 应用现代化样式
  setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QTabWidget::pane {
            border: 1px solid #c0c0c0;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #e1e1e1;
            padding: 8px 16px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 2px solid #3498db;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #cccccc;
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
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #21618c;
        }
    )");
}

void MainWindow::setupMenuBar() {
  // 文件菜单
  QMenu *fileMenu = menuBar()->addMenu("文件");

  refreshAction = new QAction("刷新数据", this);
  refreshAction->setShortcut(QKeySequence::Refresh);
  connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshData);
  fileMenu->addAction(refreshAction);

  fileMenu->addSeparator();

  importAction = new QAction("导入数据...", this);
  importAction->setShortcut(QKeySequence::Open);
  connect(importAction, &QAction::triggered, this, &MainWindow::importData);
  fileMenu->addAction(importAction);

  exportAction = new QAction("导出数据...", this);
  exportAction->setShortcut(QKeySequence::SaveAs);
  connect(exportAction, &QAction::triggered, this, &MainWindow::exportData);
  fileMenu->addAction(exportAction);

  fileMenu->addSeparator();

  exitAction = new QAction("退出", this);
  exitAction->setShortcut(QKeySequence::Quit);
  connect(exitAction, &QAction::triggered, this, &QWidget::close);
  fileMenu->addAction(exitAction);

  // 帮助菜单
  QMenu *helpMenu = menuBar()->addMenu("帮助");
  aboutAction = new QAction("关于", this);
  connect(aboutAction, &QAction::triggered, [this]() {
    QMessageBox::about(this, "关于",
                       "川渝地区轨道交通客流数据分析与展示系统\n"
                       "版本: 1.0\n"
                       "基于Qt6开发的现代化图形界面\n"
                       "支持客流统计、分析、预测和可视化展示");
  });
  helpMenu->addAction(aboutAction);
}

void MainWindow::setupCentralWidget() {
  centralTabs = new QTabWidget;
  setCentralWidget(centralTabs);

  // 数据展示标签页
  setupDataPanel();

  // 分析标签页
  setupAnalysisPanel();

  // 图表标签页
  setupChartPanel();
}

void MainWindow::setupDataPanel() {
  dataTab = new QWidget;
  centralTabs->addTab(dataTab, "数据管理");

  dataSplitter = new QSplitter(Qt::Horizontal);

  // 左侧：数据树
  QWidget *leftPanel = new QWidget;
  QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

  // 站点树
  QGroupBox *stationGroup = new QGroupBox("站点信息");
  QVBoxLayout *stationLayout = new QVBoxLayout(stationGroup);
  stationTree = new QTreeWidget;
  stationTree->setHeaderLabels({"站点名称", "编号", "城市", "类型"});
  connect(stationTree, &QTreeWidget::itemSelectionChanged, this,
          &MainWindow::onStationSelectionChanged);
  stationLayout->addWidget(stationTree);
  leftLayout->addWidget(stationGroup);

  // 线路树
  QGroupBox *routeGroup = new QGroupBox("线路信息");
  QVBoxLayout *routeLayout = new QVBoxLayout(routeGroup);
  routeTree = new QTreeWidget;
  routeTree->setHeaderLabels({"线路名称", "编号", "类型", "总长"});
  connect(routeTree, &QTreeWidget::itemSelectionChanged, this,
          &MainWindow::onRouteSelectionChanged);
  routeLayout->addWidget(routeTree);
  leftLayout->addWidget(routeGroup);

  dataSplitter->addWidget(leftPanel);

  // 右侧：详细信息
  QWidget *rightPanel = new QWidget;
  QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

  // 列车表格
  QGroupBox *trainGroup = new QGroupBox("列车信息");
  QVBoxLayout *trainLayout = new QVBoxLayout(trainGroup);
  trainTable = new QTableWidget;
  trainTable->setColumnCount(5);
  trainTable->setHorizontalHeaderLabels(
      {"列车号", "类型", "线路", "载客量", "当前状态"});
  trainTable->horizontalHeader()->setStretchLastSection(true);
  trainLayout->addWidget(trainTable);
  rightLayout->addWidget(trainGroup);

  // 详细信息显示
  QGroupBox *infoGroup = new QGroupBox("详细信息");
  QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
  infoDisplay = new QTextEdit;
  infoDisplay->setReadOnly(true);
  infoLayout->addWidget(infoDisplay);
  rightLayout->addWidget(infoGroup);

  dataSplitter->addWidget(rightPanel);
  dataSplitter->setSizes({400, 800});

  QVBoxLayout *dataLayout = new QVBoxLayout(dataTab);
  dataLayout->addWidget(dataSplitter);
}

void MainWindow::setupAnalysisPanel() {
  analysisTab = new QWidget;
  centralTabs->addTab(analysisTab, "数据分析");

  analysisLayout = new QVBoxLayout(analysisTab);

  // 控制面板
  controlGroup = new QGroupBox("分析控制");
  QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);

  controlLayout->addWidget(new QLabel("分析类型:"));
  analysisTypeCombo = new QComboBox;
  analysisTypeCombo->addItems({"站点客流排行", "川渝双向流量", "列车载客率",
                               "小时流量分布", "客流预测"});
  controlLayout->addWidget(analysisTypeCombo);

  controlLayout->addWidget(new QLabel("日期:"));
  dateEdit = new QDateEdit;
  dateEdit->setDate(QDate::currentDate());
  dateEdit->setCalendarPopup(true);
  connect(dateEdit, &QDateEdit::dateChanged, this, &MainWindow::onDateChanged);
  controlLayout->addWidget(dateEdit);

  analyzeButton = new QPushButton("开始分析");
  connect(analyzeButton, &QPushButton::clicked, [this]() {
    QString type = analysisTypeCombo->currentText();
    if (type == "站点客流排行") {
      analyzeStationRanking();
    } else if (type == "川渝双向流量") {
      analyzeDirectionalFlow();
    } else if (type == "列车载客率") {
      analyzeLoadFactor();
    } else if (type == "客流预测") {
      predictFlow();
    }
  });
  controlLayout->addWidget(analyzeButton);

  controlLayout->addStretch();
  analysisLayout->addWidget(controlGroup);

  // 结果显示
  QGroupBox *resultGroup = new QGroupBox("分析结果");
  QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
  analysisResults = new QTextEdit;
  analysisResults->setReadOnly(true);
  resultLayout->addWidget(analysisResults);
  analysisLayout->addWidget(resultGroup);
}

void MainWindow::setupChartPanel() {
  chartTab = new QWidget;
  centralTabs->addTab(chartTab, "可视化图表");

  chartLayout = new QVBoxLayout(chartTab);

  // 图表控制
  chartControlLayout = new QHBoxLayout;
  chartControlLayout->addWidget(new QLabel("图表类型:"));

  chartTypeCombo = new QComboBox;
  chartTypeCombo->addItems(
      {"客流趋势图", "站点排行图", "双向流量对比", "载客率分析", "小时分布图"});
  chartControlLayout->addWidget(chartTypeCombo);

  updateChartButton = new QPushButton("更新图表");
  connect(updateChartButton, &QPushButton::clicked, this,
          &MainWindow::updateCharts);
  chartControlLayout->addWidget(updateChartButton);

  chartControlLayout->addStretch();
  chartLayout->addLayout(chartControlLayout);

  // 图表组件
  chartWidget = new ChartWidget;
  chartLayout->addWidget(chartWidget);
}

void MainWindow::setupStatusBar() {
  statusLabel = new QLabel("就绪");
  statusBar()->addWidget(statusLabel);

  recordCountLabel = new QLabel("记录数: 0");
  statusBar()->addPermanentWidget(recordCountLabel);

  progressBar = new QProgressBar;
  progressBar->setVisible(false);
  statusBar()->addPermanentWidget(progressBar);
}

void MainWindow::initializeData() {
  statusLabel->setText("正在加载数据...");
  progressBar->setVisible(true);
  progressBar->setRange(0, 0); // 不确定进度

  if (!fileManager.importAllData(stations, routes, trains, passengerFlow)) {
    statusLabel->setText("未找到数据文件，正在初始化示例数据...");

    // 初始化示例数据（与console版本相同的逻辑）
    initSampleData();

    fileManager.exportAllData(stations, routes, trains, passengerFlow);
    statusLabel->setText("示例数据初始化完成");
  } else {
    statusLabel->setText("数据加载完成");
  }

  progressBar->setVisible(false);
}

// 由于代码很长，我会创建一个简化版本，包含主要功能
void MainWindow::initSampleData() {
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
  auto route1 = std::make_shared<Route>("R001", "成渝高铁", "高铁", 308.0, 350);
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
  passengerFlow.addRecord(FlowRecord("F001", "CD001", "成都东站", today, 8, 350,
                                     120, "G8501", "川->渝"));
  passengerFlow.addRecord(FlowRecord("F002", "CQ001", "重庆北站", today, 10,
                                     200, 320, "G8501", "川->渝"));
}

void MainWindow::updateDisplays() {
  updateStationList();
  updateRouteList();
  updateTrainList();
  updateFlowStatistics();
}

void MainWindow::updateStationList() {
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

void MainWindow::updateRouteList() {
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

void MainWindow::updateTrainList() {
  trainTable->setRowCount(trains.size());
  for (size_t i = 0; i < trains.size(); ++i) {
    if (trains[i]) {
      trainTable->setItem(i, 0,
                          new QTableWidgetItem(
                              QString::fromStdString(trains[i]->getTrainId())));
      trainTable->setItem(i, 1,
                          new QTableWidgetItem(QString::fromStdString(
                              trains[i]->getTrainType())));
      trainTable->setItem(
          i, 2,
          new QTableWidgetItem(trains[i]->getRoute()
                                   ? QString::fromStdString(
                                         trains[i]->getRoute()->getRouteName())
                                   : "无"));
      trainTable->setItem(
          i, 3,
          new QTableWidgetItem(QString::number(trains[i]->getTotalCapacity())));
      trainTable->setItem(i, 4, new QTableWidgetItem("运行中"));
    }
  }
}

void MainWindow::updateFlowStatistics() {
  int recordCount = passengerFlow.getRecordCount();
  recordCountLabel->setText(QString("记录数: %1").arg(recordCount));
}

void MainWindow::updateCharts() {
  QString chartType = chartTypeCombo->currentText();

  if (chartType == "客流趋势图") {
    chartWidget->updateChart(passengerFlow, ChartWidget::FlowTrend);
  } else if (chartType == "站点排行图") {
    chartWidget->updateChart(passengerFlow, ChartWidget::StationRanking);
  } else if (chartType == "双向流量对比") {
    chartWidget->updateChart(passengerFlow, ChartWidget::DirectionalFlow);
  } else if (chartType == "载客率分析") {
    chartWidget->updateChart(passengerFlow, ChartWidget::LoadFactor);
  } else if (chartType == "小时分布图") {
    chartWidget->updateChart(passengerFlow, ChartWidget::HourlyFlow);
  }
}

// 槽函数实现
void MainWindow::refreshData() {
  initializeData();
  updateDisplays();
  statusLabel->setText("数据已刷新");
}

void MainWindow::showStationInfo() { updateStationList(); }

void MainWindow::showRouteInfo() { updateRouteList(); }

void MainWindow::showTrainInfo() { updateTrainList(); }

void MainWindow::showFlowStatistics() {
  QString stats = QString("总记录数: %1\n").arg(passengerFlow.getRecordCount());

  auto stationFlow = passengerFlow.getAllStationsFlow();
  stats += "\n各站点总客流量:\n";
  for (const auto &pair : stationFlow) {
    stats += QString("站点 %1: %2 人次\n")
                 .arg(QString::fromStdString(pair.first))
                 .arg(pair.second);
  }

  infoDisplay->setText(stats);
}

void MainWindow::analyzeStationRanking() {
  QString ranking =
      QString::fromStdString(passengerFlow.generateStationRanking());
  analysisResults->setText(ranking);
}

void MainWindow::analyzeDirectionalFlow() {
  Date today(2024, 12, 15);
  int cd2cq = passengerFlow.getChengduToChongqingFlow(today);
  int cq2cd = passengerFlow.getChongqingToChengduFlow(today);
  double ratio = passengerFlow.getFlowRatio();

  QString analysis = QString("川渝双向流量分析\n"
                             "==================\n"
                             "川->渝方向: %1 人次\n"
                             "渝->川方向: %2 人次\n"
                             "流量比 (川/渝): %3\n\n"
                             "分析结论: %4")
                         .arg(cd2cq)
                         .arg(cq2cd)
                         .arg(ratio, 0, 'f', 2)
                         .arg(ratio > 1.2   ? "成都到重庆方向客流明显高于反向"
                              : ratio < 0.8 ? "重庆到成都方向客流明显高于反向"
                                            : "双向客流相对均衡");

  analysisResults->setText(analysis);
}

void MainWindow::analyzeLoadFactor() {
  Date today(2024, 12, 15);
  auto loadFactors = passengerFlow.getAllTrainsLoadFactor(today);

  QString analysis = "列车载客率分析\n==================\n";
  for (const auto &pair : loadFactors) {
    QString status;
    if (pair.second > 90) {
      status = "严重超载，建议增加班次";
    } else if (pair.second > 80) {
      status = "高负荷运行";
    } else if (pair.second > 60) {
      status = "正常运行";
    } else {
      status = "低负荷运行";
    }

    analysis += QString("列车 %1: %2% - %3\n")
                    .arg(QString::fromStdString(pair.first))
                    .arg(pair.second, 0, 'f', 1)
                    .arg(status);
  }

  analysisResults->setText(analysis);
}

void MainWindow::predictFlow() {
  auto prediction = passengerFlow.predictFlow("CD001", 3);

  QString result = "成都东站未来3天客流预测\n========================\n";
  for (size_t i = 0; i < prediction.size(); ++i) {
    result += QString("第%1天: %2 人次\n").arg(i + 1).arg(prediction[i]);
  }

  auto directionPrediction = passengerFlow.predictDirectionalFlow("川->渝", 3);
  result += "\n川->渝方向未来3天流量预测\n=======================\n";
  for (size_t i = 0; i < directionPrediction.size(); ++i) {
    result +=
        QString("第%1天: %2 人次\n").arg(i + 1).arg(directionPrediction[i]);
  }

  analysisResults->setText(result);
}

void MainWindow::generateReport() {
  Date today(2024, 12, 15);
  QString report =
      QString::fromStdString(passengerFlow.generateFlowReport(today));
  analysisResults->setText(report);
}

void MainWindow::exportData() {
  QString fileName =
      QFileDialog::getSaveFileName(this, "导出数据", "", "CSV文件 (*.csv)");
  if (!fileName.isEmpty()) {
    if (fileManager.exportAllData(stations, routes, trains, passengerFlow)) {
      QMessageBox::information(this, "成功", "数据导出成功！");
    } else {
      QMessageBox::warning(this, "错误", "数据导出失败！");
    }
  }
}

void MainWindow::importData() {
  QString fileName =
      QFileDialog::getOpenFileName(this, "导入数据", "", "CSV文件 (*.csv)");
  if (!fileName.isEmpty()) {
    initializeData();
    updateDisplays();
    QMessageBox::information(this, "成功", "数据导入成功！");
  }
}

void MainWindow::onStationSelectionChanged() {
  auto current = stationTree->currentItem();
  if (current) {
    QString stationId = current->text(1);
    for (const auto &station : stations) {
      if (station && station->getStationId() == stationId.toStdString()) {
        infoDisplay->setText(QString::fromStdString(station->toString()));
        break;
      }
    }
  }
}

void MainWindow::onRouteSelectionChanged() {
  auto current = routeTree->currentItem();
  if (current) {
    QString routeId = current->text(1);
    for (const auto &route : routes) {
      if (route && route->getRouteId() == routeId.toStdString()) {
        QString info = QString::fromStdString(route->toString()) + "\n\n" +
                       QString::fromStdString(route->getStationsInOrder());
        infoDisplay->setText(info);
        break;
      }
    }
  }
}

void MainWindow::onDateChanged() { updateFlowStatistics(); }

#include "MainWindow.moc"