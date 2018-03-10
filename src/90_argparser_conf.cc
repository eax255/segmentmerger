//requires: [module argparser]
//provides: [module argparser_conf]

class config : public argparser::argparser{
public:
    std::vector<std::string> search_path;
    std::vector<std::string> files;
    bool single,recursive;
    std::string output;
    config() : argparser(
                      "segmentmerge v0.0.1",
                      "https://github.com/eax255/segmentmerger/issues",
                      "Segmented program merger.",
                      "[FILENAME]..."),
                  single(false),recursive(false) {
        add({.name = "lib", .key = 'L', .argdesc = "PATH", .doc = "library path"});
        add({.name = "single", .key = 's', .argdesc = "", .doc = "single file output"});
        add({.name = "recursive", .key = 'r', .argdesc = "", .doc = "recursive directory traverse"});
        add({.name = "output", .key = 'o', .argdesc = "OUTPUT", .doc = "output file"});
    }

    void parse(int key, const char*arg) final{
        switch (key) {
        case 'L': if(arg != NULL)search_path.push_back(arg); break;
        case 's': single = true; break;
        case 'r': recursive = true; break;
        case 'o': if(arg != NULL) output = arg; break;
        case 0: if(arg != NULL) files.push_back(arg); break;
        default: throw ::argparser::UnknownArgument(arg);
        }
    }
};

