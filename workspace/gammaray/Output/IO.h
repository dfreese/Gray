#ifndef io_h
#define io_h

#include <iostream>
#include <string>

namespace IO {
    std::istream& GetLineCommented(std::istream& is,
                                   std::string& str,
                                   char comment = '#');
    std::istream& GetLineCommentLeadWs(std::istream& is,
                                       std::string& str,
                                       char comment = '#');
}

#endif // io_h
