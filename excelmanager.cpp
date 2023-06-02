#include "excelmanager.h"
#include "ChartAlgo.h"
#include "xlsxchart.h"
#include <memory>
#include <QDateTime>
#include <cstdint>

ExcelManager::ExcelManager(const std::string& filename) : m_filename(filename), m_maxRow(0)
{

}
void ExcelManager::drawChart()
{
    QXlsx::Chart* chart = m_document.insertChart(20, 1, QSize(300, 300));
    chart->setChartType(QXlsx::Chart::CT_LineChart);
    std::string range = "C1:H";
    range += std::to_string(m_maxRow);
    chart->addSeries(QXlsx::CellRange(range.c_str()), nullptr, true, true, false);
}
bool ExcelManager::saveFile()
{
    return m_document.saveAs(m_filename.c_str());
}
void ExcelManager::exportRecords(const std::vector<Record>& records)
{
    preaperDocument();
    uint64_t row = 2;
    std::unique_ptr<QDateTime> m_firstDate(nullptr);
    bool firstIt = true;
    for(auto& record : records)
    {
        int64_t _time = getTimeInSeconds(record._date, m_firstDate);
        if(m_timeOffset && _time % m_timeOffset != 0)
        {
            continue;
        }
        uint64_t col = 1;
        m_document.write(row, col++, record.id);
        m_document.write(row, col++, (_time/1000.0));
        uint64_t size = arraySize + col;
        for(uint64_t i = col; i < size; i++)
        {
            m_document.write(row, i, record._conductivity[i - col]);
        }
        row++;
    }
    m_maxRow = row;
    saveFile();
    return;
}
void ExcelManager::exportNoDimRecords(const std::vector<Record>& records)
{
    std::vector<Record>&& _records = createVectorWithoutDim(records);
    exportRecords(_records);
}





// private
void ExcelManager::preaperDocument()
{
    m_document.write(1, 1, "Nr");
    m_document.write(1, 2, "Data");
    const std::string baseStr = "PRZEWODNOSC ";
    for(uint8_t i = 0; i < arraySize ; i++)
    {
        const std::string str = baseStr + std::to_string(i);
        m_document.write(1, i + 3, str.c_str());
    }
}
