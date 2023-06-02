#ifndef EXCELMANAGER_H
#define EXCELMANAGER_H
#include "xlsxdocument.h"
#include "Record.h"
#include <string>
#include <QDateTime>
#include <memory>
#include <cstdint>
class ExcelManager
{
std::string m_filename;
QXlsx::Document m_document;
void preaperDocument();
uint64_t m_maxRow;
uint64_t m_timeOffset = 0;
bool saveFile();


public:
    explicit ExcelManager(const std::string& filename);
    void exportRecords(const std::vector<Record>& record);
    void exportNoDimRecords(const std::vector<Record>& record);
    void drawChart();
    void setTimeOffset(uint64_t t) {m_timeOffset = t;}
    ~ExcelManager()
    {
        saveFile();
    }
};

#endif // EXCELMANAGER_H
