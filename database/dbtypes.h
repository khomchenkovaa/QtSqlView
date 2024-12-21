#ifndef DBTYPES_H
#define DBTYPES_H

#include <QString>

namespace Db {

namespace MySQL {

inline QString typeNameById(int mysqlType) {
    switch (mysqlType) {
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
    }
    return QString("mysqlType %1").arg(mysqlType);
}

} // namespace MySQL

namespace PostgreSQL {

enum { // PostgreSQL base types
    PG_BOOL        = 16,
    PG_BYTEA       = 17,
    PG_INT8        = 20,
    PG_INT2        = 21,
    PG_INT4        = 23,
    PG_REGPROC     = 24,
    PG_XID         = 28,
    PG_CID         = 29,
    PG_FLOAT4      = 700,
    PG_FLOAT8      = 701,
    PG_ABSTIME     = 702,
    PG_RELTIME     = 703,
    PG_DATE        = 1082,
    PG_TIME        = 1083,
    PG_TIMESTAMP   = 1114,
    PG_TIMESTAMPTZ = 1184,
    PG_TIMETZ      = 1266,
    PG_BIT         = 1560,
    PG_VARBIT      = 1562,
    PG_NUMERIC     = 1700,
    PG_OID         = 2278
};

inline QString typeNameById(int pgType) {
    switch (pgType) {
    case PG_BOOL:   	 return "bool";
    case PG_BYTEA:   	 return "bytea";
    case 18:   	         return "char";
    case 19:   	         return "name";
    case PG_INT8:   	 return "int8";
    case PG_INT2:	     return "int2";
    case 22:	         return "int2vector";
    case PG_INT4:	     return "int4";
    case PG_REGPROC:	 return "regproc";
    case 25:	         return "text";
    case 26:	         return "oid";
    case 27:	         return "tid";
    case PG_XID:	     return "xid";
    case PG_CID:	     return "cid";
    case 30:	         return "oidvector";
    case 71:	         return "pg_type";
    case 75:	         return "pg_attribute";
    case 81:	         return "pg_proc";
    case 83:	         return "pg_class";
    case 86:	         return "pg_shadow";
    case 87:	         return "pg_group";
    case 88:	         return "pg_database";
    case 90:	         return "pg_tablespace";
    case 210:	         return "smgr";
    case 600:	         return "point";
    case 601:	         return "lseg";
    case 602:	         return "path";
    case 603:	         return "box";
    case 604:	         return "polygon";
    case 628:	         return "line";
    case 629:	         return "_line";
    case 650:	         return "cidr";
    case 651:	         return "_cidr";
    case PG_FLOAT4:	     return "float4";
    case PG_FLOAT8:	     return "float8";
    case PG_ABSTIME:	 return "abstime";
    case PG_RELTIME:	 return "reltime";
    case 704:	         return "tinterval";
    case 705:	         return "unknown";
    case 718:	         return "circle";
    case 719:	         return "_circle";
    case 790:	         return "money";
    case 791:	         return "_money";
    case 829:	         return "macaddr";
    case 869:	         return "inet";
    case 1000:	         return "_bool";
    case 1001:	         return "_bytea";
    case 1002:	         return "_char";
    case 1003:	         return "_name";
    case 1005:	         return "_int2";
    case 1006:	         return "_int2vector";
    case 1007:	         return "_int4";
    case 1008:	         return "_regproc";
    case 1009:	         return "_text";
    case 1010:	         return "_tid";
    case 1011:	         return "_xid";
    case 1012:	         return "_cid";
    case 1013:	         return "_oidvector";
    case 1014:	         return "_bpchar";
    case 1015:	         return "_varchar";
    case 1016:	         return "_int8";
    case 1017:	         return "_point";
    case 1018:	         return "_lseg";
    case 1019:	         return "_path";
    case 1020:	         return "_box";
    case 1021:	         return "_float4";
    case 1022:	         return "_float8";
    case 1023:	         return "_abstime";
    case 1024:	         return "_reltime";
    case 1025:	         return "_tinterval";
    case 1027:	         return "_polygon";
    case 1028:	         return "_oid";
    case 1033:	         return "aclitem";
    case 1034:	         return "_aclitem";
    case 1040:	         return "_macaddr";
    case 1041:	         return "_inet";
    case 1042:	         return "bpchar";
    case 1043:	         return "varchar";
    case PG_DATE:	     return "date";
    case PG_TIME:	     return "time";
    case PG_TIMESTAMP:	 return "timestamp";
    case 1115:	         return "_timestamp";
    case 1182:	         return "_date";
    case 1183:	         return "_time";
    case PG_TIMESTAMPTZ: return "timestamptz";
    case 1185:	         return "_timestamptz";
    case 1186:	         return "interval";
    case 1187:	         return "_interval";
    case 1231:	         return "_numeric";
    case PG_TIMETZ:	     return "timetz";
    case 1270:	         return "_timetz";
    case PG_BIT:	     return "bit";
    case 1561:	         return "_bit";
    case PG_VARBIT:	     return "varbit";
    case 1563:	         return "_varbit";
    case PG_NUMERIC:	 return "numeric";
    case 1790:	         return "refcursor";
    case 2201:	         return "_refcursor";
    case 2202:	         return "regprocedure";
    case 2203:	         return "regoper";
    case 2204:	         return "regoperator";
    case 2205:	         return "regclass";
    case 2206:	         return "regtype";
    case 2207:	         return "_regprocedure";
    case 2208:	         return "_regoper";
    case 2209:	         return "_regoperator";
    case 2210:	         return "_regclass";
    case 2211:	         return "_regtype";
    case 2249:	         return "record";
    case 2275:	         return "cstring";
    case 2276:	         return "any";
    case 2277:	         return "anyarray";
    case PG_OID:         return "void";
    case 2279:	         return "trigger";
    case 2280:	         return "language_handler";
    case 2281:	         return "internal";
    case 2282:	         return "opaque";
    case 2283:	         return "anyelement";
    }
    return QString("pgType %1").arg(pgType);
}

} // namespace PostgreSQL

namespace Sqlite {

inline QString typeNameById(int sqliteType) {
    switch (sqliteType) {
    case 1:		return "integer";
    case 2:		return "float";
    case 3:		return "text";
    case 4:		return "blob";
    case 5:		return "null";
    }
    return QString("sqliteType %1").arg(sqliteType);
}

} // namespace Sqlite

namespace Oci {

inline QString typeNameById(int ociType) {
    switch (ociType) {
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
    }
    return QString("ociType %1").arg(ociType);
}

} // namespace Oci

namespace Odbc {

enum { // ODBC types
    SQL_UNKNOWN_TYPE   = 0,
    SQL_CHAR           = 1,
    SQL_NUMERIC        = 2,
    SQL_DECIMAL        = 3,
    SQL_INTEGER        = 4,
    SQL_SMALLINT       = 5,
    SQL_FLOAT          = 6,
    SQL_REAL           = 7,
    SQL_DOUBLE         = 8,
    SQL_DATETIME       = 9,
    SQL_TIME           = 10,
    SQL_TIMESTAMP      = 11,
    SQL_VARCHAR        = 12,
    SQL_TYPE_DATE      = 91,
    SQL_TYPE_TIME      = 92,
    SQL_TYPE_TIMESTAMP = 93,
    SQL_LONGVARCHAR    = -1,
    SQL_BINARY         = -2,
    SQL_VARBINARY      = -3,
    SQL_LONGVARBINARY  = -4,
    SQL_BIGINT         = -5,
    SQL_TINYINT        = -6,
    SQL_BIT            = -7,
    SQL_WCHAR		   = -8,
    SQL_WVARCHAR	   = -9,
    SQL_WLONGVARCHAR   = -10,
    SQL_GUID		   = -11
};

inline QString typeNameById(int sqlType) {
    switch (sqlType) {
    case SQL_CHAR:           return QLatin1String("char");
    case SQL_NUMERIC:        return QLatin1String("numeric");
    case SQL_DECIMAL:        return QLatin1String("decimal");
    case SQL_INTEGER:        return QLatin1String("int");
    case SQL_SMALLINT:       return QLatin1String("smallint");
    case SQL_FLOAT:          return QLatin1String("float");
    case SQL_REAL:           return QLatin1String("real");
    case SQL_DOUBLE:         return QLatin1String("double");
    case SQL_DATETIME:       return QLatin1String("datetime");
    case SQL_TIME:           return QLatin1String("time");
    case SQL_TIMESTAMP:      return QLatin1String("timestamp");
    case SQL_VARCHAR:        return QLatin1String("varchar");
    case SQL_TYPE_DATE:      return QLatin1String("date_t");
    case SQL_TYPE_TIME:      return QLatin1String("time_t");
    case SQL_TYPE_TIMESTAMP: return QLatin1String("timestamp_t");
    case SQL_LONGVARCHAR:    return QLatin1String("longvarchar");
    case SQL_BINARY:         return QLatin1String("binary");
    case SQL_VARBINARY:      return QLatin1String("varbinary");
    case SQL_LONGVARBINARY:  return QLatin1String("longvarbinary");
    case SQL_BIGINT:         return QLatin1String("bigint");
    case SQL_TINYINT:        return QLatin1String("tinyint");
    case SQL_BIT:            return QLatin1String("bit");
    case SQL_WCHAR:          return QLatin1String("wchar");
    case SQL_WVARCHAR:       return QLatin1String("wvarchar");
    case SQL_WLONGVARCHAR:   return QLatin1String("wlongvarchar");
    case SQL_GUID:           return QLatin1String("guid");
    }
    return QString("sqlType %1").arg(sqlType);
}

} // namespace Odbc

namespace MdbTools {

enum { // QMDBTOOLS types
    MDB_BOOL     = 0x01,
    MDB_BYTE     = 0x02,
    MDB_INT      = 0x03,
    MDB_LONGINT  = 0x04,
    MDB_MONEY    = 0x05,
    MDB_FLOAT    = 0x06,
    MDB_DOUBLE   = 0x07,
    MDB_DATETIME = 0x08,
    MDB_BINARY   = 0x09,
    MDB_TEXT     = 0x0a,
    MDB_OLE      = 0x0b,
    MDB_MEMO     = 0x0c,
    MDB_REPID    = 0x0f,
    MDB_NUMERIC  = 0x10,
    MDB_COMPLEX  = 0x12
};

inline QString typeNameById(int mdbType) {
    switch (mdbType) {
    case MDB_BOOL:     return QLatin1String("bool");
    case MDB_BYTE:     return QLatin1String("byte");
    case MDB_INT:      return QLatin1String("int");
    case MDB_LONGINT:  return QLatin1String("longint");
    case MDB_MONEY:    return QLatin1String("money");
    case MDB_FLOAT:    return QLatin1String("float");
    case MDB_DOUBLE:   return QLatin1String("double");
    case MDB_DATETIME: return QLatin1String("datetime");
    case MDB_BINARY:   return QLatin1String("binary");
    case MDB_TEXT:     return QLatin1String("text");
    case MDB_OLE:      return QLatin1String("ole");
    case MDB_MEMO:     return QLatin1String("memo");
    case MDB_REPID:    return QLatin1String("repid");
    case MDB_NUMERIC:  return QLatin1String("numeric");
    case MDB_COMPLEX:  return QLatin1String("complex");
    }
    return QString("mdbType %1").arg(mdbType);
}

} // namespace MdbTools

inline QString typeNameById(const QString &driver, int t) {
    if (driver == "QMYSQL" || driver == "QMYSQL3") {
        return Db::MySQL::typeNameById(t);
    }
    if (driver  == "QPSQL" || driver == "PSQL7") {
        return Db::PostgreSQL::typeNameById(t);
    }
    if (driver  == "QSQLITE" || driver == "QSQLITE2") {
        return Db::Sqlite::typeNameById(t);
    }
    if (driver  == "QOCI" || driver == "QOCI8") {
        return Db::Oci::typeNameById(t);
    }
    if (driver  == "QODBC" || driver == "QODBC3") {
        return Db::Odbc::typeNameById(t);
    }
    if (driver  == "QMDBTOOLS") {
        return Db::MdbTools::typeNameById(t);
    }

    return QString("Type %1").arg(t);
}

} // namespace Db

#endif // DBTYPES_H
