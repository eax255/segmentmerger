//requires: [module argparser] [namespace fs] [header <iostream>] [module parser]
//provides: [function main]

int main(int argc, char* argv[]){
    arguments args;
    argp_parse(&argparser, argc, argv, 0, 0, &args);
    parser modules;
    for(auto& it : args.search_path){
        if(args.recursive){
            for(auto& p: fs::recursive_directory_iterator(it)){
                modules.parse_file(p);
            }
        }
        else{
            for(auto& p: fs::directory_iterator(it)){
                modules.parse_file(p);
            }
        }
    }
    for(auto& it : args.files){
       modules.parse_file(it);
    }
    std::ostream *output = &std::cout;
    if(args.output.size() != 0) output = new std::ofstream(args.output);
    modules.gen_program(*output,{"function","main"},args.single);
    if(args.output.size() != 0) delete output,output=nullptr;
}
