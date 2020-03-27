/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "parser.h"

#include <cmath>
#include <iostream>

// for getpwuid: http://linux.die.net/man/3/getpwuid
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "array.h"
#include "sym_table.h"

namespace {
  // helper method
  // replace algorithm of all substring instances
  // from http://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
  void replaceAll(std::string& source, const std::string& from, const std::string& to)
  {
    std::string newString;
    newString.reserve( source.length() );  // avoids a few memory allocations
    
    std::string::size_type lastPos = 0;
    std::string::size_type findPos;
    
    while( std::string::npos != ( findPos = source.find( from, lastPos )))
      {
        newString.append( source, lastPos, findPos - lastPos );
        newString += to;
        lastPos = findPos + from.length();
      }

    // Care for the rest after last occurrence
    newString += source.substr( lastPos );
    
    source.swap( newString );
  }
}

using namespace GMAD;

namespace GMAD {
  extern std::string yyfilename;
}

extern int yyparse();
extern FILE *yyin;

Parser* Parser::instance = nullptr;

Parser* Parser::Instance()
{
  if (!instance) {
    std::cerr << "Parser has not been initialized!" << std::endl;
    exit(1);
  }
  return instance;
}

Parser* Parser::Instance(const std::string& name)
{
  if(instance) {
    std::cerr << "Warning parser was already initialized!" << std::endl;
    delete instance;
  }
  instance = new Parser(name);
  return instance;
}

Parser::~Parser()
{
  beamline_list.erase();
  // delete allocated lines
  for (auto element : allocated_lines)
    {delete element;}
  
  instance = nullptr;
}

Parser::Parser(std::string name)
{
  instance = this;
#ifdef BDSDEBUG
  std::cout << "gmad_parser> opening file" << std::endl;
#endif
  // replace all ~ symbols with home directory to allow for that
  // note $HOME is not necessarily equivalent to ~
  // see http://linux.die.net/man/3/getpwuid
  std::string tilde("~");
  std::string home(getpwuid(getuid())->pw_dir);

  replaceAll(name,tilde,home);
  
  FILE *f = fopen(name.c_str(),"r");

  if(f==nullptr) {
    std::cerr << "gmad_parser> Can't open input file " << name << std::endl;
    exit(1);
  }
  // set global string for parser
  yyfilename = std::string(name);

  Initialise();
  
  std::cout.precision(10); // set output precision to 10 decimals

  ParseFile(f);
}

void Parser::ParseFile(FILE *f)
{
  yyin=f; 

#ifdef BDSDEBUG
  std::cout << "gmad_parser> beginning to parse file" << std::endl;
#endif

  while(!feof(yyin))
    {
      yyparse();
    }
  
  expand_sequences();
#ifdef BDSDEBUG
  std::cout << "gmad_parser> finished to parsing file" << std::endl;
#endif
  // clear temporary stuff
#ifdef BDSDEBUG
  std::cout << "gmad_parser> clearing temporary lists" << std::endl;
#endif
  element_list.clear();
  tmp_list.clear();
  symtab_map.clear();
  for(auto it : var_list)
    {delete it;}

#ifdef BDSDEBUG
  std::cout << "gmad_parser> finished" << std::endl;
#endif

  fclose(f);
}

void Parser::Initialise()
{
  const int reserved = 1;
  // embedded arithmetical functions
  add_func("sqrt",std::sqrt);
  add_func("cos",std::cos);
  add_func("sin",std::sin);
  add_func("exp",std::exp);
  add_func("log",std::log);
  add_func("tan",std::tan);
  add_func("asin",std::asin);
  add_func("acos",std::acos);
  add_func("atan",std::atan);
  add_func("abs",std::abs);
 
  add_var("pi",     4.0*std::atan(1),reserved);
  add_var("twopi",  8.0*std::atan(1),reserved);
  add_var("halfpi", 2.0*std::atan(1),reserved);

  add_var("PeV",1e6, reserved);
  add_var("TeV",1e+3,reserved);
  add_var("GeV",1.0 ,reserved);
  add_var("MeV",1e-3,reserved);
  add_var("keV",1e-6,reserved);
  add_var("KeV",1e-6,reserved); // for compatibility
  add_var("eV" ,1e-9,reserved);

  add_var("PJ", 1e12,  reserved);
  add_var("GJ", 1e9,   reserved);
  add_var("MJ", 1e6,   reserved);
  add_var("kJ", 1e3,   reserved);
  add_var("J",  1,     reserved);
  add_var("mJ", 1e-3,  reserved);
  add_var("uJ", 1e-6,  reserved);
  add_var("nJ", 1e-9,  reserved);
  add_var("pJ", 1e-12, reserved);

  add_var("V" ,1.0, reserved);
  add_var("kV",1e+3,reserved);
  add_var("MV",1e+6,reserved);

  add_var("Tesla",1.0,reserved);
  add_var("T",    1.0,reserved);

  add_var("km" ,1e3 ,reserved);
  add_var("m"  ,1.0 ,reserved);
  add_var("cm" ,1e-2,reserved);
  add_var("mm" ,1e-3,reserved);
  add_var("um" ,1e-6,reserved);
  add_var("mum",1e-6,reserved);
  add_var("nm" ,1e-9,reserved);
  add_var("ang",1e-10,reserved);
  add_var("pm" ,1e-12,reserved);

  add_var("s"  ,1.0  ,reserved);
  add_var("ms" ,1.e-3,reserved);
  add_var("us" ,1.e-6,reserved);
  add_var("ns" ,1.e-9,reserved);
  add_var("ps" ,1.e-12,reserved);

  add_var("Hz" ,1.0,  reserved);
  add_var("kHz",1e+3, reserved);
  add_var("MHz",1e+6, reserved);
  add_var("GHz",1e+9, reserved);

  add_var("rad" ,1.0  ,reserved);
  add_var("mrad",1.e-3,reserved);
  add_var("urad",1.e-6,reserved);

  add_var("degrees",std::atan(1)/45,reserved);

  add_var("clight",2.99792458e+8,reserved);

  params.flush();
}

void Parser::quit()
{
  std::cout << "parsing complete..." << std::endl;
  exit(0);
}

void Parser::write_table(std::string* name, ElementType type, bool isLine)
{
  Element e;
  e.set(params,*name,type);
  if (isLine)
    {
      e.lst = new std::list<Element>(tmp_list);
      allocated_lines.push_back(e.lst);
      // clean list
      tmp_list.clear();
      sequences.push_back(*name); // append to all sequence definitions
    }
  
  // insert element with uniqueness requirement
  element_list.push_back(e,true);
}

void Parser::expand_sequences()
{
  for (const auto& name : sequences)
    {
      FastList<Element>* newLine = new FastList<Element>();
      expand_line(*newLine, name);
      expandedSequences[name] = newLine;
    }
}

void Parser::expand_line(const std::string& name, std::string start, std::string end)
{
  expand_line(beamline_list, name, start, end);
}

void Parser::expand_line(FastList<Element>& target,
                         const std::string& name,
                         std::string        start,
                         std::string        end)
{
  const Element& line = find_element(name);
  if(line.type != ElementType::_LINE && line.type != ElementType::_REV_LINE ) {
    std::cerr << "'ERROR" << name << "' is not a line" << std::endl;
    exit(1);
  }

  // delete the previous beamline  
  target.clear();
  
  // expand the desired beamline
  Element e;
  e.type = line.type;
  e.name = name;
  e.l = 0;
  e.lst = nullptr;
  
  target.push_back(e);

#ifdef BDSDEBUG 
  std::cout << "expanding line " << name << ", range = " << start << end << std::endl;
#endif
  if(!line.lst) return; //list empty
    
  // first expand the whole range 
  std::list<Element>::iterator sit = line.lst->begin();
  std::list<Element>::iterator eit = line.lst->end();
  
  // copy the list into the resulting list
  switch(line.type){
  case ElementType::_LINE:
    target.insert(target.end(),sit,eit);
    break;
  case ElementType::_REV_LINE:
    target.insert(target.end(),line.lst->rbegin(),line.lst->rend());
    break;
  default:
    target.insert(target.end(),sit,eit);
  }
  // bool to check if beamline is fully expanded
  bool is_expanded = false;
  
  // parse starting from the second element until the list is expanded
  int iteration = 0;
  while(!is_expanded)
    {
      is_expanded = true;
      // start at second element
      std::list<Element>::iterator it = ++target.begin();
      for(;it!=target.end();++it)
        {
          Element& element = *it; // alias
          const ElementType& type = element.type;
#ifdef BDSDEBUG 
          std::cout << element.name << " , " << type << std::endl;
#endif
          // if list - expand further
          if(type != ElementType::_LINE && type != ElementType::_REV_LINE)
            {continue;}
          is_expanded = false;
          // lookup the line in main list
          std::list<Element>::const_iterator tmpit = element_list.find(element.name);
          std::list<Element>::const_iterator iterEnd = element_list.end();
          if( (tmpit != iterEnd) && ( (*tmpit).lst != nullptr) ) { // sublist found and not empty
            const Element& list = *tmpit; // alias
#ifdef BDSDEBUG
            std::cout << "inserting sequence for " << element.name << " - " << list.name << " ...";
#endif
            if(type == ElementType::_LINE)
              target.insert(it,list.lst->begin(),list.lst->end());
            else if(type == ElementType::_REV_LINE){
              //iterate over list and invert any sublines contained within. SPM
              std::list<Element> tmpList;
              tmpList.insert(tmpList.end(),list.lst->begin(),list.lst->end());
              for(std::list<Element>::iterator itLineInverter = tmpList.begin();
                  itLineInverter != tmpList.end(); ++itLineInverter){
                if((*itLineInverter).type == ElementType::_LINE)
                  (*itLineInverter).type = ElementType::_REV_LINE;
                else if ((*itLineInverter).type == ElementType::_REV_LINE)
                  (*itLineInverter).type = ElementType::_LINE;
              }
              target.insert(it,tmpList.rbegin(),tmpList.rend());
            }
#ifdef BDSDEBUG
            std::cout << "inserted" << std::endl;
#endif
            // delete the list pointer
            target.erase(it--);
          } else if ( tmpit != iterEnd ) { // entry points to a scalar element type -
            //transfer properties from the main list
#ifdef BDSDEBUG 
            std::cout << "keeping element..." << element.name << std::endl;
#endif
            // copy properties
            element = (*tmpit);

#ifdef BDSDEBUG 
            std::cout << "done" << std::endl;
#endif
          } else { // element of undefined type
            std::cerr << "Error : Expanding line \"" << name << "\" : element \"" << element.name << "\" has not been defined! " << std::endl;
            exit(1);
          }
        }
      iteration++;
      if( iteration > MAX_EXPAND_ITERATIONS )
        {
          std::cerr << "Error : Line expansion of '" << name << "' seems to loop, " << std::endl
                    << "possible recursive line definition, quitting" << std::endl;
          exit(1);
        }
    }// while
  
  // leave only the desired range
  //
  // rule - from first occurence of 'start' till first 'end' coming after 'start'
  
  if( !start.empty()) // determine the start element
    {
      std::list<Element>::const_iterator startIt = target.find(std::string(start));
      
      if(startIt!=target.end())
        {target.erase(target.begin(),startIt);}
    }
  
  if( !end.empty()) // determine the end element
    {
      std::list<Element>::const_iterator endIt = target.find(std::string(end));
      
      if(endIt!=target.end())
        {target.erase(++endIt,target.end());}
    }
  
  // insert the tunnel if present
  
  std::list<Element>::iterator itTunnel = element_list.find("tunnel");
  if(itTunnel!=element_list.end())
    {target.push_back(*itTunnel);}
}

const FastList<Element>& Parser::get_sequence(const std::string& name)
{
  // search for previously queried beamlines
  const auto search = expandedSequences.find(name);
  if (search != expandedSequences.end())
    {return *(search->second);}
  else
    {std::cerr << "parser> no such sequence \"" << name << "\"" << std::endl; exit(1);} 
}

void Parser::set_sampler(std::string name, int count, ElementType type, std::string samplerType, double samplerRadius)
{
  // if count equal to -2 add to all elements regardless of name
  // typically used for output elements like samplers
  // skip first element and add one at the end
  if (count==-2)
    {
      for (auto it=beamline_list.begin(); it!=beamline_list.end(); ++it) {
	// skip LINEs
	if((*it).type == ElementType::_LINE || (*it).type == ElementType::_REV_LINE)
	  {continue;}
	// if type not equal to NONE and elements have to match type 
	if (type != ElementType::_NONE && type != (*it).type) {
	  continue;
	}

	(*it).setSamplerInfo(samplerType,(*it).name,samplerRadius);
      }
    }
  // if count equal to -1 add sampler to all element instances
  else if (count==-1)
    {
      auto itPair = beamline_list.equal_range(name);
      if (itPair.first==itPair.second) {
	std::cerr<<"current beamline doesn't contain element "<< name << std::endl;
	exit(1);
      }
      for (auto it = itPair.first; it!= itPair.second; ++it) {
        // if sampler is attached to a marker, really attach it to the previous element with the name of marker
        auto elementIt = (it->second);
        std::string samplerName = elementIt->name;
        if ((*elementIt).type == ElementType::_MARKER) {
          // need to find real element before
          // but careful not to go beyond first element also!
          while ((*elementIt).isSpecial()) {
            elementIt--;
            // have to break first before continue since in while loop
            if (elementIt==beamline_list.begin()) break;
          }
	  
          if (elementIt==beamline_list.begin()) {
            std::cout << "WARNING: no element before marker " << name << ", no sampler added" << std::endl;
            continue;
          }
        }
        (*elementIt).setSamplerInfo(samplerType,samplerName,samplerRadius);
      }
    }
  else
    {
      auto it = beamline_list.find(name,count);
      if (it==beamline_list.end()) {
	std::cerr<<"current beamline doesn't contain element "<<name<<" with number "<<count<<std::endl;
	exit(1);
      }
      // if sampler is attached to a marker, really attach it to the previous element with the name of marker
      std::string samplerName = (*it).name;
      if ((*it).type == ElementType::_MARKER) {
        // need to find real element before
        // but careful not to go beyond first element also!
        while ((*it).isSpecial()) {
          it--;
          if (it==beamline_list.begin()) {
            std::cout << "WARNING: no element before marker " << name << ", no sampler added" << std::endl;
            return;
          }
        }
      }
      (*it).setSamplerInfo(samplerType,samplerName,samplerRadius);
    }
}

void Parser::add_sampler(const std::string& name, int count, ElementType type)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting sampler "<<name;
  if (count>=0) std::cout<<"["<< count <<"]";
  std::cout<<std::endl;
#endif

  set_sampler(name,count,type,"plane");
}

void Parser::add_csampler(const std::string& name, int count, ElementType type)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting csampler "<<name;
  if (count>=0) std::cout<<"["<<count<<"]";
  std::cout<<std::endl;
#endif

  set_sampler(name,count,type,"cylinder", params.samplerRadius);
}

Element& Parser::find_element(const std::string& element_name)
{
  std::list<Element>::iterator it = element_list.find(element_name);
  std::list<Element>::const_iterator iterEnd = element_list.end();

  if(it == iterEnd) {
    std::cerr << "parser.h> Error: element (type) \"" << element_name << "\" has not been defined." << std::endl;
    exit(1);
  }
  return (*it);
}

const Element& Parser::find_element(const std::string& element_name)const
{
  std::list<Element>::const_iterator it = element_list.find(element_name);
  std::list<Element>::const_iterator iterEnd = element_list.end();

  if(it == iterEnd) {
    std::cerr << "parser.h> Error: unknown element \"" << element_name << "\"." << std::endl; 
    exit(1);
  }
  return (*it);
}

double Parser::property_lookup(const std::string& element_name, const std::string& property_name)const
{
  const Element& element = find_element(element_name);
  return element.property_lookup(property_name);
}

void Parser::add_element_temp(const std::string& name, int number, bool pushfront, ElementType linetype)
{
#ifdef BDSDEBUG
  std::cout << "matched sequence element, " << name;
  if (number > 1) std::cout << " * " << number;
  std::cout << std::endl;
#endif
  // add to temporary element sequence
  Element e;
  e.name = name;
  e.type = linetype;
  e.lst = nullptr;
  if (pushfront) {
    for(int i=0;i<number;i++) {
      tmp_list.push_front(e);
    }
  }
  else {
    for(int i=0;i<number;i++) {
      tmp_list.push_back(e);
    }
  }
}

int Parser::copy_element_to_params(const std::string& elementName)
{
  int type;
#ifdef BDSDEBUG
  std::cout << "newinstance : VARIABLE -- " << elementName << std::endl;
#endif
  const Element& element = find_element(elementName);

  // inherit properties from the base type
  type = static_cast<int>(element.type);
  params.inherit_properties(element);

  return type;
}

void Parser::add_func(std::string name, double (*func)(double))
{
  Symtab *sp=symtab_map.symcreate(name);
  sp->Set(func);
}

void Parser::add_var(std::string name, double value, int is_reserved)
{
  Symtab *sp=symtab_map.symcreate(name);
  sp->Set(value,is_reserved);
}

Symtab * Parser::symcreate(std::string s)
{
  return symtab_map.symcreate(s);
}

Symtab * Parser::symlook(std::string s)
{
  return symtab_map.symlook(s);
}
void Parser::Store(double value)
{
  tmparray.push_front(value);
}

void Parser::Store(std::string name)
{
  tmpstring.push_front(name);
}

void Parser::FillArray(Array* array)
{
  array->Copy(tmparray);
  tmparray.clear();
}

void Parser::FillString(Array* array)
{
  array->Copy(tmpstring);
  tmpstring.clear();
}

void Parser::ClearParams()
{
  params.flush();
}

void Parser::Overwrite(const std::string& objectName)
{
  // find object and set values

  // possible object types are:
  // element, atom, colour, crystal, field, material, physicsbiasing, placement,
  // query, region, tunnel, cavitymodel, samplerplacement, aperture, blm
  bool extended = false;
  auto element_it = element_list.find(objectName);
  if (element_it != element_list.end())
    {
      ExtendObject(*element_it);
      extended = true;
    }
  else
    {
      auto it = xsecbias_list.find(objectName);
      if (it != xsecbias_list.end() )
	{
	  ExtendObject(*it);
	  extended = true;
	}
    }
  // vectors
  if (extended == false) {
    if (      (extended = FindAndExtend<Atom>       (objectName)) ) {}
    else if ( (extended = FindAndExtend<NewColour>  (objectName)) ) {}
    else if ( (extended = FindAndExtend<Crystal>    (objectName)) ) {}
    else if ( (extended = FindAndExtend<Field>      (objectName)) ) {}
    else if ( (extended = FindAndExtend<Material>   (objectName)) ) {}
    else if ( (extended = FindAndExtend<Placement>  (objectName)) ) {}
    else if ( (extended = FindAndExtend<Query>      (objectName)) ) {}
    else if ( (extended = FindAndExtend<Region>     (objectName)) ) {}
    else if ( (extended = FindAndExtend<Tunnel>     (objectName)) ) {}
    else if ( (extended = FindAndExtend<CavityModel>(objectName)) ) {}
    else if ( (extended = FindAndExtend<SamplerPlacement>(objectName)) ) {}
    else if ( (extended = FindAndExtend<Aperture>   (objectName)) ) {}
    else if ( (extended = FindAndExtend<BLMPlacement> (objectName)) ) {}
    else if ( (extended = FindAndExtend<Laser>      (objectName)) ) {}
  }

  if (extended==false)
    {
      std::cerr << "parser.h> Error: object \"" << objectName
		<< "\" has not been defined and can't be extended." << std::endl;
      exit(1);
    }

  // clear maps
  extendedNumbers.clear();
  extendedStrings.clear();
  extendedVectors.clear();
}

template <class C>
bool Parser::FindAndExtend(const std::string& objectName)
{
  auto vec = GetList<C>();
  for (auto it = vec.begin(); it!=vec.end(); ++it) {
    if ((*it).name == objectName) {
      ExtendObject(*it);
      return true;
    }
  }
  return false;
}

template<class C>
void Parser::ExtendObject(C& object)
{
  for (auto option : extendedNumbers)
    {object.set_value(option.first, option.second);}
  for (auto option : extendedStrings)
    {object.set_value(option.first, option.second);}
  for (auto option : extendedVectors)
    {object.set_value(option.first, option.second);}
}

void Parser::AddVariable(std::string* name)
{
  var_list.push_back(name);
}

void Parser::PrintBeamline()const
{
  beamline_list.print();
}

void Parser::PrintElements()const
{
  element_list.print();
}

void Parser::PrintOptions()const
{
  options.print();
}

const FastList<Element>& Parser::GetBeamline()const
{
  return beamline_list;
}

//template specialisation
// put explicitly in namespace since g++ complains
namespace GMAD {
  template<>
  Beam& Parser::GetGlobal(){return beam;}
  
  template<>
  Parameters& Parser::GetGlobal(){return params;}

  template<>
  Options& Parser::GetGlobal(){return options;}

  template<>
  Region& Parser::GetGlobal(){return region;}

  template<>
  std::vector<Region>& Parser::GetList<Region>(){return region_list;}

  template<>
  NewColour& Parser::GetGlobal(){return colour;}

  template<>
  std::vector<NewColour>& Parser::GetList<NewColour>(){return colour_list;}
  
  template<>
  Crystal& Parser::GetGlobal(){return crystal;}

  template<>
  std::vector<Crystal>& Parser::GetList<Crystal>(){return crystal_list;}
  
  template<>
  Field& Parser::GetGlobal(){return field;}

  template<>
  std::vector<Field>& Parser::GetList<Field>(){return field_list;}

  template<>
  Query& Parser::GetGlobal(){return query;}
  
  template<>
  std::vector<Query>& Parser::GetList<Query>(){return query_list;}
  
  template<>
  Atom& Parser::GetGlobal(){return atom;}

  template<>
  std::vector<Atom>& Parser::GetList<Atom>(){return atom_list;}

  template<>
  Material& Parser::GetGlobal(){return material;}

  template<>
  std::vector<Material>& Parser::GetList<Material>(){return material_list;}

  template<>
  Tunnel& Parser::GetGlobal(){return tunnel;}

  template<>
  std::vector<Tunnel>& Parser::GetList<Tunnel>(){return tunnel_list;}

  template<>
  CavityModel& Parser::GetGlobal(){return cavitymodel;}

  template<>
  std::vector<CavityModel>& Parser::GetList<CavityModel>(){return cavitymodel_list;}

  template<>
  Placement& Parser::GetGlobal(){return placement;}

  template<>
  std::vector<Placement>& Parser::GetList<Placement>() {return placement_list;}

  template<>
  Laser&     Parser::GetGlobal(){return laser;}

  template<>
  std::vector<Laser>& Parser::GetList<Laser>() {return laser_list;}

  template<>
  PhysicsBiasing& Parser::GetGlobal(){return xsecbias;}

  template<>
  FastList<PhysicsBiasing>& Parser::GetList<PhysicsBiasing, FastList<PhysicsBiasing>>(){return xsecbias_list;}

  template<>
  SamplerPlacement& Parser::GetGlobal(){return samplerplacement;}

  template<>
  std::vector<SamplerPlacement>& Parser::GetList<SamplerPlacement>() {return samplerplacement_list;}

  template<>
  BLMPlacement& Parser::GetGlobal() {return blm;}

  template<>
  std::vector<BLMPlacement>& Parser::GetList<BLMPlacement>() {return blm_list;}

  template<>
  Aperture& Parser::GetGlobal() {return aperture;}

  template<>
  std::vector<Aperture>& Parser::GetList<Aperture>() {return aperture_list;}
  
  template<>
  void Parser::ExtendValue(const std::string& property, double value)
  {extendedNumbers[property]=value;}

  template<>
  void Parser::ExtendValue(const std::string& property, std::string value)
  {extendedStrings[property]=value;}

  template<>
  void Parser::ExtendValue(const std::string& property, Array* value)
  {extendedVectors[property]=value;}
}
