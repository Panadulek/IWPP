#ifndef CHARTALGO_H
#define CHARTALGO_H
#include <vector>
#include "Record.h"
#include <QDateTime>


std::vector<Record> createVectorWithoutDim(const std::vector<Record>& records);
int64_t getTimeInSeconds(std::string str, std::unique_ptr<QDateTime>& m_firstDate); // zwraca odstep czasowy od poczatkowej daty do podanej daty [ms].
#endif // CHARTALGO_H
