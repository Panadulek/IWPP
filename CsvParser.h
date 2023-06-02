#ifndef CSVPARSER_H
#define CSVPARSER_H
#include <cstdio>
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <string_view>
#include "Record.h"
#include <QObject>
#define GET_OPT(X) static_cast<uint64_t>(X)
#define IS_FOUND(X) X != std::string::npos
class CsvParser // klasa parsujaca plik csv.
{
    enum class OPT : uint64_t
    {
        ID = 0,
        DATE,
        CONDUCTIVITY_1,
        CONDUCTIVITY_2,
        CONDUCTIVITY_3,
        CONDUCTIVITY_4,
        CONDUCTIVITY_5,
        CONDUCTIVITY_6,
        END_OPT
    };

public:

    CsvParser()
    {

    }
    void clear()
    {
        clearArray();
        m_datas.clear();
        m_isSettedOption = false;
        m_isValid = false;
    }
    bool parse(const char* filename)
    {
        clear();
        constexpr uint64_t BUFF_SIZE = 4096;
        FILE* fd = fopen(filename, "r+");
        char buff[BUFF_SIZE];
        while(true)
        {
            std::string line;
            char* result = fgets(buff, BUFF_SIZE, fd);
            if(result == nullptr)
            {
                if(feof(fd))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            line += buff;
            if(line.find("\n"))
            {
                auto record = parseLine(line.c_str());
                if(record.has_value())
                    m_datas.push_back(record.value());
                line.clear();
            }
        }

    }
    std::vector<Record>& getRefRecords()
    {
        return m_datas;
    }
    std::vector<Record> getRecords()
    {
        return m_datas;
    }
    bool isValid()
    {
        return m_isValid;
    }
private:
        void clearArray()
        {
            for(auto& opt : m_opts)
            {
                opt = false;
            }
        }
        bool m_isValid = false;
        std::vector<Record> m_datas;
        bool m_isSettedOption = false;
        std::array<bool, static_cast<uint64_t>(OPT::END_OPT)> m_opts;
        void setOptions(char* opt)
        {
            char* it = opt;
            while(*it != '\0' && *it != '\n')
            {
                if(*it >= 'a' && *it <= 'z')
                    *it = *it - ('a' - 'A');
                it++;
            }

            if(strstr(opt, "NR"))
            {
                m_opts[GET_OPT(OPT::ID)] = true;
            }
            if(strstr(opt, "DATA"))
            {
                m_opts[GET_OPT(OPT::DATE)] = true;
            }
            if(strstr(opt,"PRZEWODNOSC 1"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_1)] = true;
            }
            if(strstr(opt,"PRZEWODNOSC 2"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_2)] = true;
            }
            if(strstr(opt,"PRZEWODNOSC 3"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_3)] = true;
            }
            if(strstr(opt,"PRZEWODNOSC 4"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_4)] = true;
            }
            if(strstr(opt,"PRZEWODNOSC 5"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_5)] = true;
            }
            if(strstr(opt, "PRZEWODNOSC 6"))
            {
                m_opts[GET_OPT(OPT::CONDUCTIVITY_6)] = true;
            }

        }
        std::optional<Record> parseLine(std::string_view str)
        {
            if(!m_isSettedOption)
            {
                setOptions(const_cast<char*>(&str[0]));
                m_isSettedOption = true;
                return std::nullopt;
            }
            auto parseNumber = [](std::string& str, const char* tok)
            {
                for(auto& ch : str )
                {
                    if(ch == *tok)
                        break;
                    if(ch == ',')
                        ch = '.';
                }
            };
            char tok[] = {'\0', '\0'};
            uint64_t pos = 0;
            Record ret;
            std::array<bool, static_cast<uint64_t>(OPT::END_OPT)> opts = m_opts;
            while(true)
            {
                for(size_t i = 0; i < str.size() && *tok == '\0'; i++)
                {
                    if(str[i] == ';' || str[i] == ',')
                        *tok = str[i];
                    else if(str[i] == ',')
                        *tok = str[i];
                }
                uint64_t newPos = str.find(tok, pos);
                std::string element;
                if(newPos == std::string::npos)
                {
                    element = str.begin() + pos;
                }
                else
                {
                     element = std::string(str.begin() + pos, newPos - pos);
                }
                pos = newPos + 1;
                if(opts[GET_OPT(OPT::ID)])
                {
                    ret.id = strtoull(element.data(), nullptr, 10);
                    opts[GET_OPT(OPT::ID)] = false;
                }
                else if(opts[GET_OPT(OPT::DATE)])
                {
                    ret._date = element;
                    opts[GET_OPT(OPT::DATE)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_1)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[0] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_1)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_2)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[1] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_2)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_3)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[2] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_3)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_4)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[3] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_4)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_5)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[4] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_5)] = false;
                }
                else if(opts[GET_OPT(OPT::CONDUCTIVITY_6)])
                {
                    parseNumber(element, tok);
                    ret._conductivity[5] = strtod(element.data(), nullptr);
                    opts[GET_OPT(OPT::CONDUCTIVITY_6)] = false;
                }
                if(newPos == std::string::npos)
                {
                    m_isValid = true;
                    return ret;
                }
            }
        }


};

#endif // CSVPARSER_H
