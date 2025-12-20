#include "facilitysystem.h"
#include "buildingdialog.h"
#include "roomdialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QHash>

FacilitySystem::FacilitySystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadBuildings();
    loadRooms();
}

void FacilitySystem::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);
    
    setupBuildingsTab();
    setupRoomsTab();

    m_tabWidget->addTab(m_buildingsTab, "Buildings");
    m_tabWidget->addTab(m_roomsTab, "Rooms & Labs");

    layout->addWidget(m_tabWidget);
}

void FacilitySystem::setupBuildingsTab()
{
    m_buildingsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_buildingsTab);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();
    m_btnAddBuilding = new QPushButton("Add Building", this);
    m_btnEditBuilding = new QPushButton("Edit Building", this);
    m_btnDeleteBuilding = new QPushButton("Delete Building", this);
    toolbar->addWidget(m_btnAddBuilding);
    toolbar->addWidget(m_btnEditBuilding);
    toolbar->addWidget(m_btnDeleteBuilding);
    toolbar->addStretch();
    
    connect(m_btnAddBuilding, &QPushButton::clicked, this, &FacilitySystem::addBuilding);
    connect(m_btnEditBuilding, &QPushButton::clicked, this, &FacilitySystem::editBuilding);
    connect(m_btnDeleteBuilding, &QPushButton::clicked, this, &FacilitySystem::deleteBuilding);

    // Table
    m_buildingsTable = new QTableView(this);
    m_buildingsModel = new QStandardItemModel(this);
    m_buildingsModel->setHorizontalHeaderLabels({"ID", "Name", "Code", "Location"});
    m_buildingsTable->setModel(m_buildingsModel);
    m_buildingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_buildingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_buildingsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addLayout(toolbar);
    layout->addWidget(m_buildingsTable);
}

void FacilitySystem::setupRoomsTab()
{
    m_roomsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_roomsTab);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();
    m_btnAddRoom = new QPushButton("Add Room/Lab", this);
    m_btnEditRoom = new QPushButton("Edit Room/Lab", this);
    m_btnDeleteRoom = new QPushButton("Delete Room/Lab", this);
    toolbar->addWidget(m_btnAddRoom);
    toolbar->addWidget(m_btnEditRoom);
    toolbar->addWidget(m_btnDeleteRoom);
    toolbar->addStretch();

    connect(m_btnAddRoom, &QPushButton::clicked, this, &FacilitySystem::addRoom);
    connect(m_btnEditRoom, &QPushButton::clicked, this, &FacilitySystem::editRoom);
    connect(m_btnDeleteRoom, &QPushButton::clicked, this, &FacilitySystem::deleteRoom);

    // Table
    m_roomsTable = new QTableView(this);
    m_roomsModel = new QStandardItemModel(this);
    m_roomsModel->setHorizontalHeaderLabels({"ID", "Building", "Room Number", "Type", "Capacity"});
    m_roomsTable->setModel(m_roomsModel);
    m_roomsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_roomsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_roomsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addLayout(toolbar);
    layout->addWidget(m_roomsTable);
}

void FacilitySystem::loadBuildings()
{
    m_buildingsModel->removeRows(0, m_buildingsModel->rowCount());
    QList<Building> buildings = m_repository.getAllBuildings();
    
    for(const auto& b : buildings) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(b.id));
        row << new QStandardItem(b.name);
        row << new QStandardItem(b.code);
        row << new QStandardItem(b.location);
        m_buildingsModel->appendRow(row);
    }
}

void FacilitySystem::loadRooms()
{
    m_roomsModel->removeRows(0, m_roomsModel->rowCount());
    QList<Room> rooms = m_repository.getAllRooms();
    
    QList<Building> buildings = m_repository.getAllBuildings();
    QHash<int, QString> buildingNames;
    for(const auto& b : buildings) buildingNames[b.id] = b.name;

    for(const auto& r : rooms) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(r.id));
        row << new QStandardItem(buildingNames.value(r.buildingId, "Unknown"));
        row << new QStandardItem(r.roomNumber);
        row << new QStandardItem(r.type);
        row << new QStandardItem(QString::number(r.capacity));
        m_roomsModel->appendRow(row);
    }
}

void FacilitySystem::addBuilding()
{
    BuildingDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted) {
        Building b = dialog.getBuilding();
        if(m_repository.addBuilding(b)) {
            loadBuildings();
        } else {
            QMessageBox::warning(this, "Error", "Failed to add building.");
        }
    }
}

void FacilitySystem::editBuilding()
{
    QModelIndex index = m_buildingsTable->currentIndex();
    if(!index.isValid()) return;
    
    int row = index.row();
    int id = m_buildingsModel->item(row, 0)->text().toInt();
    Building b = m_repository.getBuildingById(id);
    
    BuildingDialog dialog(this, &b);
    if(dialog.exec() == QDialog::Accepted) {
        Building updated = dialog.getBuilding();
        if(m_repository.updateBuilding(updated)) {
            loadBuildings();
            loadRooms(); // Building name might have changed
        } else {
            QMessageBox::warning(this, "Error", "Failed to update building.");
        }
    }
}

void FacilitySystem::deleteBuilding()
{
    QModelIndex index = m_buildingsTable->currentIndex();
    if(!index.isValid()) return;
    
    int row = index.row();
    int id = m_buildingsModel->item(row, 0)->text().toInt();
    
    if(QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this building? All rooms within it will be deleted.") == QMessageBox::Yes) {
        if(m_repository.deleteBuilding(id)) {
            loadBuildings();
            loadRooms();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete building.");
        }
    }
}

void FacilitySystem::addRoom()
{
    QList<Building> buildings = m_repository.getAllBuildings();
    if(buildings.isEmpty()) {
        QMessageBox::information(this, "Info", "Please add a building first.");
        return;
    }

    RoomDialog dialog(buildings, this);
    if(dialog.exec() == QDialog::Accepted) {
        Room r = dialog.getRoom();
        if(m_repository.addRoom(r)) {
            loadRooms();
        } else {
            QMessageBox::warning(this, "Error", "Failed to add room.");
        }
    }
}

void FacilitySystem::editRoom()
{
    QModelIndex index = m_roomsTable->currentIndex();
    if(!index.isValid()) return;
    
    int row = index.row();
    int id = m_roomsModel->item(row, 0)->text().toInt();
    
    QList<Room> allRooms = m_repository.getAllRooms();
    Room r;
    bool found = false;
    for(const auto& room : allRooms) {
        if(room.id == id) {
            r = room;
            found = true;
            break;
        }
    }
    
    if(found) {
        QList<Building> buildings = m_repository.getAllBuildings();
        RoomDialog dialog(buildings, this, &r);
        if(dialog.exec() == QDialog::Accepted) {
            Room updated = dialog.getRoom();
            if(m_repository.updateRoom(updated)) {
                loadRooms();
            } else {
                QMessageBox::warning(this, "Error", "Failed to update room.");
            }
        }
    }
}

void FacilitySystem::deleteRoom()
{
    QModelIndex index = m_roomsTable->currentIndex();
    if(!index.isValid()) return;
    
    int row = index.row();
    int id = m_roomsModel->item(row, 0)->text().toInt();
    
    if(QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this room?") == QMessageBox::Yes) {
        if(m_repository.deleteRoom(id)) {
            loadRooms();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete room.");
        }
    }
}
