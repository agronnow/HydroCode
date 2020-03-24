#ifndef CREATEBOUNDARY_H
#define CREATEBOUNDARY_H
#include <Fixed.hpp>
#include <outflow.hpp>
#include <User.hpp>
#include <Periodic.hpp>

std::unique_ptr<Boundary> CreateBoundary(const std::string&, const Side);
#endif //CREATEBOUNDARY_H
