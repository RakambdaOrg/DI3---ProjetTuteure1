/*
 * Copyright 2010 Sébastien Aupetit <sebastien.aupetit@univ-tours.fr>
 *
 * This source code is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This source code is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this source code. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: CustomerDB.c 247 2010-09-10 10:23:07Z sebtic $
 */

#include <CustomerDB.h>
#include <CustomerRecord.h>
#include <CustomerRecordEditor.h>

const char * CUSTOMERDB_FILENAME = BASEPATH "/data/Customer.db";

CustomerDB * IMPLEMENT(CustomerDB_create)(const char * filename) {
    FILE * recordsFile = fopen(filename, "w+b");
	CustomerDB * customerDB;
	if(recordsFile == NULL)
	{
		return NULL;
	}
	if((customerDB = (CustomerDB *)malloc(sizeof(CustomerDB))) == NULL)
	{
        fatalError("Error malloc");
	}
	customerDB->file = recordsFile;
	customerDB->recordCount = 0;
    return customerDB;
}

CustomerDB * IMPLEMENT(CustomerDB_open)(const char * filename) {
	FILE * recordsFile = fopen(filename, "r+b");
	CustomerDB * customerDB;
	if(recordsFile == NULL)
	{
		return NULL;
	}
	if((customerDB = (CustomerDB *)malloc(sizeof(CustomerDB))) == NULL)
	{
		fatalError("Error malloc");
	}
	customerDB->file = recordsFile;
    fread(&(customerDB->recordCount), sizeof(int), 1, recordsFile);
	return customerDB;
}

CustomerDB * IMPLEMENT(CustomerDB_openOrCreate)(const char * filename) {
    CustomerDB * customerDB;
	if((customerDB = CustomerDB_open(filename)) == NULL && (customerDB = CustomerDB_create(filename)) == NULL)
	{
		return NULL;
	}
	return customerDB;
}

void IMPLEMENT(CustomerDB_close)(CustomerDB * customerDB) {
    fseek(customerDB->file, 0, SEEK_SET);
	fwrite(&(customerDB->recordCount), sizeof(int), 1, customerDB->file);
	fclose(customerDB->file);
}

int IMPLEMENT(CustomerDB_getRecordCount)(CustomerDB * customerDB) {
    return customerDB->recordCount;
}

char * CustomerDB_getFieldValueAsString(CustomerDB * customerDB, int recordIndex, int field) {
    char * content = NULL;
    if (customerDB != NULL) {
        CustomerRecord_FieldProperties properties = CustomerRecord_getFieldProperties(field);
        CustomerRecord record;
        CustomerRecord_init(&record);
        CustomerDB_readRecord(customerDB, recordIndex, &record);
        content = (*properties.getValue)(&record);
        CustomerRecord_finalize(&record);
    }
    return content;
}

void IMPLEMENT(CustomerDB_appendRecord)(CustomerDB * customerDB, CustomerRecord *record) {
	CustomerDB_writeRecord(customerDB, CustomerDB_getRecordCount(customerDB), record);
}

void IMPLEMENT(CustomerDB_insertRecord)(CustomerDB * customerDB, int recordIndex, CustomerRecord * record) {
    CustomerRecord customRecord;
    int i;
    for(i = CustomerDB_getRecordCount(customerDB) - 1; i >= recordIndex; i--)
    {
        CustomerDB_readRecord(customerDB, i, &customRecord);
        CustomerDB_writeRecord(customerDB, i + 1, &customRecord);
    }
    CustomerDB_writeRecord(customerDB, recordIndex, record);
}

void IMPLEMENT(CustomerDB_removeRecord)(CustomerDB * customerDB, int recordIndex) {
    CustomerRecord customRecord;
    int i;
    for(i = recordIndex + 1; i < CustomerDB_getRecordCount(customerDB); i++)
    {
        CustomerDB_readRecord(customerDB, i, &customRecord);
        CustomerDB_writeRecord(customerDB, i - 1, &customRecord);
    }
    customerDB->recordCount--;
}

void IMPLEMENT(CustomerDB_readRecord)(CustomerDB * customerDB, int recordIndex, CustomerRecord * record) {
	if(recordIndex >= CustomerDB_getRecordCount(customerDB))
	{
		fatalError("Record index isn't present");
	}
	fseek(customerDB->file, (long) (sizeof(int) + (long unsigned int)recordIndex * CUSTOMERRECORD_SIZE), SEEK_SET);
	CustomerRecord_read(record, customerDB->file);
}

void IMPLEMENT(CustomerDB_writeRecord)(CustomerDB * customerDB, int recordIndex, CustomerRecord * record) {
    if(recordIndex >= CustomerDB_getRecordCount(customerDB))
    {
        customerDB->recordCount++;
    }
	fseek(customerDB->file, (long)(sizeof(int) + (long unsigned int)recordIndex * CUSTOMERRECORD_SIZE), SEEK_SET);
	CustomerRecord_write(record, customerDB->file);
}
