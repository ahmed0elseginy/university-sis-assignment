#ifndef REPORTSSYSTEM_H
#define REPORTSSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include "../basesystemwidget.h"

class ReportsSystem : public BaseSystemWidget
{
    Q_OBJECT

public:
    explicit ReportsSystem(QWidget *parent = nullptr);

private slots:
    void onReportTypeChanged(int index);
    void onGenerateReport();
    void onExportReport();

private:
    void setupUi();
    void generateStudentReport();
    void generateCourseReport();
    void generateAttendanceReport();
    void generateFinancialReport();
    
    QComboBox *m_reportTypeCombo;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QPushButton *m_generateBtn;
    QPushButton *m_exportBtn;
    QTextEdit *m_reportText;
    QTableView *m_reportTable;
    QStandardItemModel *m_reportModel;
};

#endif // REPORTSSYSTEM_H

