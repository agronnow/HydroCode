#ifndef TAB_HPP
#define TAB_HPP

#include <Output.hpp>


class Tab : public Output
{
    public:
        Tab(int n, double d, std::size_t nd, std::vector<std::pair<std::string, std::size_t>> v) : Output(n, d, nd, v)
        {
            assert(("Tab output only available in 1D!", this->ndim == 1));
        };
        virtual ~Tab();
        void OutputSnapshot(Grid&, StepInfo&) override;
        std::string GetFormat() override {return "Tab";}
    protected:
    private:
};


#endif // TAB_HPP
