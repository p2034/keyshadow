// Copyright (c) 2022 Kandakov Danil (p2034 or the_lll_end)
// https://github.com/p2034



#ifndef NOTE_WRITABLE_H
#define NOTE_WRITABLE_H



#include <stdint.h>
#include <string>



#define NOTE_DEFAULT_CODE 0



/**
 * @class Writable
 * @brief abstarct class for writable data
 * 
 * @code
 *    < Code > | < Name Length > |       < Name >      | < Data Length > |      < Data >
 *    2 bytes  |    1 bytes      | "Name Length" bytes |     4 bytes     | "Data Length" bytes  
 */
template<class Derived>
class Writable {
protected:
  uint8_t name_length;
  uint8_t* name;

  uint32_t data_length;

  uint32_t writeHeader(uint8_t* buff);
  uint32_t readHeader(uint8_t* buff);
  virtual uint32_t writeData(uint8_t* buff) = 0;
  virtual uint32_t readData(uint8_t* buff) = 0;

  void copyHeader(const Derived& object);
  void copyHeader(Derived&& object);
  virtual void copyData(const Derived& object) = 0;
  virtual void copyData(Derived&& object) = 0;

public:
  Writable();
  Writable(std::string name);

  void setName(std::string name);

  void copy(const Derived& object);
  void copy(Derived&& object);
  virtual Derived* copy() = 0;

  virtual uint16_t code() = 0;
  virtual uint32_t size() = 0; //<- full size of writed data
  
  uint32_t write(uint8_t* buff);
  uint32_t read(uint8_t* buff);

  virtual ~Writable();
};



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  temporary because I can't solve linker problem
  help
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



template<class Derived>
Writable<Derived>::Writable() {
  this->name_length = 0;
  this->name = nullptr;
}



template<class Derived>
Writable<Derived>::Writable(std::string name) {
  this->name_length = name.length();
  memcpy(this->name, name.c_str(), this->name_length * sizeof(uint8_t));
}



/**
 * @brief set name to the writable object
 */
template<class Derived>
void Writable<Derived>::setName(std::string name) {
  if (this->name != nullptr)
    delete[] this->name;

  this->name_length = name.length();
  memcpy(this->name, name.c_str(), this->name_length * sizeof(uint8_t));
}



/**
 * @brief delete name
 */
template<class Derived>
Writable<Derived>::~Writable() {
  if (name != nullptr)
    delete[] name;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                              PARSE
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/**
 * @brief set data in buffer
 */
template<class Derived>
uint32_t Writable<Derived>::write(uint8_t* buff) {
  size_t padding = this->writeHeader(buff);
  padding += this->writeData(buff + padding);
  return padding;
}



/**
 * @brief parse data from buffer
 */
template<class Derived>
uint32_t Writable<Derived>::read(uint8_t* buff) {
  size_t padding = this->readHeader(buff);
  padding += this->readData(buff + padding);
  return padding;
}



/**
 * @brief write header in the buffer in writable format
 */
template<class Derived>
uint32_t Writable<Derived>::writeHeader(uint8_t* buff) {
  size_t padding = 0;

  uint16_t code = this->code();
  memcpy(buff + padding, &code, sizeof(uint16_t));
  padding += sizeof(uint16_t);
  memcpy(buff + padding, &name_length, sizeof(uint8_t));
  padding += sizeof(uint8_t);
  memcpy(buff + padding, name, name_length * sizeof(uint8_t));
  padding += name_length * sizeof(uint8_t);
  memcpy(buff + padding, &data_length, sizeof(uint32_t));
  padding += sizeof(uint32_t);

  return padding;
}



/**
 * @brief read header from the buffer in writable format
 */
template<class Derived>
uint32_t Writable<Derived>::readHeader(uint8_t* buff) {
  size_t padding = 0;

  // write code
  uint16_t code;
  memcpy(&code, buff + padding, sizeof(uint16_t));
  if (code != this->code())
    throw std::invalid_argument("Wrong onject type.");
  padding += sizeof(uint16_t);
  // write name length
  memcpy(&name_length, buff + padding, sizeof(uint8_t));
  padding += sizeof(uint8_t);
  // write name
  memcpy(name, buff + padding, name_length * sizeof(uint8_t));
  padding += name_length * sizeof(uint8_t);
  // write data length
  memcpy(&data_length, buff + padding, sizeof(uint32_t));
  padding += sizeof(uint32_t);

  return padding;
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                              COPY
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/**
 * @brief copy funnction
 */
template<class Derived>
void Writable<Derived>::copy(const Derived& object) {
  this->copyHeader(object);
  this->copyData(object);
}



/**
 * @brief move funnction
 */
template<class Derived>
void Writable<Derived>::copy(Derived&& object) {
  this->copyHeader(object);
  this->copyData(object);
}



/**
 * @brief copy header function
 */
template<class Derived>
void Writable<Derived>::copyHeader(const Derived& object) {
  static_assert(std::is_base_of<Writable<Derived>, Derived>::value, "Derived not derived from Writable.");

  if (this->name != nullptr)
    delete[] this->name;

  this->name_length = object.name_length;
  this->name = (uint8_t*) new uint8_t[this->name_length];

  this->data_length = object.data_length;
}

/**
 * @brief move header function
 */
template<class Derived>
void Writable<Derived>::copyHeader(Derived&& object) {
  static_assert(std::is_base_of<Writable<Derived>, Derived>::value, "Derived not derived from Writable.");

  if (this->name != nullptr)
    delete[] this->name;

  this->name_length = object.name_length;
  this->name = object.name;

  object.name_length = 0;
  object.name = nullptr;

  this->data_length = object.data_length;
}

#endif