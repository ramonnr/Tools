/*
	TODO -
	Make file instead of  g++ cpv.cpp -std=c++17 -lstdc++fs -lpthread
*/


#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <sys/stat.h>
#include <thread>
#include <future>

namespace fs = std::filesystem;

void sanitizeInput(int, const std::vector<fs::path>&, const fs::path&);
bool copyFile(const fs::path&, const fs::path&);

void sanitizeInput(int argc, const std::vector<fs::path>& files, const fs::path& odir){
	if(argc < 3){
		std::cout << "expeced input file(s) and outputh path/file" << std::endl;
		exit(-1);
	}
	for(auto f : files){
		if(!std::filesystem::is_regular_file(f)){
			std::cout << f << " is not a file" << std::endl;
			exit(-1);
		}
	}
	if(std::filesystem::is_regular_file(odir) && argc > 3){
		std::cout << "cant output multiple files to single" << std::endl;
		exit(-1);
	}
	if(!fs::is_directory(odir)){
		std::cout << odir << " not found"<< std::endl;
		exit(-1);
	}
}

bool copyFile(const fs::path& in, const fs::path& out){
	int fdi = open(in.c_str(),O_RDONLY);
	if(fdi < 0){return false;}

  struct stat finfo;
	if(fstat(fdi,&finfo) != 0){return false;}

  std::unique_ptr<char[]> buf{new char[finfo.st_size]};
	if(read(fdi,buf.get(),finfo.st_size) < 0){return false;}

  int fdo = open((out.string() + "/" + in.filename().string()).c_str(),O_CREAT | O_WRONLY, finfo.st_mode);
	if(fdo < 0){return false;}

  const long long fsize = finfo.st_size;
	long long bwritten = 0;

  auto output = [](const fs::path& f, const long long tot, const long long done){
    std::cout << "\r[";
    for(int i = 0; i < done / (tot + 0.0) * 20; i++){
      std::cout << "=";
    }
    for(int i =  done / (tot + 0.0) * 20; i < 20; i++){
      std::cout << " ";
    }
    std::cout << "] " << f.filename().string();
    std::flush(std::cout);
  };
  std::async(std::launch::async,[=]{output(in,fsize,0);});
  while(bwritten < fsize){
		if(long long cwrite = write(fdo,buf.get() + bwritten, fsize - bwritten); cwrite < 0){return false;}
		else{
      bwritten += cwrite;
      std::async(std::launch::async,[=]{output(in,fsize,bwritten);});
    }
	}
  std::cout << "\n";
	return true;
}


int main(int argc, char** argv){
	const fs::path odir{argv[argc-1]};
	const std::vector<fs::path> ifiles = [=]()->std::vector<fs::path>{
		std::vector<fs::path> v;
		if(argc == 3){
			v.push_back(argv[1]);
			return v;
		}
		for(int i = 1; i < argc - 1; i++){
			v.push_back(argv[i]);
		}
		return v;
	}();

	sanitizeInput(argc,ifiles,odir);
	for(auto f : ifiles){
    copyFile(f,odir);
  }
}
