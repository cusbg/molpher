
#include "CSV.h"
#include "DataConverter.h"

#include <fstream>
#include <stdexcept>
#include "math.h"
#include <assert.h>

using namespace std;

typedef CSVparse::DataConverter DConv;

void CSVparse::CSV::saveToken(string &token, unsigned int columnIdx, CSVparse::DataType type) {
    if (type == CSVparse::FLOAT_TYPE) {
        assert(columnIdxDataType[columnIdx] == CSVparse::FLOAT_TYPE);
        if (token.compare(emptyValue) == 0) {
            floatData[columnIdx].push_back(NAN);
        } else {
            floatData[columnIdx].push_back(DConv::ValToDouble(token));
        }
    } else {
        assert(columnIdxDataType[columnIdx] == CSVparse::STRING_TYPE);
        DConv::trim(token, stringSeparator);
        stringData[columnIdx].push_back(token);
    }
}

void CSVparse::CSV::loadData() {
    // TODO: ignore separator, if it is enclosed within stringSeparator
    ifstream infile(filename.c_str());
    if (infile.is_open()) {      
        std::string s;
        bool header_read = false;
        bool first_pass = true;
        unsigned int row_counter = 0;
        map<unsigned int, vector<string> > postponed;
        while (std::getline(infile, s)) {
            s.append(separator);
            string token;
            string::size_type pos = 0;
            unsigned int columnIdx = 0;
            bool first_value = true;
            while ((pos = s.find(separator)) != string::npos) {
                token = s.substr(0, pos);
                DConv::trim(token);
                if (headerOn && !header_read || (first_pass && !(first_value && rowNamesOn))) {
                    columnCount = columnIdx + 1;
                }
                if (headerOn && !header_read) {
                    DConv::trim(token, stringSeparator);
                    this->header.push_back(token);
                    this->columnNameColumnIdx[token] = columnIdx;
                } else if (first_value && rowNamesOn) {
                    DConv::trim(token, stringSeparator);
                    rowNames.push_back(token);
                    if (headerOn) {
                        rowNameRowIdx[token] = row_counter - 1;
                    } else {
                        rowNameRowIdx[token] = row_counter;
                    }
                } else if (postponed.find(columnIdx) == postponed.end()
                        && columnIdxDataType.find(columnIdx) == columnIdxDataType.end()) {
                    if (token.compare(emptyValue) == 0) {
                        postponed[columnIdx] = vector<string>();
                        postponed[columnIdx].push_back(token);
                    } else if (DConv::IsNumeric(token, decimalChar, stringSeparator)) {
                        columnIdxDataType[columnIdx] = CSVparse::FLOAT_TYPE;
                        floatData[columnIdx] = vector<double>();
                        saveToken(token, columnIdx, CSVparse::FLOAT_TYPE);
                    } else {
                        columnIdxDataType[columnIdx] = CSVparse::STRING_TYPE;
                        stringData[columnIdx] = vector<string>();
                        saveToken(token, columnIdx, CSVparse::STRING_TYPE);
                    }
                } else if (postponed.find(columnIdx) != postponed.end()) {
                    if (token.compare(emptyValue) == 0) {
                        postponed[columnIdx].push_back(token);
                    } else if (DConv::IsNumeric(token, decimalChar, stringSeparator)) {
                        columnIdxDataType[columnIdx] = CSVparse::FLOAT_TYPE;
                        floatData[columnIdx] = vector<double>();
                        vector<string>::iterator it;
                        for (it = postponed[columnIdx].begin(); it != postponed[columnIdx].end(); it++) {
                            saveToken(*it, columnIdx, CSVparse::FLOAT_TYPE);
                        }
                        saveToken(token, columnIdx, CSVparse::FLOAT_TYPE);
                        postponed.erase(columnIdx);
                    } else {
                        columnIdxDataType[columnIdx] = CSVparse::STRING_TYPE;
                        stringData[columnIdx] = vector<string>();
                        vector<string>::iterator it;
                        for (it = postponed[columnIdx].begin(); it != postponed[columnIdx].end(); it++) {
                            saveToken(*it, columnIdx, CSVparse::STRING_TYPE);
                        }
                        saveToken(token, columnIdx, CSVparse::STRING_TYPE);
                        postponed.erase(columnIdx);
                    }
                } else if (token.compare(emptyValue) != 0) {
                    saveToken(token, columnIdx, columnIdxDataType[columnIdx]);
                } else if (token.compare(emptyValue) == 0) {
                    saveToken(token, columnIdx, columnIdxDataType[columnIdx]);
                } else {
                    assert(false);
                }
                
                s.erase(0, pos + separator.length());
                if (headerOn && !header_read || !(first_value && rowNamesOn)) {
                    ++columnIdx;
                }
                first_value = false;
            }
            header_read = true;
            first_pass = false;
            ++row_counter;
        }
        
        // set rowCount and create custom header, if it is not read
        if (headerOn) {
            rowCount = row_counter - 1;
        } else {
            rowCount = row_counter;
            for (unsigned int i = 0; i != columnCount; i++) {
                string name = DConv::ValToString(i + 1);
                header.push_back(name);
                columnNameColumnIdx[name] = i;
            }
        }
        
        if (!rowNamesOn) {
            for (unsigned int i = 0; i != rowCount; i++) {
                string name = DConv::ValToString(i + 1);
                rowNames.push_back(name);
                rowNameRowIdx[name] = i;
            }
        }
        
        // if all values in a column are empty, save NAN instead
        map<unsigned int, vector<string> >::iterator it;
        for (it = postponed.begin(); it != postponed.end(); it++) {
            assert(columnIdxDataType.find(it->first) == columnIdxDataType.end());
            columnIdxDataType[it->first] = CSVparse::FLOAT_TYPE;
            floatData[it->first] = vector<double>();
            vector<string>::iterator it2;
            for (it2 = it->second.begin() ; it2 != it->second.end(); it2++) {
                saveToken(*it2, it->first, CSVparse::FLOAT_TYPE);
            }
        }
        assert(header.size() == columnCount);
        assert(rowCount == rowNames.size());
        assert(columnCount == (stringData.size() + floatData.size()));
        assert(columnIdxDataType.size() == columnCount);
        assert(columnNameColumnIdx.size() == columnCount);
    } else {
        throw runtime_error("Error opening file: " + filename);
    }
}

void CSVparse::CSV::write(ostream &stream) {
    if (headerOn) {
        writeHeader(stream);
    }
    for (unsigned int rowIdx = 0; rowIdx != rowCount; rowIdx++) {
        writeRow(stream, rowIdx);
    }
}

void CSVparse::CSV::write(const string &filename) {
    setFilename(filename);
    ofstream outstream(filename.c_str());
    write(outstream);
}

void CSVparse::CSV::writeHeader(ostream &stream) {
    std::vector<string>::iterator it;
    for (it = header.begin(); it != header.end(); it++) {
        stream << *it;
        if (header.end() != it + 1) {
            stream << separator;
        }
    }
    stream << endl;
}

void CSVparse::CSV::writeRow(ostream &stream, unsigned int rowNumber) {
    if (rowNumber >= rowCount) {
        throw runtime_error("Index out of range: " + DConv::ValToString(rowNumber));
    }
    if (rowNamesOn) {
        stream << rowNames[rowNumber] << separator;
    }
    unsigned int column_count = columnIdxDataType.size();
    for (unsigned int colIdx = 0; colIdx != column_count; colIdx++) {
        switch (columnIdxDataType[colIdx]) {
            case CSVparse::FLOAT_TYPE:
                if (floatData[colIdx].size() - 1 < rowNumber) {
                    stream << emptyValue;
                } else if (isnan(floatData[colIdx][rowNumber]) != 0) {
                    stream << emptyValue;
                } else {
                    stream << floatData[colIdx][rowNumber];
                }
                break;
            case CSVparse::STRING_TYPE:
                if (stringData[colIdx].size() - 1 < rowNumber) {
                    stream << emptyValue;
                } else if (emptyValue.compare(stringData[colIdx][rowNumber]) == 0) {
                    stream << emptyValue;
                } else {
                    stream << stringSeparator << stringData[colIdx][rowNumber] << stringSeparator;
                }
                break;
        }
        if (colIdx != column_count - 1) {
            stream << separator;
        }
    }
    stream << endl;
}

unsigned int CSVparse::CSV::getColumnCount() {
    return columnCount;
}

unsigned int CSVparse::CSV::getRowCount() {
    return rowCount;
}

vector<string> CSVparse::CSV::getColumnNames() {
    return header;
}

vector<string> CSVparse::CSV::getRowNames() {
    return rowNames;
}

unsigned int CSVparse::CSV::getColumnIdx(const string& colName) {
    return columnNameColumnIdx[colName];
}

unsigned int CSVparse::CSV::getRowIdx(const string& rowName) {
    return rowNameRowIdx[rowName];
}

const map<unsigned int, CSVparse::DataType>& CSVparse::CSV::getTypeMap() {
    return columnIdxDataType;
}

const vector<string>& CSVparse::CSV::getHeader() {
    return header;
}

const vector<double>& CSVparse::CSV::getFloatData(unsigned int columnIdx) {
    if (columnIdx >= columnCount) {
        throw runtime_error("Index out of range: " + DConv::ValToString(columnIdx));
    }
    if (columnIdxDataType[columnIdx] == CSVparse::FLOAT_TYPE) {
        return floatData[columnIdx];
    } else {
        throw runtime_error("Column not of float type: " + header[columnIdx]);
    }
}

const vector<double>& CSVparse::CSV::getFloatData(const string &colName) {
    if (columnNameColumnIdx.find(colName) == columnNameColumnIdx.end()) {
        throw runtime_error("Column of this name does not exist: " + colName);
    }
    return getFloatData(columnNameColumnIdx[colName]);
}

const vector<string>& CSVparse::CSV::getStringData(unsigned int columnIdx) {
    if (columnIdx >= columnCount) {
        throw runtime_error("Index out of range: " + DConv::ValToString(columnIdx));
    }
    if (columnIdxDataType[columnIdx] == CSVparse::STRING_TYPE) {
        return stringData[columnIdx];
    } else {
        throw runtime_error("Column not of string type: " + header[columnIdx]);
    }
}

const vector<string>& CSVparse::CSV::getStringData(const string &colName) {
    if (columnNameColumnIdx.find(colName) == columnNameColumnIdx.end()) {
        throw runtime_error("Column of this name does not exist: " + colName);
    }
    return getStringData(columnNameColumnIdx[colName]);
}

string CSVparse::CSV::getEmptyValue() {
    return emptyValue;
}

CSVparse::CSV::CSV(
        string filename
        , string sep
        , string emptyValue
        , bool header
        , bool rowName
        , char stringSep
        , char decimalChar
        ) :
filename(filename)
, separator(sep)
, emptyValue(emptyValue)
, stringSeparator(stringSep)
, decimalChar(decimalChar)
, headerOn(header)
, rowNamesOn(rowName)
, columnCount(0)
, rowCount(0) {
    if (!filename.empty()) {
        loadData();
    }
}

void CSVparse::CSV::loadData(const string &filename) {
    this->filename = filename;
    rowCount = 0;
    columnCount = 0;
    header.clear();
    rowNames.clear();
    columnIdxDataType.clear();
    columnNameColumnIdx.clear();
    rowNameRowIdx.clear();
    floatData.clear();
    stringData.clear();
    loadData();
}

void CSVparse::CSV::setFilename(const string &filename) {
    this->filename = filename;
}

void CSVparse::CSV::generateRows(unsigned int count) {
    if (rowCount < count) {
        rowCount = count;
        for (unsigned int i = 0; i < rowCount; i++) {
            rowNames.push_back(DConv::ValToString(i+1));
            rowNameRowIdx[DConv::ValToString(i+1)] = i;
        }
    }
}

void CSVparse::CSV::addFloatData(const string &colName, const vector<double> &data) {
    if (columnNameColumnIdx.find(colName) == columnNameColumnIdx.end()) {
        generateRows(data.size());
        ++columnCount;
        header.push_back(colName);
        columnIdxDataType[columnCount - 1] = CSVparse::FLOAT_TYPE;
        columnNameColumnIdx[colName] = columnCount - 1;
        floatData[columnCount - 1] = data;
    } else if (columnIdxDataType[columnNameColumnIdx[colName]] == CSVparse::FLOAT_TYPE) {
        floatData[columnNameColumnIdx[colName]].insert(floatData[columnNameColumnIdx[colName]].end(), data.begin(), data.end());
        generateRows(floatData[columnNameColumnIdx[colName]].size());
    } else {
        assert(false);
    }
}

void CSVparse::CSV::addStringData(const string &colName, const vector<string> &data) {
    if (columnNameColumnIdx.find(colName) == columnNameColumnIdx.end()) {
//        assert(rowCount == data.size());
        generateRows(data.size());
        ++columnCount;
        header.push_back(colName);
        columnIdxDataType[columnCount - 1] = CSVparse::STRING_TYPE;
        columnNameColumnIdx[colName] = columnCount - 1;
        stringData[columnCount - 1] = data;
    } else if (columnIdxDataType[columnNameColumnIdx[colName]] == CSVparse::STRING_TYPE) {
//        assert(rowCount == data.size() + stringData.size());
        stringData[columnNameColumnIdx[colName]].insert(stringData[columnNameColumnIdx[colName]].end(), data.begin(), data.end());
        generateRows(stringData[columnNameColumnIdx[colName]].size());
    } else {
        assert(false);
    }
    
}
