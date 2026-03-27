#ifndef LOWPASS_FILTER_H
#define LOWPASS_FILTER_H

namespace pid_longitudinal_controller
{

class LowpassFilter1d
{

private:
    double m_x;
    double m_gain;

public:

    LowpassFilter1d(const double x, const double gain) :
            m_x(x), m_gain(gain)
    {
    }

    void reset(const double x)
    {
        m_x = x;
    }

    double getValue() const
    {
        return m_x;
    }

    double filter(const double u)
    {
        const double ret = m_gain * m_x + (1.0 - m_gain) * u;
        m_x = ret;

        return ret;
    }

};

}    // namespace

#endif  // LOWPASS_FILTER_H
