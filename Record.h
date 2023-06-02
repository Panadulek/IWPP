#ifndef RECORD_H
#define RECORD_H
#include <cstdint>
#include <string>
constexpr uint8_t arraySize = 6;

struct Record // reprezentacja linii z pliku csv.
{
    std::string _date;
    uint64_t id;
    double _conductivity[arraySize];
};
#endif // RECORD_H
