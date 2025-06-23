#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QComboBox>
#include <QDateEdit>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <memory>
#include <vector>


#include "ChartWidget.h"
#include "DataAnalyzer.h"
#include "FileManager.h"
#include "PassengerFlow.h"
#include "Route.h"
#include "Station.h"
#include "Train.h"


class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void refreshData();
  void showStationInfo();
  void showRouteInfo();
  void showTrainInfo();
  void showFlowStatistics();
  void analyzeStationRanking();
  void analyzeDirectionalFlow();
  void analyzeLoadFactor();
  void predictFlow();
  void generateReport();
  void exportData();
  void importData();
  void onStationSelectionChanged();
  void onRouteSelectionChanged();
  void onDateChanged();

private:
  void setupUI();
  void setupMenuBar();
  void setupStatusBar();
  void setupCentralWidget();
  void setupDataPanel();
  void setupAnalysisPanel();
  void setupChartPanel();
  void initializeData();
  void updateDisplays();
  void updateStationList();
  void updateRouteList();
  void updateTrainList();
  void updateFlowStatistics();
  void updateCharts();

  // 数据成员
  std::vector<std::shared_ptr<Station>> stations;
  std::vector<std::shared_ptr<Route>> routes;
  std::vector<std::shared_ptr<Train>> trains;
  PassengerFlow passengerFlow;
  DataAnalyzer dataAnalyzer;
  FileManager fileManager;

  // UI组件
  QTabWidget *centralTabs;

  // 数据展示标签页
  QWidget *dataTab;
  QSplitter *dataSplitter;
  QTreeWidget *stationTree;
  QTreeWidget *routeTree;
  QTableWidget *trainTable;
  QTextEdit *infoDisplay;

  // 分析标签页
  QWidget *analysisTab;
  QVBoxLayout *analysisLayout;
  QGroupBox *controlGroup;
  QComboBox *analysisTypeCombo;
  QDateEdit *dateEdit;
  QPushButton *analyzeButton;
  QTextEdit *analysisResults;

  // 图表标签页
  QWidget *chartTab;
  ChartWidget *chartWidget;
  QVBoxLayout *chartLayout;
  QHBoxLayout *chartControlLayout;
  QComboBox *chartTypeCombo;
  QPushButton *updateChartButton;

  // 预测标签页
  QWidget *predictionTab;
  QVBoxLayout *predictionLayout;
  QGroupBox *predictionControlGroup;
  QComboBox *stationCombo;
  QSpinBox *daysSpinBox;
  QPushButton *predictButton;
  QTextEdit *predictionResults;

  // 状态栏组件
  QLabel *statusLabel;
  QLabel *recordCountLabel;
  QProgressBar *progressBar;

  // 菜单和动作
  QAction *refreshAction;
  QAction *exportAction;
  QAction *importAction;
  QAction *exitAction;
  QAction *aboutAction;
};

#endif // MAINWINDOW_H