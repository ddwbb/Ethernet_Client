#ifndef INDICATOR_H
#define INDICATOR_H

#include <QLabel>
#include <QTimer>
#include <QString>

static const QString base = "border: 2px solid #fff; border-radius: 12px; min-width: 20px; max-width: 24px; min-height: 20px; max-height: 20px;";
static const QString okState = "background-color: #2ED03C;";
static const QString offState = "background-color: #555;";
static const QString errorState = "background-color: red;";
static const QString processingStateDark = "background-color: darkblue;";
static const QString processingStateLight = "background-color: blue;";

enum IndicatorState {
    Ok,
    Off,
    Error,
    Processing
};

class Indicator : public QLabel
{
    Q_OBJECT
public:
    explicit Indicator(QWidget * parent = nullptr);

    void setState(IndicatorState);

public slots:
    void processing();

private:
    int m_state;
    bool m_processingStateDark;
    QTimer m_changeStateTimer;

    void setState(const QString);
};

#endif // INDICATOR_H
