/*
 * File:   CSV.h
 * Author: martin
 *
 * Created on 17. prosinec 2014, 10:36
 */

#ifndef CSV_H
#define	CSV_H

#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace CSVparse {

    enum DataType {
        FLOAT_TYPE
        , STRING_TYPE
    };

    class CSV {
    private:
        std::string filename;
        std::string separator;
        std::string emptyValue;
        char stringSeparator;
        char decimalChar;
        bool headerOn;
        bool rowNamesOn;
        unsigned int rowCount;
        unsigned int columnCount;

        std::vector<std::string> header;
        std::vector<std::string> rowNames;
        std::map<unsigned int, DataType> columnIdxDataType;
        std::map<std::string, unsigned int> columnNameColumnIdx;
        std::map<std::string, unsigned int> rowNameRowIdx;

        std::map<unsigned int, std::vector<double> > floatData;
        std::map<unsigned int, std::vector<std::string> > stringData;

        void saveToken(std::string &token, unsigned int columnIdx, DataType type);
        void generateRows(unsigned int count);

    public:

        CSV(std::string filename = ""
                , std::string sep = ";"
                , std::string emptyValue = "NA"
                , bool header = true
                , bool rowNames = false
                , char stringSeparator = '"'
                , char decimalChar = '.'
                );

        void loadData();
        void loadData(const std::string &filename);
        void write(std::ostream &stream);
        void write(const std::string &filename);
        void writeRow(std::ostream &stream, unsigned int number);
        void writeHeader(std::ostream &stream);
        unsigned int getRowCount();
        unsigned int getColumnCount();
        std::vector<std::string> getColumnNames();
        std::vector<std::string> getRowNames();
        unsigned int getColumnIdx(const std::string &colName);
        unsigned int getRowIdx(const std::string &rowName);
        const std::map<unsigned int, DataType>& getTypeMap();
        const std::vector<std::string>& getHeader();

        // these methods should be generic, maybe next time :)
        const std::vector<double>& getFloatData(unsigned int columnIdx);
        const std::vector<std::string>& getStringData(unsigned int columnIdx);
        const std::vector<double>& getFloatData(const std::string &colName);
        const std::vector<std::string>& getStringData(const std::string &colName);
        void addFloatData(const std::string &colName, const std::vector<double> &data);
        void addStringData(const std::string &colName, const std::vector<std::string> &data);

        std::string getEmptyValue();

        void setFilename(const std::string &filename);
    };


}

#endif	/* CSV_H */

