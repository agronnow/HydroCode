#ifndef CREATEBOUNDARY_H
#define CREATEBOUNDARY_H
#include <Fixed.hpp>
#include <outflow.hpp>
#include <User.hpp>
#include <Periodic.hpp>

std::unique_ptr<Boundary> CreateBoundary(const std::string& name, const Side side)
{
    if (name == "fixed")
    {
        return std::make_unique<Fixed>(side);
    }
    else if (name == "outflow")
    {
        return std::make_unique<Outflow>(side);
    }
    else if (name == "user")
    {
        return std::make_unique<User>(side);
    }
    else
    {
        return std::make_unique<Periodic>(side);
    }
}
#endif //CREATEBOUNDARY_H
