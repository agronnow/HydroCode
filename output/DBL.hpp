#ifndef DBL_H
#define DBL_H

#include <Output.hpp>


class DBL : public Output
{
    public:
        DBL(int n, double d, std::vector<std::pair<std::string, int>> v) : Output(n, d, v) {};
        virtual ~DBL();
        void OutputSnapshot(Grid&, StepInfo&) override;
        std::string GetFormat() override {return "DBL";}
    protected:
    private:
};

#endif // DBL_H
