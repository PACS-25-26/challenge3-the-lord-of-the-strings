#include "DataManager.hpp"

namespace Laplace
{
    namespace {

            /**
             * @class ParsedFunction
             * @brief class wrapper around mu::Parser for evaluating mathematical expressions
             * @details This class provides a way to safely evaluate the given mathematical functions at 
             * runtime based on given coordinates.
             * To ensure safety in shared-memory parallel regions, each instance 
             * dynamically allocates its own parser with a std::unique_ptr and manages its own 
             * local coordinate buffer. This avoids race conditions during variable binding.
             * @note This class is explicitly non-copyable to prevent multiple instances from 
             * aliasing the same internal coordinate memory addresses, but it supports move semantics 
             * for efficient ownership transfer.
             */
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


    // helper function to create a std::function from a string expression using the ParsedFunction class
    Function make_parsed_function(const std::string& expr)
    {
        auto par = std::make_shared<ParsedFunction>(expr); // shared pointer allows the lambda to capture a copy of the parsed function that remains valid independently of the original object
        return [par](const Coord& coords) mutable -> double { return (*par)(coords); }; // lambda function that passes the coordinates to the parsed function and returns the result
    }

    // main function to read the data from the file and return a struct with the parsed functions and numerical values
    std::vector<std::string> read_data(const std::string& filename)
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

        return read_lines;
    }

    SolverConfig process_data(const std::vector<std::string>& s_vec)
    {
        // creating struct to return
        SolverConfig configs;

        // storing according to data type using the helper function for the functions
        configs.u_ex = make_parsed_function(s_vec[0]);
        configs.f = make_parsed_function(s_vec[1]);
        configs.cond1 = make_parsed_function(s_vec[2]);
        configs.cond2 = make_parsed_function(s_vec[3]);
        configs.cond3 = make_parsed_function(s_vec[4]);
        configs.cond4 = make_parsed_function(s_vec[5]);

        configs.N = std::stoul(s_vec[6]);
        configs.tol = std::stod(s_vec[7]);
        configs.max_it = std::stoul(s_vec[8]);

        return configs;
}