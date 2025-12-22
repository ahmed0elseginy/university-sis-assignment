#ifndef FACILITYSYSTEM_H
#define FACILITYSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include "../../modules/facility/facilityrepository.h"

class FacilitySystem : public QWidget {
    Q_OBJECT
public:
    explicit FacilitySystem(QWidget *parent = nullptr);

private slots:
    void loadBuildings();
    void loadRooms();
    void addBuilding();
    void editBuilding();
    void deleteBuilding();
    void addRoom();
    void editRoom();
    void deleteRoom();

private:
    FacilityRepository m_repository;
    QTabWidget *m_tabWidget;

    // Buildings Tab
    QWidget *m_buildingsTab;
    QTableView *m_buildingsTable;
    QStandardItemModel *m_buildingsModel;
    QPushButton *m_btnAddBuilding;
    QPushButton *m_btnEditBuilding;
    QPushButton *m_btnDeleteBuilding;

    // Rooms Tab
    QWidget *m_roomsTab;
    QTableView *m_roomsTable;
    QStandardItemModel *m_roomsModel;
    QPushButton *m_btnAddRoom;
    QPushButton *m_btnEditRoom;
    QPushButton *m_btnDeleteRoom;

    void setupUi();
    void setupBuildingsTab();
    void setupRoomsTab();
};

#endif // FACILITYSYSTEM_H
