#ifndef SIMULATION_H
#define SIMULATION_H
#include <string>
#include <vector>
#include <map>
#include <auxillary.hpp>
#include <memory>
#include <Solver.hpp>
#include "Grid.hpp"
#include <hll.hpp>
#include <eos.hpp>
#include <Ideal.hpp>
#include <Integrator.hpp>
#include <Euler.hpp>
#include <Muscl.hpp>
#include <Slopelimiter.hpp>
#include <MonotonizedCentral.hpp>
#include <Minmod.hpp>
#include <Flat.hpp>
#include <Output.hpp>
#include <DBL.hpp>
#include <PNG.hpp>
#include <Tab.hpp>
#include <chrono>
#include <iniparser.hpp>
#include <Boundary.hpp>
#include <Fixed.hpp>
#include <outflow.hpp>
#include <Periodic.hpp>
#include <User.hpp>
#include <CreateBoundary.hpp>

class Simulation
{
    public:
        Simulation(const std::string&, INI::File&);
        virtual ~Simulation();
        double Getdt() { return dt; }
        void Setdt(double val) { dt = val; }
        double Gett() { return t; }
        void Sett(double val) { t = val; }
        const std::string& Getname() { return name; }
        void MainLoop();
    protected:
    private:
        void ApplyInitialConditions();
        void SetParams(INI::File&);
        void SetBoundaries(INI::File&);
        void SetTimestep(INI::File&);
        void SetOutput(INI::File&);
        void SetPhysics(INI::File&);
        void SetSchemes(INI::File&);
        void Step();
        const std::string name;
        double dt;
        double t;
        double tstop;
        Grid grid;
        std::unique_ptr<Solver> solver;
        std::unique_ptr<EOS> eos;
        std::unique_ptr<Integrator> integrator;
        std::unique_ptr<Slopelimiter> slopelimiter;
        std::map<Side, std::unique_ptr<Boundary>> boundaries;
        std::vector<std::unique_ptr<Output>> outputs;
};

#endif // SIMULATION_H
