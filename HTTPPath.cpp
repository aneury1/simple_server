#include "HTTPPath.h"

int count_foward_slashed(string s)
{
    int ret=0;
    for(int i=0;i<s.length();i++)
      if(s[i]=='/')
       ret++;
    return ret;
}

////#include <iostream>
HTTPPath::HTTPPath(string path)
{
    ////std::cout <<"path \n"<<path<<"path\n\n\n";
    if(path.length()<=0){
      /////  std::cout <<"path length is 0\n";
        return;
    }
     this->path=path;
     if(path.length()==1)
     {
        foward_slashed_number = 0;
        /////std::cout <<"foward slashed count just 1";
        return  ;
     }
     foward_slashed_number = count_foward_slashed(path);
     string tmp="";
     int iter=path.find("/", 0);
     if(iter==string::npos)return;
     iter++;
     while(1)
     {
         if(path[iter]=='/')
         {
             path_parsed.emplace_back(tmp);
             tmp ="";
         }         
         else
         {
            tmp+=path[iter];
         }
          iter++;


          if(iter==path.length())
          {
              if(tmp.length()>0)
                path_parsed.emplace_back(tmp);
              break;
          }
     }  
  
}