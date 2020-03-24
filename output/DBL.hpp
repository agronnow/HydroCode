#ifndef DBL_H
#define DBL_H

#include <Output.hpp>


class DBL : public Output
{
    public:
        DBL(std::size_t n, double d, std::size_t nd, std::vector<std::pair<std::string, std::size_t>> v) : Output(n, d, nd, v) {};
        virtual ~DBL();
        void OutputSnapshot(Grid&, StepInfo&) override;
        std::string GetFormat() override {return "DBL";}
    protected:
    private:
};

#endif // DBL_H
