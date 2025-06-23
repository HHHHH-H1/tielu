#include "FileManager.h"
#include "PassengerFlow.h"
#include "Route.h"
#include "Station.h"
#include "Train.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#endif

using namespace std;

// 初始化示例数据
void initSampleData(vector<shared_ptr<Station>> &stations,
                    vector<shared_ptr<Route>> &routes,
                    vector<shared_ptr<Train>> &trains,
                    PassengerFlow &passengerFlow) {

  cout << "正在初始化示例数据..." << endl;

  // 创建示例站点
  auto station1 = make_shared<Station>("CQ001", "重庆北站", "重庆", 106.5516,
                                       29.8132, "起始站", 8, true);
  auto station2 = make_shared<Station>("CQ002", "重庆西站", "重庆", 106.4270,
                                       29.5308, "中间站", 6, false);
  auto station3 = make_shared<Station>("CD001", "成都东站", "成都", 104.1414,
                                       30.6302, "终点站", 12, true);
  auto station4 = make_shared<Station>("CD002", "成都南站", "成都", 104.0633,
                                       30.6115, "中间站", 4, false);

  stations.push_back(station1);
  stations.push_back(station2);
  stations.push_back(station3);
  stations.push_back(station4);

  // 创建示例线路
  auto route1 = make_shared<Route>("R001", "成渝高铁", "高铁", 308.0, 350);
  route1->addStation(station3); // 成都东
  route1->addStation(station4); // 成都南
  route1->addStation(station2); // 重庆西
  route1->addStation(station1); // 重庆北

  routes.push_back(route1);

  // 创建示例列车
  auto train1 = make_shared<Train>("G8501", "G", route1, 1200);
  auto train2 = make_shared<Train>("G8502", "G", route1, 1200);

  // 添加时刻表
  train1->addScheduleEntry(
      ScheduleEntry("CD001", "成都东站", TimePoint(8, 0), TimePoint(8, 5), 5));
  train1->addScheduleEntry(ScheduleEntry("CD002", "成都南站", TimePoint(8, 25),
                                         TimePoint(8, 27), 2));
  train1->addScheduleEntry(ScheduleEntry("CQ002", "重庆西站", TimePoint(10, 15),
                                         TimePoint(10, 17), 2));
  train1->addScheduleEntry(ScheduleEntry("CQ001", "重庆北站", TimePoint(10, 35),
                                         TimePoint(10, 35), 0));

  train2->addScheduleEntry(
      ScheduleEntry("CQ001", "重庆北站", TimePoint(9, 0), TimePoint(9, 5), 5));
  train2->addScheduleEntry(ScheduleEntry("CQ002", "重庆西站", TimePoint(9, 18),
                                         TimePoint(9, 20), 2));
  train2->addScheduleEntry(ScheduleEntry("CD002", "成都南站", TimePoint(11, 8),
                                         TimePoint(11, 10), 2));
  train2->addScheduleEntry(ScheduleEntry("CD001", "成都东站", TimePoint(11, 30),
                                         TimePoint(11, 30), 0));

  trains.push_back(train1);
  trains.push_back(train2);

  // 创建示例客流数据
  Date today(2024, 12, 15);

  // 成都东站客流
  passengerFlow.addRecord(FlowRecord("F001", "CD001", "成都东站", today, 8, 350,
                                     120, "G8501", "川->渝"));
  passengerFlow.addRecord(FlowRecord("F002", "CD001", "成都东站", today, 9, 420,
                                     80, "G8503", "川->渝"));
  passengerFlow.addRecord(FlowRecord("F003", "CD001", "成都东站", today, 11,
                                     180, 280, "G8502", "渝->川"));

  // 重庆北站客流
  passengerFlow.addRecord(FlowRecord("F004", "CQ001", "重庆北站", today, 9, 380,
                                     150, "G8502", "渝->川"));
  passengerFlow.addRecord(FlowRecord("F005", "CQ001", "重庆北站", today, 10,
                                     200, 320, "G8501", "川->渝"));
  passengerFlow.addRecord(FlowRecord("F006", "CQ001", "重庆北站", today, 15,
                                     450, 90, "G8504", "渝->川"));

  cout << "示例数据初始化完成！" << endl << endl;
}

// 显示菜单
void showMenu() {
  cout << "\n==================== 川渝轨道交通客流分析系统 ===================="
       << endl;
  cout << "1. 显示所有站点信息" << endl;
  cout << "2. 显示所有线路信息" << endl;
  cout << "3. 显示所有列车信息" << endl;
  cout << "4. 显示客流统计信息" << endl;
  cout << "5. 分析站点客流排行" << endl;
  cout << "6. 分析川渝双向流量" << endl;
  cout << "7. 分析列车载客率" << endl;
  cout << "8. 预测客流变化" << endl;
  cout << "9. 生成日报告" << endl;
  cout << "0. 退出系统" << endl;
  cout << "==============================================================="
       << endl;
  cout << "请选择功能: ";
}

// 显示站点信息
void showStations(const vector<shared_ptr<Station>> &stations) {
  cout << "\n========== 站点信息 ==========" << endl;
  for (const auto &station : stations) {
    cout << station->toString() << endl;
  }
}

// 显示线路信息
void showRoutes(const vector<shared_ptr<Route>> &routes) {
  cout << "\n========== 线路信息 ==========" << endl;
  for (const auto &route : routes) {
    cout << route->toString() << endl;
    cout << route->getStationsInOrder() << endl;
  }
}

// 显示列车信息
void showTrains(const vector<shared_ptr<Train>> &trains) {
  cout << "\n========== 列车信息 ==========" << endl;
  for (const auto &train : trains) {
    cout << train->toString() << endl;
    cout << train->getScheduleString() << endl;
  }
}

// 显示客流统计
void showFlowStatistics(const PassengerFlow &passengerFlow) {
  cout << "\n========== 客流统计 ==========" << endl;
  cout << "总记录数: " << passengerFlow.getRecordCount() << endl;

  auto stationFlow = passengerFlow.getAllStationsFlow();
  cout << "\n各站点总客流量:" << endl;
  for (const auto &pair : stationFlow) {
    cout << "站点 " << pair.first << ": " << pair.second << " 人次" << endl;
  }
}

// 分析站点排行
void analyzeStationRanking(const PassengerFlow &passengerFlow) {
  cout << "\n========== 站点客流排行 ==========" << endl;
  cout << passengerFlow.generateStationRanking() << endl;
}

// 分析川渝双向流量
void analyzeDirectionalFlow(const PassengerFlow &passengerFlow) {
  cout << "\n========== 川渝双向流量分析 ==========" << endl;
  Date today(2024, 12, 15);

  int chengduToChongqing = passengerFlow.getChengduToChongqingFlow(today);
  int chongqingToChengdu = passengerFlow.getChongqingToChengduFlow(today);
  double ratio = passengerFlow.getFlowRatio();

  cout << "川->渝方向: " << chengduToChongqing << " 人次" << endl;
  cout << "渝->川方向: " << chongqingToChengdu << " 人次" << endl;
  cout << "流量比 (川/渝): " << fixed << setprecision(2) << ratio << endl;

  if (ratio > 1.2) {
    cout << "分析: 成都到重庆方向客流明显高于反向" << endl;
  } else if (ratio < 0.8) {
    cout << "分析: 重庆到成都方向客流明显高于反向" << endl;
  } else {
    cout << "分析: 双向客流相对均衡" << endl;
  }
}

// 分析列车载客率
void analyzeLoadFactor(const PassengerFlow &passengerFlow) {
  cout << "\n========== 列车载客率分析 ==========" << endl;
  Date today(2024, 12, 15);

  auto loadFactors = passengerFlow.getAllTrainsLoadFactor(today);

  for (const auto &pair : loadFactors) {
    cout << "列车 " << pair.first << ": " << fixed << setprecision(1)
         << pair.second << "%" << endl;

    if (pair.second > 90) {
      cout << "  状态: 严重超载，建议增加班次" << endl;
    } else if (pair.second > 80) {
      cout << "  状态: 高负荷运行" << endl;
    } else if (pair.second > 60) {
      cout << "  状态: 正常运行" << endl;
    } else {
      cout << "  状态: 低负荷运行" << endl;
    }
  }
}

// 预测客流
void predictFlow(const PassengerFlow &passengerFlow) {
  cout << "\n========== 客流预测 ==========" << endl;

  // 预测成都东站未来3天客流
  auto prediction = passengerFlow.predictFlow("CD001", 3);
  cout << "成都东站未来3天客流预测:" << endl;
  for (size_t i = 0; i < prediction.size(); ++i) {
    cout << "第" << (i + 1) << "天: " << prediction[i] << " 人次" << endl;
  }

  cout << endl;

  // 预测川渝方向流量
  auto directionPrediction = passengerFlow.predictDirectionalFlow("川->渝", 3);
  cout << "川->渝方向未来3天流量预测:" << endl;
  for (size_t i = 0; i < directionPrediction.size(); ++i) {
    cout << "第" << (i + 1) << "天: " << directionPrediction[i] << " 人次"
         << endl;
  }
}

// 生成日报告
void generateDailyReport(const PassengerFlow &passengerFlow) {
  cout << "\n========== 日报告 ==========" << endl;
  Date today(2024, 12, 15);
  cout << passengerFlow.generateFlowReport(today) << endl;
}

int main() {
  // 设置Windows控制台支持UTF-8编码
#ifdef _WIN32
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
#endif

  cout << "川渝地区轨道交通客流数据分析与展示系统" << endl;
  cout << "版本: 1.0" << endl;
  cout << "开发团队: [学生姓名]" << endl << endl;

  // 初始化数据
  vector<shared_ptr<Station>> stations;
  vector<shared_ptr<Route>> routes;
  vector<shared_ptr<Train>> trains;
  PassengerFlow passengerFlow;
  FileManager fileManager("data");

  if (!fileManager.importAllData(stations, routes, trains, passengerFlow)) {
    cout << "未找到数据文件，初始化示例数据..." << endl;
    initSampleData(stations, routes, trains, passengerFlow);
    fileManager.exportAllData(stations, routes, trains, passengerFlow);
  }

  int choice;
  do {
    showMenu();
    cin >> choice;

    switch (choice) {
    case 1:
      showStations(stations);
      break;
    case 2:
      showRoutes(routes);
      break;
    case 3:
      showTrains(trains);
      break;
    case 4:
      showFlowStatistics(passengerFlow);
      break;
    case 5:
      analyzeStationRanking(passengerFlow);
      break;
    case 6:
      analyzeDirectionalFlow(passengerFlow);
      break;
    case 7:
      analyzeLoadFactor(passengerFlow);
      break;
    case 8:
      predictFlow(passengerFlow);
      break;
    case 9:
      generateDailyReport(passengerFlow);
      break;
    case 0:
      cout << "感谢使用！再见！" << endl;
      break;
    default:
      cout << "无效选择，请重新输入！" << endl;
      break;
    }

    if (choice != 0) {
      cout << "\n按回车键继续...";
      cin.ignore();
      cin.get();
    }

  } while (choice != 0);

  fileManager.exportAllData(stations, routes, trains, passengerFlow);

  return 0;
}