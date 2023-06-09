#ifndef DBTYPES_H
#define DBTYPES_H

#include <QString>

class DbTypes
{
public:
    static inline QString getName(QString driver, int t) {
        if (driver == "QMYSQL" || driver == "QMYSQL3") {
            return getNameMySql(t);
        }
        if (driver  == "QPSQL" || driver == "PSQL7") {
            return getNamePostgreSql(t);
        }
        if (driver  == "QSQLITE" || driver == "QSQLITE2") {
            return getNameSqlite(t);
        }
        if (driver  == "QOCI" || driver == "QOCI8") {
            return getNameOracle(t);
        }

        return QString("Type %1").arg(t);
    }

protected:
    static inline QString getNameSqlite(int t) {
        switch (t) {
        case 1:		return "integer";
        case 2:		return "float";
        case 3:		return "text";
        case 4:		return "blob";
        case 5:		return "null";

        default:
            return QString("Type %1").arg(t);
        }
    }

    static inline QString getNameMySql(int t) {
        switch (t) {
        case 0:		return "decimal";
        case 1:		return "tiny";
        case 2:		return "short";
        case 3:		return "integer";
        case 4:		return "float";
        case 5:		return "double";
        case 6:		return "null";
        case 7:		return "timestamp";
        case 8:		return "longlong";
        case 9:		return "int24";
        case 10:	return "date";
        case 11:	return "time";
        case 12:	return "datetime";
        case 13:	return "year";
        case 14:	return "newdate";
        case 247:	return "enum";
        case 248:	return "set";
        case 249:	return "tinytext";
        case 250:	return "mediumtext";
        case 251:	return "longtext";
        case 252:	return "text";
        case 253:	return "varchar";
        case 254:	return "char";

        default:
            return QString("Type %1").arg(t);
        }
    }

    static inline QString getNamePostgreSql(int t) {
        switch (t) {
        case 16:   	return "bool";
        case 17:   	return "bytea";
        case 18:   	return "char";
        case 19:   	return "name";
        case 20:   	return "int8";
        case 21:	return "int2";
        case 22:	return "int2vector";
        case 23:	return "int4";
        case 24:	return "regproc";
        case 25:	return "text";
        case 26:	return "oid";
        case 27:	return "tid";
        case 28:	return "xid";
        case 29:	return "cid";
        case 30:	return "oidvector";
        case 71:	return "pg_type";
        case 75:	return "pg_attribute";
        case 81:	return "pg_proc";
        case 83:	return "pg_class";
        case 86:	return "pg_shadow";
        case 87:	return "pg_group";
        case 88:	return "pg_database";
        case 90:	return "pg_tablespace";
        case 210:	return "smgr";
        case 600:	return "point";
        case 601:	return "lseg";
        case 602:	return "path";
        case 603:	return "box";
        case 604:	return "polygon";
        case 628:	return "line";
        case 629:	return "_line";
        case 650:	return "cidr";
        case 651:	return "_cidr";
        case 700:	return "float4";
        case 701:	return "float8";
        case 702:	return "abstime";
        case 703:	return "reltime";
        case 704:	return "tinterval";
        case 705:	return "unknown";
        case 718:	return "circle";
        case 719:	return "_circle";
        case 790:	return "money";
        case 791:	return "_money";
        case 829:	return "macaddr";
        case 869:	return "inet";
        case 1000:	return "_bool";
        case 1001:	return "_bytea";
        case 1002:	return "_char";
        case 1003:	return "_name";
        case 1005:	return "_int2";
        case 1006:	return "_int2vector";
        case 1007:	return "_int4";
        case 1008:	return "_regproc";
        case 1009:	return "_text";
        case 1010:	return "_tid";
        case 1011:	return "_xid";
        case 1012:	return "_cid";
        case 1013:	return "_oidvector";
        case 1014:	return "_bpchar";
        case 1015:	return "_varchar";
        case 1016:	return "_int8";
        case 1017:	return "_point";
        case 1018:	return "_lseg";
        case 1019:	return "_path";
        case 1020:	return "_box";
        case 1021:	return "_float4";
        case 1022:	return "_float8";
        case 1023:	return "_abstime";
        case 1024:	return "_reltime";
        case 1025:	return "_tinterval";
        case 1027:	return "_polygon";
        case 1028:	return "_oid";
        case 1033:	return "aclitem";
        case 1034:	return "_aclitem";
        case 1040:	return "_macaddr";
        case 1041:	return "_inet";
        case 1042:	return "bpchar";
        case 1043:	return "varchar";
        case 1082:	return "date";
        case 1083:	return "time";
        case 1114:	return "timestamp";
        case 1115:	return "_timestamp";
        case 1182:	return "_date";
        case 1183:	return "_time";
        case 1184:	return "timestamptz";
        case 1185:	return "_timestamptz";
        case 1186:	return "interval";
        case 1187:	return "_interval";
        case 1231:	return "_numeric";
        case 1266:	return "timetz";
        case 1270:	return "_timetz";
        case 1560:	return "bit";
        case 1561:	return "_bit";
        case 1562:	return "varbit";
        case 1563:	return "_varbit";
        case 1700:	return "numeric";
        case 1790:	return "refcursor";
        case 2201:	return "_refcursor";
        case 2202:	return "regprocedure";
        case 2203:	return "regoper";
        case 2204:	return "regoperator";
        case 2205:	return "regclass";
        case 2206:	return "regtype";
        case 2207:	return "_regprocedure";
        case 2208:	return "_regoper";
        case 2209:	return "_regoperator";
        case 2210:	return "_regclass";
        case 2211:	return "_regtype";
        case 2249:	return "record";
        case 2275:	return "cstring";
        case 2276:	return "any";
        case 2277:	return "anyarray";
        case 2278:	return "void";
        case 2279:	return "trigger";
        case 2280:	return "language_handler";
        case 2281:	return "internal";
        case 2282:	return "opaque";
        case 2283:	return "anyelement";

        default:
            return QString("Type %1").arg(t);
        }
    }

    static inline QString getNameOracle(int t) {
        switch (t) {
        /* input data types */
        case   1: return "VARCHAR2"; // (ORANET TYPE) character string
        case   2: return "NUMBER";   // (ORANET TYPE) oracle numeric
        case   3: return "INT";      // (ORANET TYPE) integer
        case   4: return "FLT";      // (ORANET TYPE) Floating point number
        case   5: return "STR";      // zero terminated string
        case   6: return "VNU";      // NUM with preceding length byte
        case   7: return "PDN";      // (ORANET TYPE) Packed Decimal Numeric
        case   8: return "LONG";     // long
        case   9: return "VCS";      // Variable character string
        case  10: return "NON";      // Null/empty PCC Descriptor entry
        case  11: return "RID";      // rowid
        case  12: return "DATE";     // date in oracle format
        case  15: return "VBI";      // binary in VCS format
        case  21: return "BFLOAT";   // Native Binary float
        case  22: return "BDOUBLE";  // Native binary double
        case  23: return "BIN";      // binary data(DTYBIN)
        case  24: return "LBI";      // long binary
        case  68: return "UIN";      // unsigned integer
        case  91: return "SLS";      // Display sign leading separate
        case  94: return "LVC";      // Longer longs (char)
        case  95: return "LVB";      // Longer long binary
        case  96: return "AFC";      // Ansi fixed char
        case  97: return "AVC";      // Ansi Var char
        case 100: return "IBFLOAT";  // binary float canonical
        case 101: return "IBDOUBLE"; // binary double canonical
        case 102: return "CUR";      // cursor  type
        case 104: return "ROWID";    // rowid descriptor
        case 105: return "LAB";      // label type
        case 106: return "OSL";      // oslabel type

        case 108: return "NTY";    // named object type
        case 110: return "REF";    // ref type
        case 112: return "CLOB";   // character lob
        case 113: return "BLOB";   // binary lob
        case 114: return "BFILEE"; // binary file lob
        case 115: return "CFILEE"; // character file lob
        case 116: return "RSET";   // result set type
        case 119: return "JSON";   // native JSON type
        case 122: return "NCO";    // named collection type (varray or nested table)
        case 155: return "VST";    // OCIString type
        case 156: return "ODT";    // OCIDate type

        /* datetimes and intervals */
        case 184: return "ANSIDATE";          // ANSI Date
        case 185: return "TIME";          // TIME
        case 186: return "TIME_TZ";       // TIME WITH TIME ZONE
        case 187: return "TIMESTAMP";     // TIMESTAMP
        case 188: return "TIMESTAMP_TZ";  // TIMESTAMP WITH TIME ZONE
        case 189: return "INTERVAL_YM";   // INTERVAL YEAR TO MONTH
        case 190: return "INTERVAL_DS";   // INTERVAL DAY TO SECOND
        case 232: return "TIMESTAMP_LTZ"; // TIMESTAMP WITH LOCAL TZ

        case 241: return "PNTY"; // pl/sql representation of named types

        /* some pl/sql specific types */
        case 250: return "REC"; // pl/sql 'record' (or %rowtype)
        case 251: return "TAB"; // pl/sql 'indexed table'
        case 252: return "BOL"; // pl/sql 'boolean'

        default:
            return QString("Type %1").arg(t);
        }
    }


};

#endif // DBTYPES_H
