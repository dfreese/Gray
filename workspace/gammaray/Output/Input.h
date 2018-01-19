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

class Input
{
public:
    Input() = default;
    bool set_logfile(const std::string & name);
    void set_format(Output::Format format);
    bool read_interaction(Interaction & interact);
    bool read_interactions(std::vector<Interaction> & interactions,
                           size_t no_interactions);

    void set_variable_mask(const Output::WriteFlags & flags);
    Output::WriteFlags get_write_flags() const;

private:
    static bool read_header_binary(std::istream & input, int & version);
    static bool read_header_ascii(std::istream & input, int & version);
    static bool read_write_flags_binary(Output::WriteFlags & flags,
                                        std::istream & input);
    static bool read_write_flags_ascii(Output::WriteFlags & flags,
                                       std::istream & input);
    static bool read_variables_binary(std::vector<Interaction> & interactions,
                                      size_t no_interactions,
                                      std::istream & input,
                                      const Output::WriteFlags & flags);
    static bool read_variables_ascii(std::vector<Interaction> & interactions,
                                     size_t no_interactions,
                                     std::istream & input,
                                     const Output::WriteFlags & flags);
    std::ifstream log_file;
    Output::Format format;
    Output::WriteFlags var_format_write_flags;
    int var_format_version;
};

#endif // INPUT_H
