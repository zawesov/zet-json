/*
Copyright (C) Alexander Zavesov
Copyright (C) ZET-JSON
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <algorithm>
#include <functional>

#include "zJSON.h"

#define PARSE_BLANK(p, len, pos)\
{\
 for(;pos < len;++pos)\
 {\
  if(p[pos] == ' ' || p[pos] == '\t' || p[pos] == '\r' || p[pos] == '\n') continue;\
  if(p[pos] == '#' || (p[pos] == '/' && p[pos+1] == '/'))\
  {\
   for(++pos;pos < len;++pos) { if(p[pos] == '\n') break; }\
   continue;\
  }\
  if(p[pos] == '/' && p[pos+1] == '*')\
  {\
   for(pos+=2;pos < len;++pos) { if(p[pos] == '*' && p[pos+1] == '/') { ++pos; break; } }\
   continue;\
  }\
  break;\
 }\
 if(pos > len) pos=len;\
}\


static const char __digs__[37] = { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

static bool hex_to_num(size_t& ret, const char* src, size_t len=4)
{
 ret=0;
 for(size_t i = 0; i < len; i++)
 {
  ret <<= 4;
  switch(src[i])
  {
   case 'a': { ret+=((unsigned char) '\x0A'); break; }
   case 'A': { ret+=((unsigned char) '\x0A'); break; }
   case 'b': { ret+=((unsigned char) '\x0B'); break; }
   case 'B': { ret+=((unsigned char) '\x0B'); break; }
   case 'c': { ret+=((unsigned char) '\x0C'); break; }
   case 'C': { ret+=((unsigned char) '\x0C'); break; }
   case 'd': { ret+=((unsigned char) '\x0D'); break; }
   case 'D': { ret+=((unsigned char) '\x0D'); break; }
   case 'e': { ret+=((unsigned char) '\x0E'); break; }
   case 'E': { ret+=((unsigned char) '\x0E'); break; }
   case 'f': { ret+=((unsigned char) '\x0F'); break; }
   case 'F': { ret+=((unsigned char) '\x0F'); break; }
   case '0': { ret+=((unsigned char) '\x00'); break; }
   case '1': { ret+=((unsigned char) '\x01'); break; }
   case '2': { ret+=((unsigned char) '\x02'); break; }
   case '3': { ret+=((unsigned char) '\x03'); break; }
   case '4': { ret+=((unsigned char) '\x04'); break; }
   case '5': { ret+=((unsigned char) '\x05'); break; }
   case '6': { ret+=((unsigned char) '\x06'); break; }
   case '7': { ret+=((unsigned char) '\x07'); break; }
   case '8': { ret+=((unsigned char) '\x08'); break; }
   case '9': { ret+=((unsigned char) '\x09'); break; }
   default: { return false; }
  }
 }
 return true;
};

static std::string str_to_json(const char* p, size_t len, size_t start_pos=0, size_t parse_len=std::string::npos)
{
 std::string ret;
 if(start_pos >= len) return ret;
 size_t n=parse_len;
 if((start_pos+parse_len) > len) n= len-start_pos;
 p+=start_pos;
 ret.reserve(n*6);
 char c;
 for(size_t i=0; i < n; ++i, ++p)
 {
  c=*p;
  switch(c)
  {
   case '\"': { ret.append("\\\"", 2); break; }
   case '\\': { ret.append("\\\\", 2); break; }
   case '/': { ret.append("\\/", 2); break; }
   case '\b': { ret.append("\\b", 2); break; }
   case '\f': { ret.append("\\f", 2); break; }
   case '\n': { ret.append("\\n", 2); break; }
   case '\r': { ret.append("\\r", 2); break; }
   case '\t': { ret.append("\\t", 2); break; }
   case '\x7F': { ret.append("\\u007F", 6); break; }
   default:
   {
    if('\x00' <= c && c < '\x20')
    {
     ret.append("\\u00", 4);
     ret+=__digs__[((unsigned char) c) >> 4];
     ret+=__digs__[((unsigned char) c) & 0xF];
     break;
    }
    ret+=c;
    break;
   }
  }
 }
 return ret;
};

static std::string str_to_json_formatted(const char* p, size_t len, size_t start_pos=0, size_t parse_len=std::string::npos)
{
 std::string ret;
 if(start_pos >= len) return ret;
 size_t n=parse_len;
 if((start_pos+parse_len) > len) n= len-start_pos;
 p+=start_pos;
 ret.reserve(n*6);
 char c;
 for(size_t i=0; i < n; ++i, ++p)
 {
  c=*p;
  switch(c)
  {
   case '\"': { ret.append("\\\"", 2); break; }
   case '\\': { ret.append("\\\\", 2); break; }
   case '/': { ret.append("\\/", 2); break; }
   case '\b': { ret.append("\\b", 2); break; }
   case '\f': { ret.append("\\f", 2); break; }
   case '\n': { ret+=c; break; }
   case '\r': { ret+=c; break; }
   case '\t': { ret+=c; break; }
   case '\x7F': { ret.append("\\u007F", 6); break; }
   default:
   {
    if('\x00' <= c && c < '\x20')
    {
     ret.append("\\u00", 4);
     ret+=__digs__[((unsigned char) c) >> 4];
     ret+=__digs__[((unsigned char) c) & 0xF];
     break;
    }
    ret+=c;
    break;
   }
  }
 }
 return ret;
};

static bool json_to_str(std::string& ret, const char* p, size_t len, size_t start_pos=0, size_t parse_len=std::string::npos)
{
 if(start_pos >= len) return false;
 size_t n=parse_len;
 if((start_pos+parse_len) > len) n= len-start_pos;
 p+=start_pos;
 ret.reserve(n+1);
 size_t unicode=0;
 size_t unicode1=0;
 size_t unicode2=0;
 for(size_t i=0; i < n; ++i, ++p)
 {
  if(*p == '\\')
  {
   if((i+1) == n) continue;
   ++i; ++p;
   switch(*p)
   {
    case 'b': { ret+='\b'; break; }
    case 'f': { ret+='\f'; break; }
    case 'n': { ret+='\n'; break; }
    case 'r': { ret+='\r'; break; }
    case 't': { ret+='\t'; break; }
    case 'u':
    {
     if((i+4) < n && hex_to_num(unicode1, p+1))
     {
      unicode=unicode1;
      if(unicode1 >= 0xD800 && unicode1 <= 0xDBFF)
      {
       if((i+10) >= n || p[5] != '\\' || p[6] != 'u') { ret.append(p-1, 6); i+=4; p+=4; break; }
       if(!hex_to_num(unicode2, p+7)) { ret.append(p-1, 6); i+=4; p+=4; break; }
       if(unicode2 >= 0xDC00 && unicode2 <= 0xDFFF) { unicode = (unicode1 << 10)+unicode2-0x35FDC00; }
       else { ret.append(p-1, 6); i+=4; p+=4; break; }
      }
      else unicode2=0;

      if(unicode < 0x80) { ret+=(unsigned char) unicode; }
      else if(unicode <= 0x7FF)
      {
       ret+=(unsigned char) (0xC0 | ((unicode >> 6) & 0x1F));
       ret+=(unsigned char) (0x80 | (unicode & 0x3F));
      }
      else if(unicode <= 0xFFFF)
      {
       ret+=(unsigned char) (0xE0 | ((unicode >> 12) & 0x0F));
       ret+=(unsigned char) (0x80 | ((unicode >> 6) & 0x3F));
       ret+=(unsigned char) (0x80 | (unicode & 0x3F));
      }
      else if(unicode <= 0x10FFFF)
      {
       ret+=(unsigned char) (0xF0 | ((unicode >> 18) & 0x07));
       ret+=(unsigned char) (0x80 | ((unicode >> 12) & 0x3F));
       ret+=(unsigned char) (0x80 | ((unicode >> 6) & 0x3F));
       ret+=(unsigned char) (0x80 | (unicode & 0x3F));
      }
      else
      {
       if(unicode2) ret.append(p-1, 12);
       else ret.append(p-1, 6);
      }

      if(unicode2) { i+=10; p+=10; }
      else { i+=4; p+=4; }
      break;
     }
     ret+=*p; break;
    }
    default: { ret+=*p; break; }
   }
  }
  else ret+=*p;
 }
 return true;
};

static size_t read_string(const char* p, size_t len, size_t start_pos)
{
 if(start_pos >= len || p[start_pos] != '\"') return std::string::npos;
 size_t n= len-start_pos-1;
 p+=(start_pos+1);
 for(size_t i=0; i < n; ++i, ++p)
 {
  if(*p == '\"') return (start_pos+1+i);
  if(*p == '\\') { ++i; ++p; }
 }
 return std::string::npos;
};

static void parse_blank(const char* p, size_t len, size_t& pos)
{
 for(;pos < len;++pos)
 {
  if(p[pos] == ' ' || p[pos] == '\t' || p[pos] == '\r' || p[pos] == '\n') continue;
  if(p[pos] == '#' || (p[pos] == '/' && p[pos+1] == '/'))
  {
   for(++pos;pos < len;++pos) { if(p[pos] == '\n') break; }
   continue;
  }
  if(p[pos] == '/' && p[pos+1] == '*')
  {
   for(pos+=2;pos < len;++pos) { if(p[pos] == '*' && p[pos+1] == '/') { ++pos; break; } }
   continue;
  }
  break;
 }
 if(pos > len) pos=len;
};

static zJSON* parse_json(const char* p, size_t len, size_t& pos);

static bool parse_name(std::string& ret, const char* p, size_t len, size_t& pos)
{
 size_t n=read_string(p, len, pos);
 if(n == std::string::npos) return false;
 size_t l=(n+1);
 PARSE_BLANK(p, len, l)
 if(p[l] != ':') return false;
 json_to_str(ret, p, len, pos+1, n-pos-1);
 pos=(l+1);
 return true;
};

static zJSON* parse_null_bool(const char* p, size_t& pos, const std::string& json_name)
{
 p+=pos;
 if(p[0] == 'n' && p[1] == 'u' && p[2] == 'l' && p[3] == 'l') { pos+=4; return new zJSON(zJSON::JSON_NULL, json_name); }
 if(p[0] == 't' && p[1] == 'r' && p[2] == 'u' && p[3] == 'e') { pos+=4; return new zJSON(json_name, true); }
 if(p[0] == 'f' && p[1] == 'a' && p[2] == 'l' && p[3] == 's' && p[4] == 'e') { pos+=5; return new zJSON(json_name, false); }
 return NULL;
};

static zJSON* parse_integer_number(const char* p, size_t len, size_t& pos, const std::string& json_name)
{
 int sign=1;
 int point=0;
 unsigned exponenta=0;
 double multiplier=1.0;
 double ret=0.0;
 p+=pos;
 size_t i=0;
 switch(*p)
 {
  case '-': { sign=-1; }
  case '+': { ++p; ++i; }
 }
 PARSE_BLANK(p, len, i)
 if(*p < '0' || *p > '9') return NULL;
 for(;;++p, ++i)
 {
  if(*p == '.') { if(point) { break; } point=1; continue; }
  if(*p == 'e' || *p == 'E')
  {
   if(point > 1) { break; }
   switch(*(p+1))
   {
    case '-': { point=4; ++p; ++i; break; }
    case '+': { point=2; ++p; ++i; break; }
    default:  { point=2; }
   }
   continue;
  }
  if(*p < '0' || *p > '9') break;
  if(point < 2) { ret*=10.; ret+=((unsigned char) *p - '0'); if(point) { multiplier*=10.; } }
  else { exponenta*=10; exponenta+=((unsigned char) *p - '0'); if(exponenta > 308) return NULL; }
 }
 if(multiplier != 0.0 && multiplier != 1.) { ret/=multiplier; }
 if(point & 2) { for(size_t j=0; j < exponenta; j++) { ret*=10.; } }
 else if(point & 4) { for(size_t j=0; j < exponenta; j++) { ret/=10.; } }
 ret*=sign;
 pos+=i;
 if(point) return new zJSON(json_name, ret);
 return new zJSON(json_name, (int64_t) ret);
};

static zJSON* parse_string(const char* p, size_t len, size_t& pos, const std::string& json_name)
{
 size_t n=read_string(p, len, pos);
 if(n == std::string::npos) return NULL;
 std::string s;
 json_to_str(s, p, len, pos+1, n-pos-1);
 pos=(n+1);
 for(;;)
 {
  PARSE_BLANK(p, len, pos)
  n=read_string(p, len, pos);
  if(n == std::string::npos) break;
  json_to_str(s, p, len, pos+1, n-pos-1);
  pos=(n+1);
 }
 zJSON* ret= new zJSON(json_name, s);
 return ret;
};

static zJSON* parse_array_node(const char* p, size_t len, size_t& pos, const std::string& json_name)
{
 zJSON* ret=NULL;
 switch(p[pos])
 {
  case '[': { ret= new zJSON(zJSON::JSON_ARRAY, json_name); break; }
  case '{': { ret= new zJSON(zJSON::JSON_NODE, json_name); break; }
  default: { return NULL; }
 }
 size_t l=pos+1;
 PARSE_BLANK(p, len, l)
 if(ret->type() == zJSON::JSON_ARRAY) { if(p[l] == ']') { pos=l+1; return ret; } }
 else if(p[l] == '}') { pos=l+1; return ret; }
 for(zJSON* p_json=parse_json(p, len, l); p_json != NULL; p_json=parse_json(p, len, l))
 {
  ret->push_back(p_json);
  PARSE_BLANK(p, len, l)
  if(p[l] == ',') 
  {
   ++l;
   PARSE_BLANK(p, len, l)
   if(ret->type() == zJSON::JSON_ARRAY) { if(p[l] == ']') { pos=l+1; return ret; } }
   else if(p[l] == '}') { pos=l+1; return ret; }
   continue; 
  }
  if(ret->type() == zJSON::JSON_ARRAY) { if(p[l] == ']') { pos=l+1; return ret; } }
  else if(p[l] == '}') { pos=l+1; return ret; }
 }
 delete ret; return NULL;
};

static zJSON* parse_json(const char* p, size_t len, size_t& pos)
{
 std::string json_name;
 PARSE_BLANK(p, len, pos)
 size_t l=pos;
 parse_name(json_name, p, len, l);
 zJSON* ret=NULL;
 PARSE_BLANK(p, len, l)
 ret=parse_null_bool(p, l, json_name);
 if(ret) { pos=l; return ret; }
 ret=parse_integer_number(p, len, l, json_name);
 if(ret) { pos=l; return ret; }
 ret=parse_string(p, len, l, json_name);
 if(ret) { pos=l; return ret; }
 ret=parse_array_node(p, len, l, json_name);
 if(ret) { pos=l; return ret; }
 return NULL;
};

void zJSON::zjson_null::write(std::string& ret, const zJSON* const prn) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\":"):"")+"null"; };

void zJSON::zjson_null::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):std::string(level,' '))+"null"; };

void zJSON::zjson_bool::write(std::string& ret, const zJSON* const prn) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):"")+((value)?("true"):("false")); };

void zJSON::zjson_bool::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):std::string(level,' '))+((value)?("true"):("false")); };

void zJSON::zjson_integer::write(std::string& ret, const zJSON* const prn) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):"")+zJSON::toString(value); };

void zJSON::zjson_integer::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):std::string(level,' '))+zJSON::toString(value); };

void zJSON::zjson_number::write(std::string& ret, const zJSON* const prn) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):"")+zJSON::toString(value); };

void zJSON::zjson_number::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):std::string(level,' '))+zJSON::toString(value); };

void zJSON::zjson_string::write(std::string& ret, const zJSON* const prn) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):"")+('\"'+str_to_json(value.c_str(), value.size())+'\"'); };

void zJSON::zjson_string::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{ ret+=((prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" : "):std::string(level,' '))+('\"'+str_to_json_formatted(value.c_str(), value.size())+'\"'); };

zJSON::zjson_array::zjson_array(const std::vector<zJSON*>& v, zJSON* prn)
{
 zJSON* p;
 for(size_t i=0; i < v.size(); i++)
 {
  p= new zJSON(*(v[i]));
  p->m_parent=prn;
  value.push_back(p);
 }
};

zJSON::zjson_array::~zjson_array() { clear(); };

void zJSON::zjson_array::set_parent(zJSON* p) { for(size_t i=0; i < value.size(); i++) { value[i]->m_parent=p; } };

size_t zJSON::zjson_array::index(const zJSON* const p) const
{
 for(size_t i=0; i < value.size(); i++) { if(value[i] == p) return i; }
 return std::string::npos;
};

size_t zJSON::zjson_array::find(const std::string& json_name, size_t start_pos) const
{
 size_t n=value.size();
 for(size_t i=start_pos; i < n; i++) { if(value[i]->m_name == json_name) return i; }
 return std::string::npos;
};

zJSON* zJSON::zjson_array::search(const std::string& json_name, size_t start_pos) const
{
 size_t n=value.size();
 for(size_t i=start_pos; i < n; i++) { if(value[i]->m_name == json_name) return value[i]; }
 return NULL;
};

void zJSON::zjson_array::clear() { for(size_t i=0; i < value.size(); i++) { value[i]->m_parent=NULL; delete value[i]; } value.clear(); };

zJSON* zJSON::zjson_array::insert(size_t pos, const zJSON& val, zJSON* prn)
{
 if(prn == NULL) return NULL;
 if(pos >= value.size()) return push_back(val, prn);
 zJSON* p= new zJSON(val);
 p->m_parent=prn;
 value.insert(value.begin()+pos, p);
 return p;
};

zJSON* zJSON::zjson_array::insert(size_t pos, zJSON* p, zJSON* prn)
{
 if(p == NULL || prn == NULL || p->m_parent != NULL || prn->root() == p) return NULL;
 if(pos >= value.size()) return push_back(p, prn);
 p->m_parent=prn;
 value.insert(value.begin()+pos, p);
 return p;
};

zJSON* zJSON::zjson_array::push_back(const zJSON& val, zJSON* prn)
{
 if(prn == NULL) return NULL;
 zJSON* p= new zJSON(val);
 p->m_parent=prn;
 value.push_back(p);
 return p;
};

zJSON* zJSON::zjson_array::push_back(zJSON* p, zJSON* prn)
{
 if(p == NULL || prn == NULL || p->m_parent != NULL || prn->root() == p) return NULL;
 p->m_parent=prn;
 value.push_back(p);
 return p;
};

bool zJSON::zjson_array::erase(size_t pos)
{
 if(pos >= value.size()) return false;
 value[pos]->m_parent=NULL;
 delete value[pos];
 value.erase(value.begin()+pos);
 return true;
};

bool zJSON::zjson_array::pop_back()
{
 size_t pos=value.size();
 if(pos == 0) return false;
 --pos;
 value[pos]->m_parent=NULL;
 delete value[pos];
 value.pop_back();
 return true;
};

zJSON* zJSON::zjson_array::remove(zJSON* p)
{
 for(size_t i=0; i < value.size(); i++)
 {
  if(p == value[i])
  {
   value.erase(value.begin()+i);
   p->m_parent=NULL;
   break;
  }
 }
 return p;
};

void zJSON::zjson_array::write(std::string& ret, const zJSON* const prn) const
{
 ret+=(prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\":["):"[";
 if(value.size()) value[0]->write(ret);
 for(size_t i=1; i < value.size(); i++) { ret+=','; value[i]->write(ret); }
 ret+=']';
};

void zJSON::zjson_array::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{
 ret+=(prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" :\n"+std::string(level,' ')+"[\n"):std::string(level,' ')+"[\n";
 if(value.size()) value[0]->m_value->write_formatted(ret, value[0], level+1);
 for(size_t i=1; i < value.size(); i++) { ret+=",\n"; value[i]->m_value->write_formatted(ret, value[i], level+1); }
 ret+='\n'+std::string(level,' ')+"]";
};

zJSON::zjson_node::zjson_node(const std::vector<zJSON*>& v, zJSON* prn)
{
 zJSON* p;
 for(size_t i=0; i < v.size(); i++)
 {
  p= new zJSON(*(v[i]));
  p->m_parent=prn;
  value.push_back(p);
 }
};

zJSON::zjson_node::~zjson_node() { clear(); };

void zJSON::zjson_node::set_parent(zJSON* p) { for(size_t i=0; i < value.size(); i++) { value[i]->m_parent=p; } };

size_t zJSON::zjson_node::index(const zJSON* const p) const
{
 for(size_t i=0; i < value.size(); i++) { if(value[i] == p) return i; }
 return std::string::npos;
};

size_t zJSON::zjson_node::find(const std::string& json_name, size_t start_pos) const
{
 size_t n=value.size();
 for(size_t i=start_pos; i < n; i++) { if(value[i]->m_name == json_name) return i; }
 return std::string::npos;
};

zJSON* zJSON::zjson_node::search(const std::string& json_name, size_t start_pos) const
{
 size_t n=value.size();
 for(size_t i=start_pos; i < n; i++) { if(value[i]->m_name == json_name) return value[i]; }
 return NULL;
};

void zJSON::zjson_node::clear() { for(size_t i=0; i < value.size(); i++) { value[i]->m_parent=NULL; delete value[i]; } value.clear(); };

zJSON* zJSON::zjson_node::insert(size_t pos, const zJSON& val, zJSON* prn)
{
 if(prn == NULL) return NULL;
 if(pos >= value.size()) return push_back(val, prn);
 zJSON* p= new zJSON(val);
 p->m_parent=prn;
 value.insert(value.begin()+pos, p);
 return p;
};

zJSON* zJSON::zjson_node::insert(size_t pos, zJSON* p, zJSON* prn)
{
 if(p == NULL || prn == NULL || p->m_parent != NULL || prn->root() == p) return NULL;
 if(pos >= value.size()) return push_back(p, prn);
 p->m_parent=prn;
 value.insert(value.begin()+pos, p);
 return p;
};

zJSON* zJSON::zjson_node::push_back(const zJSON& val, zJSON* prn)
{
 if(prn == NULL) return NULL;
 zJSON* p= new zJSON(val);
 p->m_parent=prn;
 value.push_back(p);
 return p;
};

zJSON* zJSON::zjson_node::push_back(zJSON* p, zJSON* prn)
{
 if(p == NULL || prn == NULL || p->m_parent != NULL || prn->root() == p) return NULL;
 p->m_parent=prn;
 value.push_back(p);
 return p;
};

bool zJSON::zjson_node::erase(size_t pos)
{
 if(pos >= value.size()) return false;
 value[pos]->m_parent=NULL;
 delete value[pos];
 value.erase(value.begin()+pos);
 return true;
};

bool zJSON::zjson_node::pop_back()
{
 size_t pos=value.size();
 if(pos == 0) return false;
 --pos;
 value[pos]->m_parent=NULL;
 delete value[pos];
 value.pop_back();
 return true;
};

zJSON* zJSON::zjson_node::remove(zJSON* p)
{
 for(size_t i=0; i < value.size(); i++)
 {
  if(p == value[i])
  {
   value.erase(value.begin()+i);
   p->m_parent=NULL;
   break;
  }
 }
 return p;
};

void zJSON::zjson_node::write(std::string& ret, const zJSON* const prn) const
{
 ret+=(prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?('\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\":{"):"{";
 if(value.size()) value[0]->write(ret);
 for(size_t i=1; i < value.size(); i++) { ret+=','; value[i]->write(ret); }
 ret+='}';
};

void zJSON::zjson_node::write_formatted(std::string& ret, const zJSON* const prn, size_t level) const
{
 ret+=(prn->m_name.size() && ((prn->m_parent)?(prn->m_parent->type() != zJSON::JSON_ARRAY):true))?(std::string(level,' ')+'\"'+str_to_json(prn->m_name.c_str(), prn->m_name.size())+"\" :\n"+std::string(level,' ')+"{\n"):std::string(level,' ')+"{\n";
 if(value.size()) value[0]->m_value->write_formatted(ret, value[0], level+1);
 for(size_t i=1; i < value.size(); i++) { ret+=",\n"; value[i]->m_value->write_formatted(ret, value[i], level+1); }
 ret+='\n'+std::string(level,' ')+"}";
};


template <class T> static std::string toString(const T& t)
{
 std::ostringstream ss;
 ss << t;
 return ss.str();
};

std::string zJSON::toString(short value) { return toString((int64_t) value); };
std::string zJSON::toString(unsigned short value) { return toString((uint64_t) value); };
std::string zJSON::toString(int value) { return toString((int64_t) value); };
std::string zJSON::toString(unsigned value) { return toString((uint64_t) value); };

std::string zJSON::toString(int64_t value)
{
 if(value == 0) return "0";
 std::string ret;
 ret.reserve(24);
 int sign=1;
 if(value < 0) { sign=-1; value*=(-1); }
 for(; value; value/=10)
 {
  switch(value%10)
  {
   case 0:  { ret+='0'; break; }
   case 1:  { ret+='1'; break; }
   case 2:  { ret+='2'; break; }
   case 3:  { ret+='3'; break; }
   case 4:  { ret+='4'; break; }
   case 5:  { ret+='5'; break; }
   case 6:  { ret+='6'; break; }
   case 7:  { ret+='7'; break; }
   case 8:  { ret+='8'; break; }
   case 9:  { ret+='9'; break; }
  }
 }
 if(sign < 0) ret+='-';
 char c;
 size_t n=ret.size();
 size_t l=n/2;
 --n;
 for(size_t i=0; i < l; i++) { c=ret[i]; ret[i]=ret[n-i]; ret[n-i]=c; }
 return ret;
};

std::string zJSON::toString(uint64_t value)
{
 if(value == 0) return "0";
 std::string ret;
 ret.reserve(24);
 for(; value; value/=10)
 {
  switch(value%10)
  {
   case 0:  { ret+='0'; break; }
   case 1:  { ret+='1'; break; }
   case 2:  { ret+='2'; break; }
   case 3:  { ret+='3'; break; }
   case 4:  { ret+='4'; break; }
   case 5:  { ret+='5'; break; }
   case 6:  { ret+='6'; break; }
   case 7:  { ret+='7'; break; }
   case 8:  { ret+='8'; break; }
   case 9:  { ret+='9'; break; }
  }
 }
 char c;
 size_t n=ret.size();
 size_t l=n/2;
 --n;
 for(size_t i=0; i < l; i++) { c=ret[i]; ret[i]=ret[n-i]; ret[n-i]=c; }
 return ret;
};

std::string zJSON::toString(float value)
{
 std::ostringstream s;
 s << value;
 return s.str();
};

std::string zJSON::toString(double value)
{
 std::ostringstream s;
 s << value;
 return s.str();
};

std::string zJSON::toString(long double value)
{
 std::ostringstream s;
 s << value;
 return s.str();
};

double zJSON::toDouble(const std::string &q, double def)
{
 size_t s=q.find_first_not_of(" \n\r\t\v");
 if(s == std::string::npos) return def;
 int sign=1;
 int point=0;
 unsigned exponenta=0;
 double multiplier=1.;
 double ret=0.;
 const char* p=&q[s];
 size_t n=q.size()-s;
 size_t i=0;
 char c=*p;
 switch(c)
 {
  case '-': { sign=-1; }
  case '+': { ++p; ++i; c=*p; }
 }
 if(i == n || c < '0' || c > '9') return def;
 for(; i < n; i++, ++p)
 {
  c=*p;
  if(c == '.') { if(point) { break; } point=1; continue; }
  if(c == 'e' || c == 'E')
  {
   if(point > 1) { break; }
   switch(*(p+1))
   {
    case '-': { point=4; ++p; ++i; break; }
    case '+': { point=2; ++p; ++i; break; }
    default:  { point=2; }
   }
   continue;
  }
  if(c < '0' || c > '9') break;
  if(point < 2) { ret*=10.; ret+=((unsigned char) c - '0'); if(point) { multiplier*=10.; } }
  else { exponenta*=10; exponenta+=((unsigned char) c - '0'); if(exponenta > 308) return def; }
 }
 if(multiplier != 0.0 && multiplier != 1.) { ret/=multiplier; }
 if(point & 2) { for(size_t j=0; j < exponenta; j++) { ret*=10.; } }
 else if(point & 4) { for(size_t j=0; j < exponenta; j++) { ret/=10.; } }
 ret*=sign;
 return ret;
};

int64_t zJSON::toInteger(const std::string &q, int64_t def)
{
 size_t s=q.find_first_not_of(" \n\r\t\v");
 if(s == std::string::npos) return def;
 int sign=1;
 int64_t ret=0;
 const char* p=&q[s];
 size_t n=q.size()-s;
 size_t i=0;
 char c=*p;
 switch(c)
 {
  case '-': { sign=-1; }
  case '+': { ++p; ++i; c=*p; }
 }
 if(i == n || c < '0' || c > '9') return def;
 for(; i < n; i++, ++p)
 {
  c=*p;
  if(c < '0' || c > '9') break;
  ret*=10; ret+=((unsigned char) c - '0');
 }
 ret*=sign;
 return ret;
};

zJSON* zJSON::parse(const char* src, size_t len, size_t pos)
{
 if(pos >= len) return NULL;
 return parse_json(src, len, pos);
};

zJSON* zJSON::parse(const std::string& src, size_t pos)
{
 if(pos >= src.size()) return NULL;
 return parse_json(src.c_str(), src.size(), pos);
};

zJSON* zJSON::parse(const char* src, size_t len, size_t pos, size_t& res_pos)
{
 res_pos=pos;
 if(pos >= len) return NULL;
 return parse_json(src, len, res_pos);
};

zJSON* zJSON::parse(const std::string& src, size_t pos, size_t& res_pos)
{
 res_pos=pos;
 if(pos >= src.size()) return NULL;
 return parse_json(src.c_str(), src.size(), res_pos);
};

bool zJSON::parse(zJSON& ret, const char* src, size_t len, size_t pos)
{
 zJSON* p_json=zJSON::parse(src, len, pos);
 if(p_json == NULL) return false;
 bool b=p_json->swap(ret);
 delete p_json;
 return b;
};

bool zJSON::parse(zJSON& ret, const std::string& src, size_t pos)
{
 zJSON* p_json=zJSON::parse(src, pos);
 if(p_json == NULL) return false;
 bool b=p_json->swap(ret);
 delete p_json;
 return b;
};

bool zJSON::parse(zJSON& ret, const char* src, size_t len, size_t pos, size_t& res_pos)
{
 zJSON* p_json=zJSON::parse(src, len, pos, res_pos);
 if(p_json == NULL) return false;
 bool b=p_json->swap(ret);
 delete p_json;
 return b;
};

bool zJSON::parse(zJSON& ret, const std::string& src, size_t pos, size_t& res_pos)
{
 zJSON* p_json=zJSON::parse(src, pos, res_pos);
 if(p_json == NULL) return false;
 bool b=p_json->swap(ret);
 delete p_json;
 return b;
};

zJSON::zJSON(int json_type, const std::string& json_name):
 m_parent(NULL),
 m_name(json_name),
 m_value(NULL),
 param(NULL)
{
 switch(json_type)
 {
  case zJSON::JSON_NULL: { m_value= new zJSON::zjson_null(); break; }
  case zJSON::JSON_BOOLEAN: { m_value= new zJSON::zjson_bool(); break; }
  case zJSON::JSON_INTEGER: { m_value= new zJSON::zjson_integer(); break; }
  case zJSON::JSON_NUMBER: { m_value= new zJSON::zjson_number(); break; }
  case zJSON::JSON_STRING: { m_value= new zJSON::zjson_string(); break; }
  case zJSON::JSON_ARRAY: { m_value= new zJSON::zjson_array(); break; }
  default: { m_value= new zJSON::zjson_node(); break; }
 }
};

void zJSON::create(int json_type, const std::string& json_name)
{
 m_name=json_name;
 if(m_value) { delete m_value; m_value=NULL; }
 switch(json_type)
 {
  case zJSON::JSON_NULL: { m_value= new zJSON::zjson_null(); break; }
  case zJSON::JSON_BOOLEAN: { m_value= new zJSON::zjson_bool(); break; }
  case zJSON::JSON_INTEGER: { m_value= new zJSON::zjson_integer(); break; }
  case zJSON::JSON_NUMBER: { m_value= new zJSON::zjson_number(); break; }
  case zJSON::JSON_STRING: { m_value= new zJSON::zjson_string(); break; }
  case zJSON::JSON_ARRAY: { m_value= new zJSON::zjson_array(); break; }
  default: { m_value= new zJSON::zjson_node(); break; }
 }
};

zJSON::zJSON(const std::string& json_name, bool json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_bool(json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, char json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, unsigned char json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, short json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, unsigned short json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, int json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, unsigned json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, int64_t json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer(json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, uint64_t json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_integer((int64_t) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, float json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_number((double) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, double json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_number(json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, long double json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_number((double) json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, const char* json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_string(json_value)),
 param(NULL)
{
};

zJSON::zJSON(const std::string& json_name, const std::string & json_value):
 m_parent(NULL),
 m_name(json_name),
 m_value(new zJSON::zjson_string(json_value)),
 param(NULL)
{
};

zJSON::zJSON(const zJSON& src):
 m_parent(NULL),
 m_name(src.m_name),
 m_value(src.m_value?(src.m_value->copy(this)):NULL),
 param(src.param?(src.param->copy(this)):NULL)
{
};

zJSON::~zJSON() { remove(); if(m_value) { delete m_value; m_value=NULL; } if(param) { delete param; param=NULL; } };

zJSON& zJSON::operator=(const zJSON& src)
{
 if(&src == this) return *this;
 m_name= src.m_name;
 zJSON::zjson_base* m_v=NULL;
 if(src.m_value) m_v=src.m_value->copy(this);
 if(m_value) delete m_value;
 m_value=m_v;
 if(param) { delete param; param=NULL; }
 if(src.param) param=src.param->copy(this);
 return *this;
};

zJSON& zJSON::operator=(bool json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_bool(json_value);
 return *this;
};

zJSON& zJSON::operator=(char json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(unsigned char json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(short json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(unsigned short json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(int json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(unsigned json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(int64_t json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer(json_value);
 return *this;
};

zJSON& zJSON::operator=(uint64_t json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_integer((int64_t) json_value);
 return *this;
};

zJSON& zJSON::operator=(float json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_number((double) json_value);
 return *this;
};

zJSON& zJSON::operator=(double json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_number(json_value);
 return *this;
};

zJSON& zJSON::operator=(long double json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_number((double) json_value);
 return *this;
};

zJSON& zJSON::operator=(const char* json_value)
{
 if(m_value) { delete m_value; m_value=NULL; }
 m_value=new zJSON::zjson_string(json_value);
 return *this;
};

zJSON& zJSON::operator=(const std::string & json_value)
{
 zJSON::zjson_base* m_v=new zJSON::zjson_string(json_value);
 if(m_value) delete m_value;
 m_value=m_v;
 return *this;
};

bool zJSON::swap(zJSON& src)
{
 if(&src == this) return false;
 if(root(&src) == &src || src.root(this) == this)  return false;
 m_name.swap(src.m_name);
 zJSON::zjson_base* p=m_value;
 m_value=src.m_value;
 src.m_value=p;
 m_value->set_parent(this);
 src.m_value->set_parent(&src);
 zJSON::zParamJSON* pp=param;
 param=src.param;
 src.param=pp;
 return true;
};

bool zJSON::operator==(const zJSON& src) const
{
 if(&src == this) return true;
 switch(m_value->type())
 {
  case zJSON::JSON_NULL: { return (src.type() == zJSON::JSON_NULL); }
  case zJSON::JSON_BOOLEAN: { return (src == *m_value->ptr_boolean()); }
  case zJSON::JSON_INTEGER: { return (src == *m_value->ptr_integer()); }
  case zJSON::JSON_NUMBER: { return (src == *m_value->ptr_number()); }
  case zJSON::JSON_STRING: { return (src == *m_value->ptr_string()); }
 }
 return false;
};

bool zJSON::operator==(bool json_value) const
{
 switch(m_value->type())
 {
  case zJSON::JSON_BOOLEAN: { return (*m_value->ptr_boolean() == json_value); }
  case zJSON::JSON_INTEGER: { return ((bool) *m_value->ptr_integer() == json_value); }
  case zJSON::JSON_NUMBER: { return ((bool) *m_value->ptr_number() == json_value); }
 }
 return false;
};

bool zJSON::operator==(char json_value) const { return operator==((int64_t) json_value); }

bool zJSON::operator==(unsigned char json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(short json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(unsigned short json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(int json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(unsigned json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(int64_t json_value) const
{
 switch(m_value->type())
 {
  case zJSON::JSON_INTEGER: { return (*m_value->ptr_integer() == json_value); }
  case zJSON::JSON_NUMBER: { return (*m_value->ptr_number() == (double) json_value); }
 }
 return false;
};

bool zJSON::operator==(uint64_t json_value) const { return operator==((int64_t) json_value); };

bool zJSON::operator==(float json_value) const { return operator==((double) json_value); };

bool zJSON::operator==(double json_value) const
{
 switch(m_value->type())
 {
  case zJSON::JSON_INTEGER: { return ((double) *m_value->ptr_integer() == json_value); }
  case zJSON::JSON_NUMBER: { return (*m_value->ptr_number() == json_value); }
 }
 return false;
};

bool zJSON::operator==(long double json_value) const { return operator==((double) json_value); };

bool zJSON::operator==(const char* json_value) const
{
 if(m_value->type() != zJSON::JSON_STRING) return false;
 return (*m_value->ptr_string() == json_value);
};

bool zJSON::operator==(const std::string & json_value) const
{
 if(m_value->type() != zJSON::JSON_STRING) return false;
 return (*m_value->ptr_string() == json_value);
};



















