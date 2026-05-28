#include "DataManager.hpp"

namespace Laplace
{
    namespace {
            class ParsedFunction {
            public:

                /**
                 * @brief Constructs a ParsedFunction from a given string
                 * @details The parser is set up once at construction time with variables x and y
                 * @param expr Mathematical expression in muParser syntax
                 */
                explicit ParsedFunction(std::string expr)
                : m_expr(std::move(expr))   // move is used to avoid a redundant string copy
                , m_parser(std::make_unique<mu::Parser>()) // unique pointer protects the memory especially when parallelisation is used 
                , m_coords({0.0 , 0.0}) // initialised at 0 before binding
            {
                m_parser -> DefineVar("x", &m_coords[0]);
                m_parser -> DefineVar("y", &m_coords[1]);
                m_parser -> DefineConst("pi", M_PI);
                m_parser -> SetExpr(m_expr);
            }

                // making the class non copyable to avoid copies that point at the wrong coordinates
                ParsedFunction(const ParsedFunction&) = delete;
                ParsedFunction& operator=(const ParsedFunction&) = delete;

                // making the class movable sp that ownership of the parser transfers cleanly 
                ParsedFunction(ParsedFunction&&)                 = default;
                ParsedFunction& operator=(ParsedFunction&&)      = default;

                // evaluation operator
                double operator()(const Coord& coords)
                {
                    m_coords = coords;      // update the values the parser will read
                    return m_parser->Eval();
                }

            private:
                std::string                  m_expr;    ///< original expression string
                std::unique_ptr<mu::Parser>  m_parser;  ///< owning handle to the muParser instance
                Coord                        m_coords;  ///< current evaluation point whose address is bound to parser
            };
    }


    Function make_parsed_function(const std::string& expr)
    {
        auto par = std::make_shared<ParsedFunction>(expr);
        return [par](const Coord& coords) mutable -> double { return (*par)(coords); };
    }

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

        // creating struct to return
        SolverConfig configs;

        // storing according to data type
        configs.u_ex = make_parsed_function(read_lines[0]);
        configs.f = make_parsed_function(read_lines[1]);
        configs.cond1 = make_parsed_function(read_lines[2]);
        configs.cond2 = make_parsed_function(read_lines[3]);
        configs.cond3 = make_parsed_function(read_lines[4]);
        configs.cond4 = make_parsed_function(read_lines[5]);

        configs.N = std::stoul(read_lines[6]);
        configs.tol = std::stod(read_lines[7]);
        configs.max_it = std::stoul(read_lines[8]);

        return configs;
    }

}