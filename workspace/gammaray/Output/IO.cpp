#include <Output/IO.h>

std::istream& IO::GetLineCommented(std::istream& is, std::string& str,
                                   char comment)
{
    std::string line;
    auto& retval = getline(is, line);
    if (retval) {
        // remove anything after a comment
        str = line.substr(0, line.find_first_of(comment));
    }
    return (retval);
}

std::istream& IO::GetLineCommentLeadWs(std::istream& is, std::string& str,
                                       char comment)
{
    std::string line;
    auto& retval = getline(is, line);
    if (retval) {
        auto non_ws_start = line.find_first_not_of(" \t");
        if (non_ws_start != std::string::npos) {
            // remove any leading whitespace
            line = line.substr(non_ws_start);
        }
        // remove anything after a comment
        str = line.substr(0, line.find_first_of(comment));
    }
    return (retval);
}