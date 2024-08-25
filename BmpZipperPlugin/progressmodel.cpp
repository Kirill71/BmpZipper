#include "progressmodel.h"

namespace BmpZipper::Ui
{

ProgressModel::ProgressModel(QObject* parent) :
    QObject(parent)
{
}

void ProgressModel::init(const int min, const int max)
{
    if (min >= max)
    {
        return;
    }
    if (min < 0 || max < 0)
    {
        return;
    }

    m_minValue = min;
    m_maxValue = max;
}

void ProgressModel::notifyProgress(const int current)
{
    constexpr int MaxPercentValue = 100;
    const int percent = current * MaxPercentValue / m_maxValue;
    emit progressChanged(percent);
}

int ProgressModel::min() const
{
    return m_minValue;
}

int ProgressModel::max() const
{
    return m_maxValue;
}

const QString& ProgressModel::getText() const
{
    return m_text;
}

void ProgressModel::setText(const QString& text)
{
    if (m_text != text)
    {
        m_text = text;
        emit textChanged(m_text);
    }
}

}
