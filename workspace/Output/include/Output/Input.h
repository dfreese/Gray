#ifndef INPUT_H
#define INPUT_H
#include <Physics/Interaction.h>
#include <Output/BinaryFormat.h>
#include <Output/Output.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

class GammaStats;
class Interaction;

class Input
{
public:
    Input();
    ~Input();
    bool set_logfile(const std::string & name);
    void set_format(Output::Format format);
    bool read_interaction(Interaction & interact);
    bool read_interactions(std::vector<Interaction> & interactions);

    void set_variable_mask(const Output::WriteFlags & flags);
    Output::WriteFlags get_write_flags() const;

private:
    void parse_log_word(int log, int & interaction, int & color,
                        int & scatter, int & det_mat, int & src_id);

    static bool read_header(std::istream & input, bool binary,
                            int & version);
    static bool read_write_flags(Output::WriteFlags & flags,
                                 std::istream & input, bool binary);
    static bool read_variable_binary(Interaction & inter, std::istream & input,
                                     const Output::WriteFlags & flags);
    static bool read_variable_ascii(Interaction & inter, std::istream & input,
                              const Output::WriteFlags & flags);
    std::ifstream log_file;
    Output::Format format;
    Output::WriteFlags var_format_write_flags;
    int var_format_version;
};

#endif // INPUT_H
