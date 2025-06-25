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
#include <clocale>
#include <locale>
#include <memory>
#include <vector>

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
      : QMainWindow(parent), fileManager("data") {
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
    }

    analysisResults->setText(result);
    statusBar()->showMessage(QString::fromUtf8("分析完成"), 2000);
  }

  void updateChart() {
    QString chartType = chartTypeCombo->currentText();
    QString chartText = QString::fromUtf8("图表类型: %1\n").arg(chartType);
    chartText += "=====================================\n\n";

    if (chartType == QString::fromUtf8("客流趋势图")) {
      chartText += QString::fromUtf8("最近7天客流趋势:\n");
      chartText += QString::fromUtf8("12-09: ████████████████████ 2800人\n");
      chartText +=
          QString::fromUtf8("12-10: ████████████████████████ 3200人\n");
      chartText += QString::fromUtf8("12-11: ██████████████████████ 2900人\n");
      chartText +=
          QString::fromUtf8("12-12: ████████████████████████████ 3500人\n");
      chartText +=
          QString::fromUtf8("12-13: ██████████████████████████████ 3800人\n");
      chartText += QString::fromUtf8(
          "12-14: ████████████████████████████████████ 4200人\n");
      chartText += QString::fromUtf8(
          "12-15: ██████████████████████████████████ 3900人\n");
    } else if (chartType == QString::fromUtf8("站点排行图")) {
      auto stationFlow = passengerFlow.getAllStationsFlow();
      chartText += QString::fromUtf8("站点客流排行:\n");
      int rank = 1;
      for (const auto &pair : stationFlow) {
        QString bars = QString::fromUtf8("█").repeated(pair.second / 10);
        chartText += QString::fromUtf8("%1. %2: %3 %4人\n")
                         .arg(rank++)
                         .arg(QString::fromStdString(pair.first))
                         .arg(bars)
                         .arg(pair.second);
      }
    }

    chartDisplay->setText(chartText);
    statusBar()->showMessage(QString::fromUtf8("图表已更新"), 2000);
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
                                 QString::fromUtf8("列车载客率分析")});
    controlLayout->addWidget(analysisTypeCombo);

    QPushButton *analyzeBtn = new QPushButton(QString::fromUtf8("开始分析"));
    connect(analyzeBtn, &QPushButton::clicked, this,
            &RailwayMainWindow::performAnalysis);
    controlLayout->addWidget(analyzeBtn);

    controlLayout->addStretch();
    analysisLayout->addWidget(controlGroup);

    // 结果显示
    QGroupBox *resultGroup = new QGroupBox(QString::fromUtf8("分析结果"));
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    analysisResults = new QTextEdit;
    analysisResults->setReadOnly(true);
    resultLayout->addWidget(analysisResults);
    analysisLayout->addWidget(resultGroup);
  }

  void setupChartTab(QTabWidget *tabWidget) {
    QWidget *chartTab = new QWidget;
    tabWidget->addTab(chartTab, QString::fromUtf8("可视化图表"));

    QVBoxLayout *chartLayout = new QVBoxLayout(chartTab);

    // 图表控制
    QGroupBox *chartControlGroup = new QGroupBox(QString::fromUtf8("图表控制"));
    QHBoxLayout *chartControlLayout = new QHBoxLayout(chartControlGroup);

    chartControlLayout->addWidget(new QLabel(QString::fromUtf8("图表类型:")));
    chartTypeCombo = new QComboBox;
    chartTypeCombo->addItems({QString::fromUtf8("客流趋势图"),
                              QString::fromUtf8("站点排行图"),
                              QString::fromUtf8("双向流量对比图")});
    chartControlLayout->addWidget(chartTypeCombo);

    QPushButton *updateChartBtn =
        new QPushButton(QString::fromUtf8("更新图表"));
    connect(updateChartBtn, &QPushButton::clicked, this,
            &RailwayMainWindow::updateChart);
    chartControlLayout->addWidget(updateChartBtn);

    chartControlLayout->addStretch();
    chartLayout->addWidget(chartControlGroup);

    // 图表显示
    QGroupBox *chartDisplayGroup = new QGroupBox(QString::fromUtf8("图表显示"));
    QVBoxLayout *chartDisplayLayout = new QVBoxLayout(chartDisplayGroup);

    chartDisplay = new QTextEdit;
    chartDisplay->setReadOnly(true);
    chartDisplay->setText(
        QString::fromUtf8("请选择图表类型并点击'更新图表'按钮"));
    chartDisplayLayout->addWidget(chartDisplay);
    chartLayout->addWidget(chartDisplayGroup);
  }

  void initializeData() {
    if (!fileManager.importAllData(stations, routes, trains, passengerFlow)) {
      initSampleData();
      fileManager.exportAllData(stations, routes, trains, passengerFlow);
    }
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

  void updateDisplays() {
    updateStationList();
    updateRouteList();
    updateTrainList();
    updateInfo();
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
  QTextEdit *chartDisplay;
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 设置系统locale
  std::setlocale(LC_ALL, "");

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