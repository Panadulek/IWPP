#ifndef CHART_H
#define CHART_H
#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QSplineSeries>
#include <cstdint>
#include <array>
#include "Record.h"
#include <QValueAxis>
#include <QDateTime>
#include "Callback.h"
#include <QKeyEvent>
#include "ChartAlgo.h"
class Chart : public QObject
{
    Q_OBJECT

    using LINE_TYPE = QLineSeries;
    std::unique_ptr<QChartView> m_view;
    std::array<LINE_TYPE*, arraySize> m_arrSeries;
    std::unique_ptr<QDateTime> m_firstDate;
    bool m_visible[arraySize];
    QValueAxis* X;
    QValueAxis* Y;
    std::vector<std::pair<uint8_t, std::pair<double, double>>> m_ranges;
    std::vector<std::pair<uint8_t, std::pair<double, double>>> m_NoDimRanges;
    AbstractCallback* m_cb;
    bool m_dim;
    bool m_isValid = false;
    template<bool isDim>
    void displayStandardData(const std::vector<Record>& records, const uint64_t time)
    {
        int64_t _time = 0;
        auto& refRange = isDim ? m_ranges : m_NoDimRanges;
        if(isDim)
            m_dim = true;
        else
            m_dim = false;
        bool isFilled = false;

        for(const auto& record : records)
        {
            _time = getTimeInSeconds(record._date, m_firstDate);
            if(time && _time % time != 0)
                continue;

            for(uint64_t i = 0; i < arraySize; i++)
            {
                double conductivity = (*m_cb)(record._conductivity[i], _time);

                if(!isFilled)
                {
                        std::pair<double, double> ranges = std::make_pair(conductivity, conductivity);
                        refRange.emplace_back(i, ranges);
                }
                else
                {
                    if(refRange[i].second.first < conductivity)
                        refRange[i].second.first = conductivity;
                    if(refRange[i].second.second > conductivity)
                        refRange[i].second.second = conductivity;
                }
                m_arrSeries[i]->append(_time/1000.0, conductivity);
            }
            isFilled = true;
        }
    }

void createAxies()
{
    Y = new QValueAxis();
    X = new QValueAxis();
    Y->setTitleText("Przewodnictwo [ uS/cm ]");
    Y->setLabelsAngle(-90);
    Y->setTitleVisible(true);
    X->setTitleText("Przesuniecie w czasie w [s]");
    X->setLabelsAngle(-90);
    X->setTitleVisible(true);
    m_view->chart()->addAxis(X, Qt::AlignBottom);
    m_view->chart()->addAxis(Y, Qt::AlignLeft);
}

bool isAxiesExists()
{
    auto list = m_view->chart()->axes();
    if(list.size() == 2)
        return true;
    return false;
}
void setSeriesSettings()
{
    m_arrSeries[0]->setColor(QColor(QRgb(0xFFFF0000)));
    m_arrSeries[0]->setName("Przewodnictwo_1");
    m_arrSeries[1]->setColor(QColor(QRgb(0xFF00FF00)));
    m_arrSeries[1]->setName("Przewodnictwo_2");
    m_arrSeries[2]->setColor(QColor(QRgb(0xFF0000FF)));
    m_arrSeries[2]->setName("Przewodnictwo_3");
    m_arrSeries[3]->setColor(QColor(QRgb(0xFFFFFF00)));
    m_arrSeries[3]->setName("Przewodnictwo_4");
    m_arrSeries[4]->setColor(QColor(QRgb(0xFFFF00FF)));
    m_arrSeries[4]->setName("Przewodnictwo_5");
    m_arrSeries[5]->setColor(QColor(QRgb(0xFF00FFFF)));
    m_arrSeries[5]->setName("Przewodnictwo_6");
}

void changeState(unsigned id)
{
    if(!m_arrSeries[id] || !m_isValid)
        return;
    if(m_arrSeries[id]->isVisible())
    {
        m_visible[id] = false;
        m_arrSeries[id]->hide();
    }
    else
    {
        m_visible[id] = true;
        m_arrSeries[id]->show();
    }
    updateRanges();
}


void updateDatas(const std::vector<Record>& records, const uint64_t time, bool noDimValue)
{
    m_view->chart()->removeAxis(X);
    m_view->chart()->removeAxis(Y);

    for(uint64_t i = 0; i < m_arrSeries.size(); i++)
    {
        m_visible[i] = true;
        m_view->chart()->removeSeries(m_arrSeries[i]);
        m_arrSeries[i]->detachAxis(X);
        m_arrSeries[i]->detachAxis(Y);
        delete m_arrSeries[i];
        m_arrSeries[i] = new LINE_TYPE();
    }

    delete X;
    delete Y;
    createAxies();
    setSeriesSettings();
    if(!noDimValue)
        displayStandardData<true>(records, time);
    else
    {
        std::vector<Record> noDimRecord = createVectorWithoutDim(records);
        displayStandardData<false>(noDimRecord, time);
    }
    updateRanges();
    for(uint64_t i = 0; i < m_arrSeries.size(); i++)
    {
        m_view->chart()->addSeries(m_arrSeries[i]);
        m_arrSeries[i]->attachAxis(X);
        m_arrSeries[i]->attachAxis(Y);
        m_arrSeries[i]->show();
    }
}


public:
    explicit Chart() : m_firstDate(nullptr), X(nullptr), Y(nullptr)
    {
        m_cb = new BaseCallback();
        m_view = std::make_unique<QChartView>();
        m_view->chart()->setTitle("Wykres Przewodnictwa");
        for(uint64_t i = 0; i < arraySize; i++)
        {
            m_visible[i] = true;
            m_arrSeries[i] = new LINE_TYPE;
        }
        setSeriesSettings();
        createAxies();
    }

    void clear()
    {
        m_ranges.clear();
        m_NoDimRanges.clear();
    }

    explicit operator QChartView*() {return m_view.get();}
    template<bool IsDim>
    double findMaxRange() // szuka maksymalnej wartosci ze wszystkich szukanych serii
    {
        double ret = 0;
         auto& refRange = IsDim ? m_ranges : m_NoDimRanges;

        for(uint64_t i = 0; i < refRange.size(); i++)
        {
            if(m_visible[i])
            {

                ret = refRange[i].second.first;
                break;
            }
        }

        for(uint64_t i = 0; i < refRange.size(); i++)
        {
            if(m_visible[i] && ret < refRange[i].second.first)
                ret = refRange[i].second.first;
        }
        return ret;
    }

    template<bool IsDim>
    double findMinRange() // szuka minimalnej wartosci ze wszystkich widzianych serii
    {
        double ret = 0;
        auto refRange = IsDim ? m_ranges : m_NoDimRanges;
        for(uint64_t i = 0; i < refRange.size(); i++)
        {
            if(m_visible[i])
            {
                ret = refRange[i].second.second;
                break;
            }
        }

        for(uint64_t i = 0; i < refRange.size(); i++)
        {
            if(m_visible[i] && ret > refRange[i].second.second)
                ret = refRange[i].second.second;
        }
        return ret;
    }
    void updateRanges() // updateuje zakres danych na wykresie
    {
        if(m_arrSeries[0]->points().empty())
           return;
        const qreal maxX = m_arrSeries[0]->points().last().x();
        X->setRange(0, maxX);
        if(m_dim)
        {
            const double min = findMinRange<true>();
            const double max = findMaxRange<true>();
            Y->setRange(min, max);
        }
        else
        {
            const double min = findMinRange<false>();
            const double max = findMaxRange<false>();
            Y->setRange(min, max);
        }
    }

    void setCallback(AbstractCallback* cb)
    {
        if(!cb)
            return;
        delete m_cb;
        m_cb = cb;
    }
public slots:
    void createChart(const std::vector<Record>& records, const uint64_t time, bool noDimValue) // tworzy wykres
    {

        if(records.empty())
            return;
        if(m_isValid)
        {
            updateDatas(records, time, noDimValue);
            return;
        }

        for(uint64_t i = 0; i < m_arrSeries.size(); i++)
        {
            m_visible[i] = true;
            m_arrSeries[i]->clear();
        }

        if(!noDimValue)
            displayStandardData<true>(records, time);
        else
        {
            std::vector<Record> noDimRecord = createVectorWithoutDim(records);
            displayStandardData<false>(noDimRecord, time);
        }
        if(!m_isValid)
        {
            for(uint64_t i = 0; i < m_arrSeries.size(); i++)
            {
                m_view->chart()->addSeries(m_arrSeries[i]);
                m_arrSeries[i]->attachAxis(X);
                m_arrSeries[i]->attachAxis(Y);
                m_arrSeries[i]->show();
            }

        }
        else
        {
            for(uint64_t i = 0; i < m_arrSeries.size(); i++)
            {
                m_arrSeries[i]->show();
            }
            m_view->update();
        }
        updateRanges();
        m_isValid = true;

    }

    void changeStateSerie1(int) // ukrywa albo pokazuje serie 1 (nastepne funkcje robia to samo dla innych serii).
    {
        changeState(0);
    }

    void changeStateSerie2(int)
    {
      changeState(1);
    }

    void changeStateSerie3(int)
    {
       changeState(2);
    }

    void changeStateSerie4(int)
    {
        changeState(3);
    }

    void changeStateSerie5(int)
    {
        changeState(4);
    }

    void changeStateSerie6(int)
    {
        changeState(5);
    }

};

#endif // CHART_H
