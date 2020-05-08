#include "indicator.h"

Indicator::Indicator(QWidget * parent) : QLabel(parent)
{
    setState(IndicatorState::Off);
    connect(&m_changeStateTimer, &QTimer::timeout, this, &Indicator::processing);
    m_changeStateTimer.setInterval(500);
}

void Indicator::setState(IndicatorState state)
{
    m_state = state;
    m_changeStateTimer.stop();
    switch(state) {
        case IndicatorState::Ok: setState(okState);
            break;
        case IndicatorState::Off: setState(offState);
            break;
        case IndicatorState::Error: setState(errorState);
            break;
        case IndicatorState::Processing:
            m_changeStateTimer.start();
            break;
    }
}

void Indicator::setState(const QString state)
{
    QString style = base + state;
    setStyleSheet(style);
}

void Indicator::processing()
{
    if (m_state != IndicatorState::Processing) return;
    if (m_processingStateDark)
        setState(processingStateLight);
    else
        setState(processingStateDark);

    m_processingStateDark = !m_processingStateDark;
}
