//requires: [header <vector>] [header <string>] [header <stdexcept>] [header <map>] [header <string.h>] [header <iostream>] [header <ctype.h>] [header <tuple>]
//provides: [module argparser]
namespace argparser{
struct option{
    std::string name;
    int key;
    std::string argdesc;
    std::string doc;
};

struct UnknownArgument : public std::runtime_error{
    UnknownArgument(const std::string& v) : std::runtime_error(v){}
};
struct MissingParameter : public std::runtime_error{
    MissingParameter(const std::string& v) : std::runtime_error(v){}
};
struct StopParse{};

struct argparser{
private:
    std::string doc;
    std::string version_;
    std::vector<option> options;
    std::map<std::string,size_t> long_opt;
    std::map<char,size_t> short_opt;
    std::string positional;
    std::string bugaddr;
public:

    argparser(std::string _version, std::string _bugaddr, std::string _doc, std::string _positional) :
            doc(_doc), version_(_version), positional(_positional), bugaddr(_bugaddr){
        add({.name = "help", .key = 'h', .argdesc="", .doc = "Give this help list"});
        add({.name = "version", .key = 'V', .argdesc="", .doc = "Print program version"});
    }

    virtual void help(const char* argv0){
        std::cout<<"Usage: " << argv0 << " [OPTIONS...] " << positional << '\n';
        std::cout<<doc<<"\n\n";
        for(auto& opt : options){
            if(isgraph(opt.key)) std::cout<<"  -"<<opt.key<<", ";
            else std::cout<<"      ";
            std::cout<<"--"<<opt.name<<' ';
            if(opt.argdesc != "")std::cout<<opt.argdesc;
            size_t nspace = 30;
            if(opt.argdesc.size()+opt.name.size()+2 > 30)std::cout<<'\n',nspace=38;
            else nspace = 30-1-opt.argdesc.size()+opt.name.size();
            while(nspace-- > 0)std::cout<<' ';
            std::cout<<opt.doc<<'\n';
        }
        std::cout<<'\n';
        std::cout<<"Report bugs to "<<bugaddr<<'\n';
        exit(0);
    }

    virtual void version(){
        std::cout<<version_<<'\n';
        exit(0);
    }

    virtual void parse(int key, const char*arg) = 0;
    void add(const option& opt){
        if(0 <= opt.key && opt.key < 128) short_opt[opt.key] = options.size();
        long_opt["--"+opt.name] = options.size();
        options.push_back(opt);
    }
    std::pair<int,int> parse_arg(option& opt, int i, int j, int argc, const char* argv[]){
        if(opt.key == 'h')help(argv[0]);
        if(opt.key == 'V')version();
        if(opt.argdesc != ""){
            if(j!=-1 && argv[i][j+1]){
                parse(opt.key, argv[i]+j+1);
                return {i,strlen(argv[i])-1};
            }
            else if(i+1<argc){
                parse(opt.key, argv[i+1]);
                return {i+1,strlen(argv[i+1])-1};
            }
            else throw MissingParameter("for argument "+opt.name);
        }
        else parse(opt.key, 0);
        return {i,j};
    }
    int parse_args(int argc, const char* argv[]){
        int i;
        try{
            for(i=1;i<argc;++i){
                int j;
                if(argv[i][0] != '-') parse(0,argv[i]);
                else if(argv[i][1] == 0) parse(0,argv[i]);
                else if(argv[i][1] == '-'){
                    if(long_opt.count(argv[i]))std::tie(i,j)=parse_arg(options[long_opt[argv[i]]],i,-1,argc,argv);
                    else parse(-256,argv[i]);
                }
                else {
                    for(j=1;argv[i][j];++j){
                         if(short_opt.count(argv[i][j])) std::tie(i,j) = parse_arg(options[short_opt[argv[i][j]]],i,j,argc,argv);
                         else parse(-argv[i][j], argv[i]);
                    }
                }
             }
        }
        catch(StopParse e){}
        return i;
    }
};
}

