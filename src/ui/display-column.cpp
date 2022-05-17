#include <string>

using std::string;

#include "display-column.hpp"

DisplayColumn::DisplayColumn(const std::string _key, 
                    const std::string _title,
                    const std::string _format,
                    size_t _width)
    : key(_key)
    , title(_title)
    , format(_format)
    , width(_width)
{

}

//DisplayColumn::~DisplayColumn(){}

