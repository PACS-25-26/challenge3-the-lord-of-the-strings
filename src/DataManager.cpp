#include "DataManager.hpp"

namespace Laplace
{
    SolverConfig read_data(const std::string& filename)
    {
        std::ifstream file(filename);
        if(!file.is_open())
        {
            throw std::runtime_error("Could not open file " + filename + " !" );
        }

        // making buffers
        std::vector<std::string> read_lines;
        std::string line;

        // saving all the lines to be put in the struct knowing the file structure
        // so it is possible to either parse a function or save a numerical value 
        while(std::getline(file, line))
        {
            if(line.empty() || line[0] == '#') continue;
            read_lines.push_back(line);
        }

        // checking if we reached the end of the file and if the data has been correctly read
        if(read_lines.size() != 9 || !file.eof())
        {
            throw std::runtime_error("Error occurred reading the file !");
        }

        // creating function for parsing 
        auto ParseFun = [](const std::string& funexpr) -> Function
        {
            return [funexpr](Coord coords) -> double
            {
                mu::Parser par;
                double x = coords[0];
                double y = coords[1];

                par.DefineVar("x", &x);
                par.DefineVar("y", &y);
                par.DefineConst("pi", M_PI);
                par.SetExpr(funexpr);

                return par.Eval();
            };
        };

        // creating struct to return
        SolverConfig configs;

        // storing according to data type
        configs.u_ex = ParseFun(read_lines[0]);
        configs.f = ParseFun(read_lines[1]);
        configs.cond1 = ParseFun(read_lines[2]);
        configs.cond2 = ParseFun(read_lines[3]);
        configs.cond3 = ParseFun(read_lines[4]);
        configs.cond4 = ParseFun(read_lines[5]);

        configs.N = std::stoul(read_lines[6]);
        configs.tol = std::stod(read_lines[7]);
        configs.max_it = std::stoul(read_lines[8]);

        return configs;
    }

}