#include "Utils.hpp"

namespace Laplace{
     Laplace::Matrix compute_F(const std::string& file_name)
     {

     }

    void init_parallel(Laplace::ParallelConfig& p_config, Laplace::Index N)
    {

    }

    void init_solver(Laplace::SolverConfig& s_config, const std::string& config_file)
    {

    }

    FunctionList read_data (const std::string& filename, Coord* coord)
    {
        // read file and manage opening errors
        std::ifstream file(filename);
        if(!file.is_open())
        {
            std::cerr << "Could not open file " << filename << " ." << std::endl;
            exit(1);
        }

        // declare containers
        FunctionList funcs;
        std::string line;
        auto coord_pos = coord->data(); // to be used for DefineVar
        while(std::getline(file, line))
        {
            if(line.empty() || line[0] == '#') continue; // so that the text file can support comments

            // define parser and variables to be read. Notice that since we already
            // know that pi is used in the functions to be tested it is added as a constant.
            auto parser = std::make_unique<mu::Parser>();
            parser->DefineVar("x", coord_pos);
            parser->DefineVar("y", coord_pos + 1);
            parser->DefineConst("pi", M_PI);

            parser->SetExpr(line);

            // turn parser into actual std::function and add it to the function list
            Function ff = [p = std::move(parser) , coord](const Coord& val) -> double
            {
                *coord = val;
                return p->Eval();
            };
            funcs.push_back(ff);
        }
        
        // check if we have arrived to the end of the file,
        if (!file.eof() && file.fail()) {
        std::cerr << "Error: failure while reading " << filename << " ." << std::endl;
        exit(1);
        }

        // check if provided data is what is asked for
        if(funcs.size() != 6)
        {
            std::cerr << "Error: not enough arguments provided in " << filename << " !" << std::endl;
            exit(1);
        }

        return funcs;
    }

}