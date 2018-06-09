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

#ifndef __zJSON_h
#define __zJSON_h 1

#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <stdint.h>

class zJSON
{
 friend class zjson_base;
 friend class zjson_null;
 friend class zjson_bool;
 friend class zjson_integer;
 friend class zjson_number;
 friend class zjson_string;
 friend class zjson_array;
 friend class zjson_node;

public:

enum
{
 JSON_NULL=0,
 JSON_BOOLEAN,
 JSON_INTEGER,
 JSON_NUMBER,
 JSON_STRING,
 JSON_ARRAY,
 JSON_NODE
};

static std::string toString(short value);
static std::string toString(unsigned short value);
static std::string toString(int value);
static std::string toString(unsigned value);
static std::string toString(int64_t value);
static std::string toString(uint64_t value);
static std::string toString(float value);
static std::string toString(double value);
static std::string toString(long double value);

static double toDouble(const std::string &q, double def=0.0);
static int64_t toInteger(const std::string &q, int64_t def=0);

static zJSON* parse(const char* src, size_t len, size_t start_pos=0);
static zJSON* parse(const std::string& src, size_t start_pos=0);
static zJSON* parse(const std::string& src, size_t start_pos, size_t& res_pos);
static zJSON* parse(const char* src, size_t len, size_t start_pos, size_t& res_pos);
static bool parse(zJSON& ret, const char* src, size_t len, size_t start_pos=0);
static bool parse(zJSON& ret, const std::string& src, size_t start_pos=0);
static bool parse(zJSON& ret, const char* src, size_t len, size_t start_pos, size_t& res_pos);
static bool parse(zJSON& ret, const std::string& src, size_t start_pos, size_t& res_pos);
/*
Parses JSON text src start at start_pos and returns the JSON object which is the root.
The function returns pointer or reference(ret) to the JSON object.
If error occurrence NULL will be return and true – successfully , false – unsuccessfully.
*/
 explicit zJSON(int json_type = zJSON::JSON_NODE, const std::string& json_name="");
 void create(int json_type = zJSON::JSON_NODE, const std::string& json_name="");

 zJSON(const std::string& json_name, bool json_value);
//JSON_BOOLEAN
 zJSON(const std::string& json_name, char json_value);
 zJSON(const std::string& json_name, unsigned char json_value);
 zJSON(const std::string& json_name, short json_value);
 zJSON(const std::string& json_name, unsigned short json_value);
 zJSON(const std::string& json_name, int json_value);
 zJSON(const std::string& json_name, unsigned json_value);
 zJSON(const std::string& json_name, int64_t json_value);
 zJSON(const std::string& json_name, uint64_t json_value);
//JSON_INTEGER
 zJSON(const std::string& json_name, float json_value);
 zJSON(const std::string& json_name, double json_value);
 zJSON(const std::string& json_name, long double json_value);
//JSON_NUMBER
 zJSON(const std::string& json_name, const char* json_value);
 zJSON(const std::string& json_name, const std::string & json_value);
//JSON_STRING
 zJSON(const zJSON& src);
/*
Creates JSON object with parent=NULL, name=json_name and assign JSON object to specific value and type.
json_type — enum { JSON_NULL=0, JSON_BOOLEAN, JSON_INTEGER, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_NODE }.
*/
 virtual ~zJSON();

 zJSON& operator=(const zJSON& src);
 zJSON& operator=(bool json_value);
 zJSON& operator=(char json_value);
 zJSON& operator=(unsigned char json_value);
 zJSON& operator=(short json_value);
 zJSON& operator=(unsigned short json_value);
 zJSON& operator=(int json_value);
 zJSON& operator=(unsigned json_value);
 zJSON& operator=(int64_t json_value);
 zJSON& operator=(uint64_t json_value);
 zJSON& operator=(float json_value);
 zJSON& operator=(double json_value);
 zJSON& operator=(long double json_value);
 zJSON& operator=(const char* json_value);
 zJSON& operator=(const std::string & json_value);
/*
Assign JSON object to specific value and type.
*/
 bool swap(zJSON& src);
/*
Swaps the contents of two objects(swaps only pointers). If objects are placed on common branch they will not be
swapped.
Returns true if successfully , false if unsuccessfully.
*/
 int type() const { return m_value->type(); };
/*
Returns type of object: enum { JSON_NULL=0, JSON_BOOLEAN, JSON_INTEGER, JSON_NUMBER,
JSON_STRING, JSON_ARRAY, JSON_NODE }.
*/
 std::string& name() { return m_name; };
 const std::string& name() const { return m_name; };
/*
Returns reference to object name.
*/
 zJSON* parent() const { return m_parent; };
/*
Returns pointer to parent object. If no parent, returns NULL.
*/
 zJSON* remove() { if(m_parent == NULL) return this; return m_parent->m_value->remove(this); };
/*
Sets parent to NULL and returns this. See functions insert, push_back, swap.
*/
 bool operator==(const zJSON& src) const;
 bool operator==(bool json_value) const;
 bool operator==(char json_value) const;
 bool operator==(unsigned char json_value) const;
 bool operator==(short json_value) const;
 bool operator==(unsigned short json_value) const;
 bool operator==(int json_value) const;
 bool operator==(unsigned json_value) const;
 bool operator==(int64_t json_value) const;
 bool operator==(uint64_t json_value) const;
 bool operator==(float json_value) const;
 bool operator==(double json_value) const;
 bool operator==(long double json_value) const;
 bool operator==(const char* json_value) const;
 bool operator==(const std::string & json_value) const;

friend bool operator==(bool json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(char json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(unsigned char json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(short json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(unsigned short json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(int json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(unsigned json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(int64_t json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(uint64_t json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(float json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(double json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(long double json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(const char* json_value, const zJSON& src) { return src.operator==(json_value); };
friend bool operator==(const std::string & json_value, const zJSON& src) { return src.operator==(json_value); };
/*
Compares this(src) and json_value.
Returns true if successfully , false if unsuccessfully.
*/
 bool operator!=(const zJSON& src) const { return !operator==(src); };
 bool operator!=(bool json_value) const { return !operator==(json_value); };
 bool operator!=(char json_value) const { return !operator==(json_value); };
 bool operator!=(unsigned char json_value) const { return !operator==(json_value); };
 bool operator!=(short json_value) const { return !operator==(json_value); };
 bool operator!=(unsigned short json_value) const { return !operator==(json_value); };
 bool operator!=(int json_value) const { return !operator==(json_value); };
 bool operator!=(unsigned json_value) const { return !operator==(json_value); };
 bool operator!=(int64_t json_value) const { return !operator==(json_value); };
 bool operator!=(uint64_t json_value) const { return !operator==(json_value); };
 bool operator!=(float json_value) const { return !operator==(json_value); };
 bool operator!=(double json_value) const { return !operator==(json_value); };
 bool operator!=(long double json_value) const { return !operator==(json_value); };
 bool operator!=(const char* json_value) const { return !operator==(json_value); };
 bool operator!=(const std::string & json_value) const { return !operator==(json_value); };

friend bool operator!=(bool json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(char json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(unsigned char json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(short json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(unsigned short json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(int json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(unsigned json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(int64_t json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(uint64_t json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(float json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(double json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(long double json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(const char* json_value, const zJSON& src) { return src.operator!=(json_value); };
friend bool operator!=(const std::string & json_value, const zJSON& src) { return src.operator!=(json_value); };
/*
Compares this(src) and json_value.
Returns true if successfully , false if unsuccessfully.
*/                    
 const bool* ptr_boolean() const { return m_value->ptr_boolean(); };
 bool* ptr_boolean() { return m_value->ptr_boolean(); };
 const int64_t* ptr_integer() const { return m_value->ptr_integer(); };
 int64_t* ptr_integer() { return m_value->ptr_integer(); };
 const double* ptr_number() const { return m_value->ptr_number(); };
 double* ptr_number() { return m_value->ptr_number(); };
 const std::string* ptr_string() const { return m_value->ptr_string(); };
 std::string* ptr_string() { return m_value->ptr_string(); };
/*
Returns pointer to plain types from JSON object. If plain types is not found the function returns NULL.
*/
 bool as_boolean() const { return m_value->as_boolean(); };
 int64_t as_integer() const { return m_value->as_integer(); };
 double as_number() const { return m_value->as_number(); };
 std::string as_string() const { return m_value->as_string(); };
 bool as_string(std::string& ret) const { return m_value->as_string(ret); };
/*
Returns pointer to plain types from JSON object. If plain types is not found the function returns NULL.
*/
 const zJSON* operator[](size_t pos) const { return m_value->at(pos); };
 zJSON* operator[](size_t pos) { return m_value->at(pos); };
 const zJSON* at(size_t pos) const { return m_value->at(pos); };
 zJSON* at(size_t pos) { return m_value->at(pos); };
/*
Returns pointer to JSON object are placed at pos. If object is not found the function returns NULL.
*/
 const zJSON* front() const { return m_value->front(); };
 zJSON* front() { return m_value->front(); };
/*
Returns a pointer to the first element. If element is not found the function returns NULL.
*/
 const zJSON* back() const { return m_value->back(); };
 zJSON* back() { return m_value->back(); };
/*
Returns a pointer to the last element. If element is not found the function returns NULL.
*/
 zJSON* root() { if(m_parent == NULL) return this; return m_parent->root(); };
 zJSON* root(zJSON* prn) { if(m_parent == NULL) return this; if(m_parent == prn) return prn; return m_parent->root(prn); };
/*
Returns a pointer to the root object or pointer to prn object if it is placed between this and root.
*/
 size_t index() const { if(m_parent == NULL) return std::string::npos; return m_parent->m_value->index(this); };
/*
Returns index of object. If parent is NULL the function returns std::string::npos.
*/
 size_t find(const std::string& json_name, size_t start_pos=0) const { return m_value->find(json_name, start_pos); };
 zJSON* search(const std::string& json_name, size_t start_pos=0) const { return m_value->search(json_name, start_pos); };
/*
Searches object with json_name through the children starting start_pos. The functions return std::string::npos and NULL if the
child does not exist.
*/
 bool empty() const { return m_value->empty(); };
/*
Indicates when the object has any children. If the object is not JSON_ARRAY or JSON_NODE the function returns true.
*/
 size_t size() const { return m_value->size(); };
/*
Returns the number of children that the object has. The function returns 0 for anything other than JSON_ARRAY or JSON_NODE.
*/
 void clear() { return m_value->clear(); };
/*
Removes all elements from the object (which are destroyed), leaving the container with a size of 0.
*/
 zJSON* insert(size_t pos, const zJSON& val) { return m_value->insert(pos, val, this); };
 zJSON* insert(size_t pos, zJSON* val) { return m_value->insert(pos, val, this); };
/*
The object is extended by inserting the element val before the element at the specified pos, effectively increasing the
container size by the one. If pos >= size the object will be added to back.
Returns pointer to inserted object. If object is not inserted the function returns NULL.
At the first function the copy of the val will be created. At the second function the object val can be inserted if its parent is NULL.
The function remove() sets parent to NULL.
*/
 zJSON* push_back(const zJSON& val) { return m_value->push_back(val, this); };
 zJSON* push_back(zJSON* val) { return m_value->push_back(val, this); };
/*
Adds a new element at the end of the object, after its current last element. The object is extended by adding the element
val at the back, effectively increasing the container size by the one.
Returns pointer to added object. If object is not added the function returns NULL.
At the first function the copy of the val will be created. At the second function the object val can be added if its parent is
NULL. The function remove() sets parent to NULL.
*/
 bool erase(size_t pos) { return m_value->erase(pos); };
/*
Removes from the object a single element at pos.
This effectively reduces the container size by the one element, which are destroyed.
*/
 bool pop_back() { return m_value->pop_back(); };
/*
Removes the last element in the object, effectively reducing the container size by one.
This destroys the removed element.
*/
 void write(std::string& ret) const { m_value->write(ret, this); };
/*
Returns JSON text, with no white space.
*/
 void write_formatted(std::string& ret) const { m_value->write_formatted(ret, this, 0); };
/*
Returns JSON text that has been indented and prettied up so that it can be easily read and modified by humans.
*/

 class zParamJSON
 {
  public:
   zParamJSON() {};
   virtual ~zParamJSON() { return; };
   virtual zParamJSON* copy(zJSON* prn) const=0;

   private:
    zParamJSON(const zParamJSON& src);
    zParamJSON& operator=(const zParamJSON& src);

 };

mutable zJSON::zParamJSON* param;
/*
zJSON::zParamJSON* param allows you to bind your object to the zJSON object. To accomplish this task, your class
must be inherited from the class zJSON::zParamJSON and the function zParamJSON * copy (zJSON* prn) const must be
defined. Function returns a copy of the object or NULL, and takes as a parameter a zJSON* pointer to the new parent of
the returned object.
*/

protected:

 zJSON* m_parent;
 std::string m_name;

 class zjson_base
 {
  public:
   zjson_base() {};
   virtual ~zjson_base() { return; };
   virtual zjson_base* copy(zJSON* prn) const=0;
   virtual int type() const=0;
   virtual void set_parent(zJSON* p)=0;

   virtual bool* ptr_boolean()=0;
   virtual int64_t* ptr_integer()=0;
   virtual double* ptr_number()=0;
   virtual std::string* ptr_string()=0;

   virtual bool as_boolean() const=0;
   virtual int64_t as_integer() const=0;
   virtual double as_number() const=0;
   virtual std::string as_string() const=0;
   virtual bool as_string(std::string& ret) const=0;

   virtual const zJSON* at(size_t pos) const=0;
   virtual zJSON* at(size_t pos)=0;
   virtual const zJSON* front() const=0;
   virtual zJSON* front()=0;
   virtual const zJSON* back() const=0;
   virtual zJSON* back()=0;
   virtual size_t index(const zJSON* const p) const=0;
   virtual size_t find(const std::string& json_name, size_t start_pos) const=0;
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const=0;
  
   virtual bool empty() const=0;
   virtual size_t size() const=0;

   virtual void clear()=0;
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn)=0;
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn)=0;
   virtual zJSON* push_back(const zJSON& val, zJSON* prn)=0;
   virtual zJSON* push_back(zJSON* val, zJSON* prn)=0;
   virtual bool erase(size_t pos)=0;
   virtual bool pop_back()=0;
   virtual zJSON* remove(zJSON* p)=0;

   virtual void write(std::string& ret, const zJSON* const prn) const=0;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const=0;

  private:
  zjson_base(const zjson_base& src);
  zjson_base& operator=(const zjson_base& src);
 };

mutable zJSON::zjson_base* m_value;

 class zjson_null: public zJSON::zjson_base
 {
  public:
   zjson_null(): zJSON::zjson_base() { };
   virtual ~zjson_null() { return; };
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_null(); };
   virtual int type() const { return zJSON::JSON_NULL; };
   virtual void set_parent(zJSON* p) { return; };

   virtual bool* ptr_boolean() { return NULL; };
   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool as_boolean() const { return false; };
   virtual int64_t as_integer() const { return 0; };
   virtual double as_number() const { return 0.0; };
   virtual std::string as_string() const { return ""; };
   virtual bool as_string(std::string& ret) const { ret=""; return false; };

   virtual const zJSON* at(size_t pos) const { return NULL; };
   virtual zJSON* at(size_t pos) { return NULL; };
   virtual const zJSON* front() const { return NULL; };
   virtual zJSON* front() { return NULL; };
   virtual const zJSON* back() const { return NULL; };
   virtual zJSON* back() { return NULL; };
   virtual size_t index(const zJSON* const p) const { return std::string::npos; };
   virtual size_t find(const std::string& json_name, size_t start_pos) const { return std::string::npos; };
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const { return NULL; };
  
   virtual bool empty() const { return true; };
   virtual size_t size() const { return 0; };

   virtual void clear() { return; };
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(zJSON* val, zJSON* prn) { return NULL; };
   virtual bool erase(size_t pos) { return NULL; };
   virtual bool pop_back() { return NULL; };
   virtual zJSON* remove(zJSON* p) { return p; };

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;
 };

 class zjson_bool: public zJSON::zjson_base
 {
  public:
   zjson_bool(bool b=false): zJSON::zjson_base(), value(b) { };
   virtual ~zjson_bool() { return; };
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_bool(value); };
   virtual int type() const { return zJSON::JSON_BOOLEAN; };
   virtual void set_parent(zJSON* p) { return; };

   virtual bool* ptr_boolean() { return &value; };
   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool as_boolean() const { return value; };
   virtual int64_t as_integer() const { if(value) return 1; return 0; };
   virtual double as_number() const { if(value) return 1.0; return 0.0; };
   virtual std::string as_string() const { if(value) return "true"; return "false"; };
   virtual bool as_string(std::string& ret) const { (value)?(ret="true"):(ret="false"); return true; };

   virtual const zJSON* at(size_t pos) const { return NULL; };
   virtual zJSON* at(size_t pos) { return NULL; };
   virtual const zJSON* front() const { return NULL; };
   virtual zJSON* front() { return NULL; };
   virtual const zJSON* back() const { return NULL; };
   virtual zJSON* back() { return NULL; };
   virtual size_t index(const zJSON* const p) const { return std::string::npos; };
   virtual size_t find(const std::string& json_name, size_t start_pos) const { return std::string::npos; };
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const { return NULL; };
  
   virtual bool empty() const { return true; };
   virtual size_t size() const { return 0; };

   virtual void clear() { return; };
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(zJSON* val, zJSON* prn) { return NULL; };
   virtual bool erase(size_t pos) { return NULL; };
   virtual bool pop_back() { return NULL; };
   virtual zJSON* remove(zJSON* p) { return p; };

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   bool value;
 };

 class zjson_integer: public zJSON::zjson_base
 {
  public:
   zjson_integer(int64_t v=0): zJSON::zjson_base(), value(v) { };
   virtual ~zjson_integer() { return; };
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_integer(value); };
   virtual int type() const { return zJSON::JSON_INTEGER; };
   virtual void set_parent(zJSON* p) { return; };

   virtual bool* ptr_boolean() { return NULL; };
   virtual int64_t* ptr_integer() { return &value; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool as_boolean() const { return value; };
   virtual int64_t as_integer() const { return value; };
   virtual double as_number() const { return value; };
   virtual std::string as_string() const { return zJSON::toString(value); };
   virtual bool as_string(std::string& ret) const { ret=zJSON::toString(value); return true; };

   virtual const zJSON* at(size_t pos) const { return NULL; };
   virtual zJSON* at(size_t pos) { return NULL; };
   virtual const zJSON* front() const { return NULL; };
   virtual zJSON* front() { return NULL; };
   virtual const zJSON* back() const { return NULL; };
   virtual zJSON* back() { return NULL; };
   virtual size_t index(const zJSON* const p) const { return std::string::npos; };
   virtual size_t find(const std::string& json_name, size_t start_pos) const { return std::string::npos; };
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const { return NULL; };
  
   virtual bool empty() const { return true; };
   virtual size_t size() const { return 0; };

   virtual void clear() { return; };
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(zJSON* val, zJSON* prn) { return NULL; };
   virtual bool erase(size_t pos) { return NULL; };
   virtual bool pop_back() { return NULL; };
   virtual zJSON* remove(zJSON* p) { return p; };

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   int64_t value;
 };

 class zjson_number: public zJSON::zjson_base
 {
  public:
   zjson_number(double d=0.0): zJSON::zjson_base(), value(d) { };
   virtual ~zjson_number() { return; };
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_number(value); };
   virtual int type() const { return zJSON::JSON_NUMBER; };
   virtual void set_parent(zJSON* p) { return; };

   virtual bool* ptr_boolean() { return NULL; };
   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return &value; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool as_boolean() const { return (bool) value; };
   virtual int64_t as_integer() const { return (int64_t) value; };
   virtual double as_number() const { return value; };
   virtual std::string as_string() const { return zJSON::toString(value); };
   virtual bool as_string(std::string& ret) const { ret=zJSON::toString(value); return true; };

   virtual const zJSON* at(size_t pos) const { return NULL; };
   virtual zJSON* at(size_t pos) { return NULL; };
   virtual const zJSON* front() const { return NULL; };
   virtual zJSON* front() { return NULL; };
   virtual const zJSON* back() const { return NULL; };
   virtual zJSON* back() { return NULL; };
   virtual size_t index(const zJSON* const p) const { return std::string::npos; };
   virtual size_t find(const std::string& json_name, size_t start_pos) const { return std::string::npos; };
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const { return NULL; };
  
   virtual bool empty() const { return true; };
   virtual size_t size() const { return 0; };

   virtual void clear() { return; };
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(zJSON* val, zJSON* prn) { return NULL; };
   virtual bool erase(size_t pos) { return NULL; };
   virtual bool pop_back() { return NULL; };
   virtual zJSON* remove(zJSON* p) { return p; };

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   double value;
 };

 class zjson_string: public zJSON::zjson_base
 {
  public:
   zjson_string(const char* s=""): zJSON::zjson_base(), value(s) { };
   zjson_string(const std::string &s): zJSON::zjson_base(), value(s) { };
   virtual ~zjson_string() { return; };
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_string(value); };
   virtual int type() const { return zJSON::JSON_STRING; };
   virtual void set_parent(zJSON* p) { return; };

   virtual bool* ptr_boolean() { return NULL; };
   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return &value; };

   virtual bool as_boolean() const { return value.size(); };
   virtual int64_t as_integer() const { return zJSON::toInteger(value); };
   virtual double as_number() const { return zJSON::toDouble(value); };
   virtual std::string as_string() const { return value; };
   virtual bool as_string(std::string& ret) const { ret=value; return true; };

   virtual const zJSON* at(size_t pos) const { return NULL; };
   virtual zJSON* at(size_t pos) { return NULL; };
   virtual const zJSON* front() const { return NULL; };
   virtual zJSON* front() { return NULL; };
   virtual const zJSON* back() const { return NULL; };
   virtual zJSON* back() { return NULL; };
   virtual size_t index(const zJSON* const p) const { return std::string::npos; };
   virtual size_t find(const std::string& json_name, size_t start_pos) const { return std::string::npos; };
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const { return NULL; };
  
   virtual bool empty() const { return true; };
   virtual size_t size() const { return 0; };

   virtual void clear() { return; };
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(const zJSON& val, zJSON* prn) { return NULL; };
   virtual zJSON* push_back(zJSON* val, zJSON* prn) { return NULL; };
   virtual bool erase(size_t pos) { return NULL; };
   virtual bool pop_back() { return NULL; };
   virtual zJSON* remove(zJSON* p) { return p; };

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   std::string  value;
 };

 class zjson_array: public zJSON::zjson_base
 {
  public:
   zjson_array(): zJSON::zjson_base(), value() { };
   zjson_array(const std::vector<zJSON*>& v, zJSON* prn);
   virtual ~zjson_array();
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_array(value, prn); };
   virtual int type() const { return zJSON::JSON_ARRAY; };
   virtual void set_parent(zJSON* p);

   virtual bool* ptr_boolean() { return NULL; };
   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool as_boolean() const { return value.size(); };
   virtual int64_t as_integer() const { return (int64_t) value.size(); };
   virtual double as_number() const { return (double) value.size(); };
   virtual std::string as_string() const { return ""; };
   virtual bool as_string(std::string& ret) const { ret=""; return false; };

   virtual const zJSON* at(size_t pos) const { if(pos < value.size()) return value[pos]; return NULL; };
   virtual zJSON* at(size_t pos) { if(pos < value.size()) return value[pos]; return NULL; };
   virtual const zJSON* front() const { if(value.size()) return value[0]; return NULL; };
   virtual zJSON* front() { if(value.size()) return value[0]; return NULL; };
   virtual const zJSON* back() const { if(value.size()) return value[value.size()-1]; return NULL; };
   virtual zJSON* back() { if(value.size()) return value[value.size()-1]; return NULL; };
   virtual size_t index(const zJSON* const p) const;
   virtual size_t find(const std::string& json_name, size_t start_pos) const;
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const;
  
   virtual bool empty() const { return value.empty(); };
   virtual size_t size() const { return value.size(); };

   virtual void clear();
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn);
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn);
   virtual zJSON* push_back(const zJSON& val, zJSON* prn);
   virtual zJSON* push_back(zJSON* val, zJSON* prn);
   virtual bool erase(size_t pos);
   virtual bool pop_back();
   virtual zJSON* remove(zJSON* p);

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   std::vector<zJSON*> value;
 };

 class zjson_node: public zJSON::zjson_base
 {
  public:
   zjson_node(): zJSON::zjson_base(), value() { };
   zjson_node(const std::vector<zJSON*>& v, zJSON* prn);
   virtual ~zjson_node();
   virtual zJSON::zjson_base* copy(zJSON* prn) const { return new zjson_node(value, prn); };
   virtual int type() const { return zJSON::JSON_NODE; };
   virtual void set_parent(zJSON* p);

   virtual int64_t* ptr_integer() { return NULL; };
   virtual double* ptr_number() { return NULL; };
   virtual std::string* ptr_string() { return NULL; };

   virtual bool* ptr_boolean() { return NULL; };
   virtual bool as_boolean() const { return value.size(); };
   virtual int64_t as_integer() const { return (int64_t) value.size(); };
   virtual double as_number() const { return (double) value.size(); };
   virtual std::string as_string() const { return ""; };
   virtual bool as_string(std::string& ret) const { ret=""; return false; };

   virtual const zJSON* at(size_t pos) const { if(pos < value.size()) return value[pos]; return NULL; };
   virtual zJSON* at(size_t pos) { if(pos < value.size()) return value[pos]; return NULL; };
   virtual const zJSON* front() const { if(value.size()) return value[0]; return NULL; };
   virtual zJSON* front() { if(value.size()) return value[0]; return NULL; };
   virtual const zJSON* back() const { if(value.size()) return value[value.size()-1]; return NULL; };
   virtual zJSON* back() { if(value.size()) return value[value.size()-1]; return NULL; };
   virtual size_t index(const zJSON* const p) const;
   virtual size_t find(const std::string& json_name, size_t start_pos) const;
   virtual zJSON* search(const std::string& json_name, size_t start_pos) const;
  
   virtual bool empty() const { return value.empty(); };
   virtual size_t size() const { return value.size(); };

   virtual void clear();
   virtual zJSON* insert(size_t pos, const zJSON& val, zJSON* prn);
   virtual zJSON* insert(size_t pos, zJSON* val, zJSON* prn);
   virtual zJSON* push_back(const zJSON& val, zJSON* prn);
   virtual zJSON* push_back(zJSON* val, zJSON* prn);
   virtual bool erase(size_t pos);
   virtual bool pop_back();
   virtual zJSON* remove(zJSON* p);

   virtual void write(std::string& ret, const zJSON* const prn) const;
   virtual void write_formatted(std::string& ret, const zJSON* const prn, size_t level) const;

  protected:
   std::vector<zJSON*> value;
 };

};

#endif //__zJSON_h




