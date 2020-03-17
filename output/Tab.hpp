#ifndef TAB_HPP
#define TAB_HPP

#include <Output.hpp>
extern int g_NDim;


class Tab : public Output
{
    public:
        Tab(int n, double d, std::vector<std::pair<std::string, int>> v) : Output(n, d, v)
        {
            assert(("Tab output only available in 1D!", g_NDim == 1));
        };
        virtual ~Tab();
        void OutputSnapshot(Grid&, StepInfo&) override;
        std::string GetFormat() override {return "Tab";}
    protected:
    private:
};


#endif // TAB_HPP
