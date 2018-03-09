//requires: [header <argp.h>] [header <vector>] [header <string>]
//provides: [module argparser]
const char *argp_program_version = "segmentmerge v0.0.0";
const char *argp_program_bug_address = "https://github.com/eax255/segmentmerger/issues";
static char doc[] = "Segmented program merger.";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    {.name = "lib", .key = 'L', .arg = "PATH", .flags = 0, .doc = "library path", .group = 0},
    {.name = "single", .key = 's', .arg = 0, .flags = 0, .doc = "single file output", .group = 0},
    {.name = "recursive", .key = 'r', .arg = 0, .flags = 0, .doc = "recursive directory traverse", .group = 0},
    {.name = "output", .key = 'o', .arg = "OUTPUT", .flags = 0, .doc = "output file", .group = 0},
    {0}
};
struct arguments {
    std::vector<std::string> search_path;
    std::vector<std::string> files;
    bool single,recursive;
    std::string output;
    arguments() : single(false) {}
};
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    arguments *args = (arguments*)state->input;
    switch (key) {
    case 'L': if(arg != NULL) args->search_path.push_back(arg); break;
    case 's': args->single = true; break;
    case 'r': args->recursive = true; break;
    case 'o': args->output = arg; break;
    case ARGP_KEY_ARG: args->files.push_back(arg); break;
    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
static struct argp argparser = { .options = options, .parser = parse_opt, .args_doc = args_doc, .doc = doc, .children = 0, .help_filter = 0, .argp_domain = 0 };
