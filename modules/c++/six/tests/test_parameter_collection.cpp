#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <vector>
#include <algorithm>
#include <iostream>


bool pcEqualsPv(const six::ParameterCollection & pc, 
                const std::vector<six::Parameter> & pv)
{
  if(pc.size() != pv.size()) return false;

  for(size_t i = 0; i < pc.size(); ++i) 
  {
    if(pc[i] != pv[i]) return false;
  }

  return true;
}

int main() 
{
  six::Parameter p0 = 34;
  six::Parameter p1 = 25.0;
  six::Parameter p2 = "aslkfj";
  six::Parameter p3 = 99; 
  six::Parameter p4 = 2.2e-12;
  six::Parameter p5 = 66ULL;
  six::Parameter p6 = 99;
  six::Parameter p7 = "aaa";
  six::Parameter p8 = "bbb";
  six::Parameter p9 = 86.0f;
  six::Parameter non_member = 100000;
  p3.setName("name is set should cmp different");


  std::vector<six::Parameter> pv;
  pv.push_back(p0);  
  pv.push_back(p1);
  pv.push_back(p2);
  pv.push_back(p3);
  pv.push_back(p4);
  pv.push_back(p5);
  pv.push_back(p6);
  pv.push_back(p7);
  pv.push_back(p8);
  pv.push_back(p9);
  six::ParameterCollection pc;
  pc.push_back(p0);  
  pc.push_back(p1);
  pc.push_back(p2);
  pc.push_back(p3);
  pc.push_back(p4);
  pc.push_back(p5);
  pc.push_back(p6);
  pc.push_back(p7);
  pc.push_back(p8);
  pc.push_back(p9);

  if(!pcEqualsPv(pc,pv)) 
  {
    std::cout << "Not constructed correctly" << std::endl;
    return 1;
  }

  if(pc[3] != p3 || pc[9] != pv[9] || pc.at(2) != pv.at(2)) 
  {
    std::cout << "Not indexing correctly" << std::endl;
    return 1;
  }

  if(pc.end() - pc.begin() != pv.end() - pv.begin()
      || *pc.begin() != p0)
  {
    std::cout << "Iterator not working correctly" << std::endl;
    return 1;
  }

  bool caught = false;
  try{
    pc.at(123123123);
  }catch(...){
    caught = true;
  }

  if(!caught) {
    std::cout << "at() function does not throw correctly" << std::endl;
    return 1;
  }
  
  six::Parameter find_p6 = *std::find(pc.begin(),pc.end(),p6);
  six::ParameterCollection::ParameterCollectionIterator_t 
      find_non_member = std::find(pc.begin(),pc.end(),non_member);
  if(find_p6 != p6 || find_non_member != pc.end()) {
    std::cout << "Does not seem to work with standard algorithms" << std::endl;
    return 1;
  }

  std::cout << "Looks like it's working" << std::endl;
  return 0;
}

