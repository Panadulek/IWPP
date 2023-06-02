#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "Chart.h"
#include <QPushButton>
#include <CsvParser.h>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include "excelmanager.h"
class MainWindow : public QMainWindow // klasa reprezentujaca wyglad aplikacji.
{
    Q_OBJECT
    QWidget* m_mainWidget;
    Chart m_view;
    QVBoxLayout* m_mainLayout;
    QPushButton* m_createSeries = nullptr;
    QPushButton* m_excelExporter;
    QHBoxLayout* m_layout;
    QHBoxLayout* m_upLayout;
    QLabel* m_label;
    QCheckBox* m_cbs[6];
    QSpinBox* m_box;
    QCheckBox* m_dimValue;
    CsvParser m_parser;

public:
    MainWindow(QWidget *parent = nullptr)
    {
        m_mainWidget = new QWidget(parent);
        m_mainLayout = new QVBoxLayout();
        m_layout = new QHBoxLayout();
        m_upLayout = new QHBoxLayout();
        m_createSeries = new QPushButton("analiza danych");
        m_excelExporter = new QPushButton("Exportuj wykres do excel'a");
        m_box = new QSpinBox();
        m_box->setMaximum(0x7FFFFFFF);
        m_label = new QLabel();
        m_dimValue = new QCheckBox(nullptr);
        m_label->setText("Podaj offset w ms, 0 oznacza brak offestu.");
        setupConnections();
        setupPos();

    }
    void setupPos()
    {
        this->setCentralWidget(m_mainWidget);
        m_mainWidget->setLayout(m_mainLayout);
        m_mainLayout->addLayout(m_upLayout);
        m_upLayout->addWidget(m_createSeries);
        m_upLayout->addWidget(m_excelExporter);
        m_upLayout->addWidget(m_box);
        m_upLayout->addWidget(m_label);
        m_mainLayout->addWidget(static_cast<QChartView*>(m_view), 10);
        m_mainLayout->addLayout(m_layout);

        m_cbs[0] = new QCheckBox();
        QLabel* label = new QLabel();
        label->setText("Seria_1");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[0]);
        QObject::connect(m_cbs[0], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie1);

        m_cbs[1] = new QCheckBox();
        label = new QLabel();
        label->setText("Seria_2");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[1]);
        QObject::connect(m_cbs[1], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie2);

        m_cbs[2] = new QCheckBox();
        label = new QLabel();
        label->setText("Seria_3");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[2]);
        QObject::connect(m_cbs[2], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie3);

        m_cbs[3] = new QCheckBox();
        label = new QLabel();
        label->setText("Seria_4");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[3]);
        QObject::connect(m_cbs[3], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie4);

        m_cbs[4] = new QCheckBox();
        label = new QLabel();
        label->setText("Seria_5");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[4]);
        QObject::connect(m_cbs[4], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie5);

        m_cbs[5] = new QCheckBox();
        label = new QLabel();
        label->setText("Seria_6");
        m_layout->addWidget(label);
        m_layout->addWidget(m_cbs[5]);
        QObject::connect(m_cbs[5], &QCheckBox::stateChanged, &m_view, &Chart::changeStateSerie6);

        label = new QLabel();
        label->setText("Wartosci bezwymiarowe");
        m_layout->addWidget(label);
        m_layout->addWidget(m_dimValue);

    }
    void setupConnections()
    {

        QObject::connect(m_excelExporter, &QPushButton::clicked, this, [&]()
        {
            if(!m_parser.isValid())
                return;
            QDateTime time = QDateTime::currentDateTime();
            QString timeStr = time.toString();
            timeStr += "_Iwpp.xlsx";
            timeStr.replace(QRegularExpression(" |:"), "_");
            ExcelManager manager(timeStr.toStdString());
           // manager.setTimeOffset()
            switch(m_dimValue->checkState())
            {

                case Qt::CheckState::Unchecked:
                    manager.exportRecords(m_parser.getRefRecords());
                break;

                default:
                case Qt::CheckState::Checked:
                    manager.exportNoDimRecords(m_parser.getRefRecords());
                break;
            }
            manager.drawChart();

        });

        QObject::connect(m_createSeries, &QPushButton::clicked, this, [&]()
        {
           const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("CSV Files (*.csv)"));
           if(m_parser.isValid())
           {
                m_parser.clear();
                m_view.clear();

           }
           if(m_parser.parse(fileName.toStdString().c_str()))
           {
                uint64_t offset = m_box->value();
                for(int i = 0; i < 6; i++)
                    m_cbs[i]->setCheckState(Qt::CheckState::Checked);
                m_view.createChart(m_parser.getRefRecords(), offset, false);
                emit drawingChart();
           }

        });
        QObject::connect(m_dimValue, &QCheckBox::stateChanged, this, [&](int state){
            if(!m_parser.isValid())
                return;
            for(int i = 0; i < 6; i++)
                m_cbs[i]->setCheckState(Qt::CheckState::Checked);
            uint64_t offset = m_box->value();
            m_view.createChart(m_parser.getRefRecords(), offset, static_cast<bool>(state));
            emit drawingChart();
        });
    }
    ~MainWindow();
signals:
    void drawingChart();

};
#endif // MAINWINDOW_H
