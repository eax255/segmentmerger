//provides: [module parser]
//requires: [header <fstream>] [header <sstream>] [header <map>] [header <string>] [namespace fs] [header <functional>] [header <stdexcept>] [header <set>]
struct file{
    std::vector<std::pair<std::string,std::string>> provides;
    std::vector<std::pair<std::string,std::string>> requires;
    std::string contents;
    std::string path;
    file(std::istream& in){
        std::string l;
        while(std::getline(in,l)){
            std::istringstream ss(l);
            std::string word;
            ss >> word;
            if(word == "#pragma"){
                ss >> word;
                if(word.substr(0,4) == "once")continue;
            }
            if(word == "#include"){
                ss >> word;
                if(word[0] == '"'){
                    requires.emplace_back("file",word.substr(1,word.size()-2));
                    continue;
                }
            }
            if(word == "//provides:" or word == "//requires:"){
                std::vector<std::pair<std::string,std::string>>* cont;
                if(word == "//provides:")cont = &provides;
                else cont = &requires;
                std::string w0,w1;
                while(ss >> w0 >> w1){
                    cont->emplace_back(w0.substr(1),w1.substr(0,w1.size()-1));
                }
            }
            else contents+=l+"\n";
        }
    };
};

struct parser{
    std::vector<file> files;
    std::map<std::string,std::map<std::string,size_t>> providemap;
    void parse_file(const fs::path& path){
        std::ifstream in(path);
        files.emplace_back(in);
        files.back().contents = "COMPILING("+path.native()+")\n" + files.back().contents;
        files.back().path = path.native();
        files.back().provides.emplace_back("file",files.back().path);
        for(auto& it : files.back().provides) providemap[it.first][it.second] = files.size();
    }
    void gen_program(std::ostream& out, std::pair<std::string, std::string> block, bool single){
        std::set<std::pair<std::string, std::string>> provided;
        std::set<std::pair<std::string,std::string>> inprogress;
        std::vector<std::string> flags;
        out<<"#define DO_PRAGMA(x) _Pragma(#x)\n#define COMPILING(x) DO_PRAGMA(message (\"Compiling - \" #x))\n";
        std::function<bool(const std::pair<std::string, std::string>&)> traverse = [&,this](const std::pair<std::string, std::string>& blk){
            if(provided.count(blk)) return true;
            if(inprogress.count(blk)) throw std::runtime_error("cyclic dependency on "+blk.first+" "+blk.second);
            if(!providemap[blk.first][blk.second]){
                if(blk.first == "header"){out<<"#include "<<blk.second<<'\n';provided.insert(blk);return true;}
                if(blk.first == "flag"){flags.push_back(blk.second);provided.insert(blk);return true;}
                throw std::runtime_error("no match for "+blk.first+" "+blk.second);
            }
            inprogress.insert(blk);
            file* f = &files[providemap[blk.first][blk.second]-1];
            for(auto& it : f->provides)inprogress.insert(it);
            for(auto& it : f->requires){
                if(!traverse(it))throw std::runtime_error("failed to load "+blk.first+" "+blk.second+" due to "+it.first+" "+it.second);
            }
            out<<"\n\n";
            for(auto& it : f->provides)provided.insert(it),inprogress.erase(it),out<<"// "<<it.first<<" "<<it.second<<"\n";
            if(single)out<<"\n\n"<<f->contents<<"\n\n";
            else out<<"\n\n#include \""+f->path+"\"\n\n";
            return true;
        };
        while(!traverse(block));
        out<<"\n\n"<<"// g++ -xc++ -";
        for(auto& it : flags)out<<" "<<it;
        out<<"\n";
    }
};
