#include "elementlist.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace GMAD;

namespace GMAD {
  extern std::string current_line;
}

void ElementList::push_back(Element& el, bool unique) {
  // better to search in map (faster)
  if (unique && itsMap.find(el.name) != itsMap.end()) {
    std::cout << "ERROR: element already defined: " << el.name << std::endl;
    exit(1);
  }
  // insert at back of list (insert() instead of push_back() to get iterator for map):
  ElementListIterator it = itsList.insert(end(),el);
  itsMap.insert(std::pair<std::string,ElementListIterator>(el.name,it));
}

int ElementList::size()const {
  return itsList.size();
}

void ElementList::clear() {
  itsList.clear();
  itsMap.clear();
}

void ElementList::erase() {
  ElementListIterator it = begin();
  for(;it!=end();it++) {
    delete (*it).lst;
  }
  clear();
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator it) {

  // find entry in map to erase:
  std::string name = (*it).name;
  if (itsMap.count(name) == 1) {
    itsMap.erase(name);
  }
  else { // more than one entry with same name 
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit) {
      if ((*emit).second == it) // valid comparison? if not, how to find correct element?
      {
	itsMap.erase(emit);
	break;
      }
    }
  }
  return itsList.erase(it);
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator first, ElementListIterator last) {
  ElementListIterator it=first;
  while (it!=last) {
    // erase one by one
    it = erase(it);
  }
  return it;
}

ElementList::ElementListIterator ElementList::begin() {
  return itsList.begin();
}

ElementList::ElementListIterator ElementList::end() {
  return itsList.end();
}

ElementList::ElementListIterator ElementList::find(std::string name,unsigned int count) {
  if (count==1) {
    ElementMapIterator emit = itsMap.find(name);
    if (emit==itsMap.end()) return itsList.end();
    return (*emit).second;
  } else {
    // if count > 1
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    unsigned int i=1;
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit, i++) {
      if (i==count) {
	return (*emit).second;
      }
    }
    return itsList.end();
  }
}

void ElementList::print(int ident) {
  if(ident == 0) printf("using line %s\n",current_line.c_str());

  for(ElementListIterator it=begin();it!=end();it++)
    {
      (*it).print(ident);
    }
}
