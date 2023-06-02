#ifndef CALLBACK_H
#define CALLBACK_H
#include <cstdint>
class AbstractCallback
{
protected:
    AbstractCallback(){}
public:
    virtual double operator() (double, uint64_t) = 0;
    virtual ~AbstractCallback(){}
};

class BaseCallback final : public AbstractCallback
{
public:
    BaseCallback() : AbstractCallback(){}
    double operator() (double value, uint64_t) override {
        return value;
    }

};

class InertCallback final : AbstractCallback
{
public:
    InertCallback() : AbstractCallback(){}
    double operator() (double value, uint64_t) override {
        return value;
      }
};

#endif // CALLBACK_H
