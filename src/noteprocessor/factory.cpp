// Copyright (c) 2022 Kandakov Danil (p2034 or the_lll_end)
// https://github.com/p2034



#include "factory.h"



/**
 * @brief factory for Writable objects, generate object with it's code
 */
Writable* factory(uint16_t code) {
  Writable* object = nullptr;

  if (code == NOTE_WRITABLEARRAY_CODE)
    object = (WritableArray*) new WritableArray;
  else if (code == NOTE_WRITABLELOGINNOTE_CODE)
    object = (WritableLoginNote*) new WritableLoginNote;
  else
    throw std::invalid_argument("Unknown type of note (" + std::to_string(code) + ")\n" + 
                                "Update your application, or contact developers.");

  return object;
}