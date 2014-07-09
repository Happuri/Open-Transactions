#ifndef BPRINTER_TABLE_PRINTER_H_
#define BPRINTER_TABLE_PRINTER_H_

#include "../utils.hpp" // debug, SplitString()

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

namespace bprinter {
class endl{};
/** \class TablePrinter

  Print a pretty table into your output of choice.

  Usage:
    TablePrinter tp(&std::cout);
    tp.AddColumn("Name", 25);
    tp.AddColumn("Age", 3);
    tp.AddColumn("Position", 30);

    tp.PrintHeader();
    tp << "Dat Chu" << 25 << "Research Assistant";
    tp << "John Doe" << 26 << "Professional Anonymity";
    tp << "Jane Doe" << tp.SkipToNextLine();
    tp << "Tom Doe" << 7 << "Student";
    tp.PrintFooter();

  \todo Add support for padding in each table cell
  */
class TablePrinter{
public:
  TablePrinter(std::ostream * output, const std::string & separator = "|");
  ~TablePrinter();

  int get_num_columns() const;
  int get_table_width() const;
  void set_separator(const std::string & separator);

  void AddColumn(const std::string & header_name, int column_width);
  void PrintHeader();
  void PrintFooter();
  void SetTableColor(const std::string & color) { this->table_color=color; }
  void SetContentColor(const std::string & color) { this->content_color=color; }
  void SetBorderColor(const std::string & color) { this->border_color=color; }
  std::vector <std::string> Format(std::string input, int l);
  TablePrinter& operator<<(endl input){
    while (j_ != 0){
      *this << "";
    }
    return *this;
  }

  // Can we merge these?
  TablePrinter& operator<<(float input);
  TablePrinter& operator<<(double input);

  template<typename T> TablePrinter& operator<<(T input){
    if (j_ == 0) // beginning of the table
      *out_stream_ << border_color << "|" << table_color;
    using namespace std;
    // Convering input to string
    ostringstream convert;
    convert << input ;
    string toFormat = convert.str();

    // Leave 3 extra space: One for negative sign, one for zero, one for decimal
    /*out_stream_ << content_color << std::setw(column_widths_.at(j_))
    	<< Format(toFormat) ;*/

    *out_stream_ << content_color << std::setw(column_widths_.at(j_));
    _note("Set point at " << column_widths_.at(j_));
    if(toFormat.length() < column_widths_.at(j_)) {
			_note("Printing table without formatting");
			*out_stream_ << input;
    }

    else {
    	_mark("Printing table with formatting! Max lenght is: " << column_widths_.at(j_));
    	vector <string> Formatted = Format(toFormat,column_widths_.at(j_));

    	for (auto tmp : Formatted)  {
    		//_note("Set point at " << column_widths_.at(j_));

    		//*out_stream_ << border_color << separator_ << table_color;
    		*out_stream_ << tmp ;
    		*out_stream_ << border_color <<  "|\n" << table_color;

    		*out_stream_ << content_color << std::setw(31);
    		*out_stream_ << border_color << "|" << table_color;

    		//*this << tmp;
    		i_ = i_ + 1;

    		//*out_stream_ << content_color << std::setw(column_widths_.at(j_));
    		//*out_stream_ << content_color << std::setw(column_widths_.at(j_));      //j_ = 0;
    	}

    }

    if (j_ == get_num_columns()-1){ // end row
      *out_stream_ << border_color <<  "|\n" << table_color;
      i_ = i_ + 1;
      j_ = 0;
    } else { // only separator
      *out_stream_ << border_color << separator_ << table_color;
      j_ = j_ + 1;
    }

    return *this;
  }

private:
  void PrintHorizontalLine();

  template<typename T> void OutputDecimalNumber(T input);

  std::ostream * out_stream_;
  std::vector<std::string> column_headers_;
  std::vector<int> column_widths_;
  std::string separator_;

  std::string table_color;
  std::string no_color;
  std::string content_color;
  std::string border_color;

  int i_; // index of current row
  int j_; // index of current column

  int table_width_;
};

}

#include "impl/table_printer.tpp.h"
#endif
