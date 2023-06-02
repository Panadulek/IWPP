#include "ChartAlgo.h"
std::vector<Record> createVectorWithoutDim(const std::vector<Record>& records)
{
    std::vector<Record> noDimRecord;
    double tmps[arraySize];
    for(uint64_t i = 0; i < arraySize; i++)
    {
        double var1 = records.at(records.size() - 1)._conductivity[i];
        double var2 = records[0]._conductivity[i];
        double result = var1 - var2;
        tmps[i] = result;
    }
    for(const auto& record : records)
    {
        Record newRecord = record;
        for(uint64_t i = 0; i < arraySize; i++)
        {
            newRecord._conductivity[i] = (record._conductivity[i] - records.begin()->_conductivity[i]) / tmps[i];
        }
        noDimRecord.push_back(newRecord);
    }
    return noDimRecord;
}
int64_t getTimeInSeconds(std::string str, std::unique_ptr<QDateTime>& m_firstDate) // zwraca odstep czasowy od poczatkowej daty do podanej daty [ms].
{
    size_t pos = str.find(",");
    if(pos == std::string::npos)
        pos = str.find(".");
    std::string strMs = "";
    if(pos != std::string::npos)
    {
        strMs = str.substr(pos + 1);
        str[pos] = '\0';
    }
    uint64_t ms = 0;
    if(strMs.size() > 0)
    {
        ms = std::stoull(strMs);
    }
    QDateTime date = QDateTime::fromString(str.c_str(), "yyyy-MM-dd HH:mm:ss");
    if(date.isValid() && ms)
        date = date.addMSecs(ms);
    if(!m_firstDate)
    {
        m_firstDate = std::make_unique<QDateTime>(date);
        return 0;
    }
    return m_firstDate->msecsTo(date);
}
