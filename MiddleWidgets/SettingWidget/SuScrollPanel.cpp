﻿#include "SuScrollPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QScrollBar>

SuScrollPanel::SuScrollPanel(QWidget *parent)
    : QWidget(parent)
{
    this->setMouseTracking(true);//详见 BesFramelessWidget.h 注释

    initEntity();
    initLayout();
    initConnection();
}

SuScrollPanel::~SuScrollPanel()
{

}

QVector<ISettingUnit *> &SuScrollPanel::getSettingUnits()
{
    if(settings.size() == 0)
    {
        ISettingUnit* suLyricMaker = new SuLyricMaker();
        ISettingUnit* suDemo2 = new SuDemo2();
        ISettingUnit* suDemo3 = new SuDemo1();
        ISettingUnit* suDemo4 = new SuDemo2();
        ISettingUnit* suDemo11 = new SuDemo1();
        ISettingUnit* suDemo12 = new SuDemo2();
        ISettingUnit* suDemo13 = new SuDemo1();
        ISettingUnit* suDemo14 = new SuDemo2();

        settings.push_back(suLyricMaker);
        settings.push_back(suDemo2);
        settings.push_back(suDemo3);
        settings.push_back(suDemo4);
        settings.push_back(suDemo11);
        settings.push_back(suDemo12);
        settings.push_back(suDemo13);
        settings.push_back(suDemo14);
    }

    return settings;
}

void SuScrollPanel::OnScrollToIndex(int index)
{
    int devidedStep = nPageStep / settings.size();

    int pos = 0;
    for(int i = 0; i< settings.size(); i++ )
    {
        if(i < index)
        {
            pos += settings.at(i)->getUnitHeight();
        }
        else
            break;
    }

    int sub = index <= 1 ? 0: devidedStep * (index-1);

    nTargetPos = pos - sub;

    if(!scrollTimer->isActive())
        scrollTimer->start();
}

void SuScrollPanel::OnSrcollTimerTimeout()
{
    int minStep = 40;
    int maxStep = 200;

    int current = scrollAreaSetting->verticalScrollBar()->value();

    if(current == nTargetPos)
        scrollTimer->stop();
    else
    {
        if(current < nTargetPos)
        {
            int step = (nTargetPos - current)*3/4;
            if(step > maxStep)
                step = maxStep;
            if(step < minStep)
                step = minStep;

            current+=step;
            if(current > nTargetPos)
                current = nTargetPos;
        }
        else
        {
            int step = (current - nTargetPos)*3/4;
            if(step > maxStep)
                step = maxStep;
            if(step < minStep)
                step = minStep;

            current-=step;
            if(current < nTargetPos)
                current = nTargetPos;
        }

        scrollAreaSetting->verticalScrollBar()->setValue(current);
    }
}

void SuScrollPanel::initEntity()
{
    scrollTimer = new QTimer(this);
    scrollTimer->setInterval(1);
}

void SuScrollPanel::initLayout()
{
    settingUnitContainer = new QWidget(this);
    settingUnitContainer->setMouseTracking(true);//详见 BesFramelessWidget.h 注释
    settingUnitContainer->setObjectName("settingUnitContainer");
    settingUnitContainer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    scrollAreaSetting = new QScrollArea(this);
    scrollAreaSetting->setWidgetResizable(true);
    scrollAreaSetting->setWidget(settingUnitContainer);
    scrollAreaSetting->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    scrollAreaSetting->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollAreaSetting->setFrameShape(QFrame::NoFrame);
    scrollAreaSetting->setObjectName("scrollAreaSetting");

    //初始化所有的设置单元控件
    getSettingUnits();

    //下面使用生产的设置单元来初始化控件
    int nSpacing = 0;
    QVBoxLayout* vSettingUnits = new QVBoxLayout(settingUnitContainer);
    vSettingUnits->setMargin(0);
    vSettingUnits->setSpacing(nSpacing);

    int nTotalHeight = (settings.size() <= 1) ? 0 : ((settings.size()-1)* nSpacing);
    for(int i = 0; i< settings.size(); i++)
    {
        int  nHeight= settings[i]->getUnitHeight();
        nTotalHeight += nHeight;

        QWidget* suWidget = settings[i]->getUnitWidget(settingUnitContainer);
        suWidget->setMouseTracking(true);   //详见 BesFramelessWidget.h 注释
        suWidget->setMinimumHeight(nHeight);
        suWidget->setMaximumHeight(nHeight);
        suWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        vSettingUnits->addWidget(suWidget);
    }

    settingUnitContainer->setMinimumHeight(nTotalHeight);
    settingUnitContainer->setMaximumHeight(nTotalHeight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(scrollAreaSetting);
}

void SuScrollPanel::initConnection()
{
    connect(scrollAreaSetting->verticalScrollBar(), &QScrollBar::rangeChanged, [=](int min, int max){
        nPageStep = scrollAreaSetting->verticalScrollBar()->pageStep();
        nScrollMin = min;
        nScrollMax = max;
    });

    connect(scrollAreaSetting->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value){
        nPageStep = scrollAreaSetting->verticalScrollBar()->pageStep();
        emit sig_scrollPosChanged(value, nPageStep);
    });

    connect(scrollTimer,SIGNAL(timeout()), this, SLOT(OnSrcollTimerTimeout()));
}
