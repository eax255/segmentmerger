//requires: [module argparser] [namespace fs] [header <iostream>] [module parser]
//provides: [function main]

int main(int argc, char* argv[]){
    arguments args;
    argp_parse(&argparser, argc, argv, 0, 0, &args);
    parser modules;
    for(auto& it : args.search_path){
        for(auto& p: fs::directory_iterator(it)){
            modules.parse_file(p);
        }
    }
    for(auto& it : args.files){
       modules.parse_file(it);
    }
    modules.gen_program(std::cout,{"function","main"},args.single);
}
