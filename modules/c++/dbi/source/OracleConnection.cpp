/* =========================================================================
 * This file is part of dbi-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * dbi-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#if defined(USE_ORACLE)

#include "dbi/OracleConnection.h"
#include <sstream>
#include <algorithm>
#include <import/sys.h>
#include <import/str.h>

dbi::OracleConnection::OracleConnection()
{
    mEnvHandle = nullptr;
    mErrorHandle = nullptr;
    (void) OCIInitialize((ub4) OCI_DEFAULT, (dvoid *)0,
                         (dvoid * (*)(dvoid *, size_t)) 0,
                         (dvoid * (*)(dvoid *, dvoid *, size_t))0,
                         (void (*)(dvoid *, dvoid *)) 0 );

    (void) OCIEnvInit( (OCIEnv **) &mEnvHandle, OCI_DEFAULT, (size_t) 0,
                       (dvoid **) 0 );


    (void) OCIHandleAlloc( (dvoid *) mEnvHandle, (dvoid **) &mErrorHandle, OCI_HTYPE_ERROR,
                           (size_t) 0, (dvoid **) 0);

    (void) OCIHandleAlloc( (dvoid *) mEnvHandle, (dvoid **) &mContextHandle, OCI_HTYPE_SVCCTX,
                           (size_t) 0, (dvoid **) 0);
}

dbi::OracleConnection::~OracleConnection()
{
    OCIHandleFree((dvoid *)mContextHandle, OCI_HTYPE_SVCCTX);
    OCIHandleFree((dvoid *)mErrorHandle, OCI_HTYPE_ERROR);
    OCIHandleFree((dvoid *)mEnvHandle, OCI_HTYPE_ENV);
}

bool dbi::OracleConnection::connect(const std::string& database,
                                    const std::string& user,
                                    const std::string& pass,
                                    const std::string& host,
                                    unsigned int port)
{
    if (port == 0) port = 1521;
    int rc = OCILogon(mEnvHandle, mErrorHandle, &mContextHandle,
                      (const OraText*)user.c_str(), user.length(), (const OraText*)pass.c_str(), pass.length(),
                      (const OraText*)database.c_str(), database.length());
    if (rc != 0)
        return false;
    return true;
}

void dbi::OracleConnection::disconnect()
{
    OCILogoff(mContextHandle, mErrorHandle);
}

const std::string dbi::OracleConnection::getLastErrorMessage()
{
    char errbuf[100];
    memset(errbuf, 0, 100);
    int errcode;
    OCIErrorGet((dvoid *)mErrorHandle, (ub4) 1, (text *) nullptr, &errcode,
                (OraText*)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
    if (strlen(errbuf) > 0)
        return std::string(errbuf);
    return "";
}

dbi::pResultSet dbi::OracleConnection::query(const std::string& q)
{
    unsigned rowCount = 0;
    OCIStmt* sqlHandle;
    /* Allocate and prepare SQL statement */
    OCIHandleAlloc( (dvoid *) mEnvHandle, (dvoid **) &sqlHandle,
                    OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
    OCIStmtPrepare(sqlHandle, mErrorHandle, (const OraText*)q.c_str(),
                   (ub4) q.length(), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

    // Oracle is ridiculous.  This MUST be 1 for non-SELECT statements
    // but needs to be 0 to avoid prefetching the first row on a SELECT.
    ub4 val = 1;

    std::string lowerCaseQuery = q;
    transform(lowerCaseQuery.begin(), lowerCaseQuery.end(), lowerCaseQuery.begin(), tolower);

    if (lowerCaseQuery.find("select") == 0)
    {
        val = 0;
        std::string countq = lowerCaseQuery;
        int one = countq.find("select ");
        int two = countq.find(" from ");
        countq = countq.replace(one, two - one, "select count(*) ");
        OCIStmt* countHandle;
        OCIHandleAlloc( (dvoid *) mEnvHandle, (dvoid **)&countHandle,
                        OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
        OCIStmtPrepare(countHandle, mErrorHandle, (const OraText*)countq.c_str(),
                       (ub4)countq.length(), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
        OCIStmtExecute(mContextHandle, countHandle, mErrorHandle, (ub4)val, (ub4)0,
                       (CONST OCISnapshot *) nullptr, (OCISnapshot *) nullptr, OCI_DEFAULT);
        OCIDefine * defineHandle = nullptr;
        OCIDefineByPos(countHandle, &defineHandle, mErrorHandle, 1,
                       &rowCount, sizeof(rowCount), SQLT_UIN, 0, 0, 0, OCI_DEFAULT);
        OCIStmtFetch(countHandle, mErrorHandle, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
    }

    /* Execute the SQL statment */
    OCIStmtExecute(mContextHandle, sqlHandle, mErrorHandle, (ub4) val, (ub4) 0,
                   (CONST OCISnapshot *) nullptr, (OCISnapshot *) nullptr, OCI_DEFAULT);

    return dbi::pResultSet(new dbi::OracleResultSet(sqlHandle, mErrorHandle, rowCount));
}

unsigned int dbi::OracleResultSet::getNumRows()
{
    return mRowCount;
}

struct Column
{
    char   name[128];
    ub2    type;
    ub2    extType;
    ub4    fieldSize;
    char   *value;
};

dbi::Row dbi::OracleResultSet::fetchRow()
{
    dbi::Row row;
    ub4 count = 0;
    sword result = OCIAttrGet(mSQLHandle, OCI_HTYPE_STMT,
                              &count, nullptr, OCI_ATTR_PARAM_COUNT, mErrorHandle);
    Column * fields = new Column[count];
    if (result == OCI_SUCCESS)
    {
        for (int i = 0; i < (int)count; i++)
        {
            memset(fields[i].name, '\0', 128);
            fields[i].type = 0;
            fields[i].extType = SQLT_STR;
            fields[i].fieldSize = 0;
            fields[i].value = nullptr;

            OCIParam* param_handle = nullptr;
            ub4       name_len = 0;

            result = OCIParamGet(mSQLHandle, OCI_HTYPE_STMT,
                                 mErrorHandle, (dvoid **) & param_handle,
                                 i + 1);
            if (result == OCI_SUCCESS)
            {
                text * temp;
                result = OCIAttrGet(param_handle, OCI_DTYPE_PARAM,
                                    &temp, &name_len, OCI_ATTR_NAME,
                                    mErrorHandle);
                strncpy(fields[i].name, (const char*)temp, name_len);
            }

            if (result == OCI_SUCCESS)
            {
                result = OCIAttrGet(param_handle, OCI_DTYPE_PARAM,
                                    &fields[i].type, nullptr, OCI_ATTR_DATA_TYPE,
                                    mErrorHandle);
            }

            if (result == OCI_SUCCESS)
            {
                result = OCIAttrGet(param_handle, OCI_DTYPE_PARAM,
                                    &fields[i].fieldSize, nullptr, OCI_ATTR_DATA_SIZE,
                                    mErrorHandle);
            }

            if (param_handle)
            {
                OCIDescriptorFree(param_handle, OCI_DTYPE_PARAM);
            }

            ub4 fieldSize = fields[i].fieldSize;
            if (fields[i].type == SQLT_DAT)
            {
                fields[i].extType = SQLT_ODT;
                fieldSize = sizeof(OCIDate);
            }


            if (result == OCI_SUCCESS)
            {
                //char temp[1];
                int indp = -1; // This means that NULL values will not cause an error
                fields[i].value = new char[fields[i].fieldSize];
                memset(fields[i].value, '\0', fields[i].fieldSize);
                OCIDefine* defineHandle = nullptr;
                result = OCIDefineByPos(mSQLHandle, &defineHandle, mErrorHandle, i + 1,
                                        fields[i].value, fieldSize, fields[i].extType,
                                        &indp, 0, 0, OCI_DEFAULT);
            }
        }
        result = OCIStmtFetch(mSQLHandle, mErrorHandle, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
        if (result == OCI_SUCCESS || result == OCI_NO_DATA || result == OCI_SUCCESS_WITH_INFO)
        {
            for (int i = 0; i < (int)count; i++)
            {
                //std::string v = std::string(fields[i].value);
                //str::trim(v);
                if (fields[i].extType == SQLT_ODT)
                {
                    char buffer[9];
                    ub4 bufferSize = 9;

                    OCIDateToText(mErrorHandle, (const OCIDate*)fields[i].value,
                                  nullptr, 0,
                                  nullptr, 0, &bufferSize, (text*)buffer);
                    memset(fields[i].value, '\0', sizeof(OCIDate));
                    strncpy(fields[i].value, buffer, bufferSize);
                }
                row.addField((const char*)fields[i].name, fields[i].type, fields[i].fieldSize, fields[i].value);
                delete [] fields[i].value;
            }
        }
    }
    delete [] fields;
    mRowIndex++;
    return row;
}

#endif
